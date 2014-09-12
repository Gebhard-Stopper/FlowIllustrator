/*
 *	Copyright (C) 2014, Max Planck Institut für Informatik, Saarbrücken.
 *	Implementation: 2014, Gebhard Stopper [ gebhard.stopper@gmail.com ]
 *	
 *	If you perform any changes on this file, please append your name to 
 *	the List of people who worked on this file.
 *
 *	If you add or modify functions or variable, please do not forget to
 *	add/update the doxygen documentation.
 *
 *	This file is part of FlowIllustrator.
 *
 *	FlowIllustrator is free software: you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation, either version 3 of the License, or
 *	(at your option) any later version.
 *
 *	FlowIllustrator is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *	GNU General Public License for more details.
 *
 *	You should have received a copy of the GNU General Public License
 *	along with FlowIllustrator.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "StdAfx.h"
#include "SVGConverter.h"
#include "VortexObj.h"
#include "Triangle.h"
#include "StreamLine.h"
#include "PathLine.h"
#include "StreakLine.h"
#include "TimeLine.h"
#include "Rectangle.h"
#include "FlowIllustratorDoc.h"
#include <cctype>


CSVGConverter::CSVGConverter(void)
{
}


CSVGConverter::~CSVGConverter(void)
{
}

BOOL CSVGConverter::_checkFileName(const CString &strOriginalFileName, CString &strRealFileName)
{
	CFile file;

	if ( ! file.Open( strOriginalFileName, CFile::modeRead ) )
	{
		AfxMessageBox( _T("unable to open file") );
		return FALSE;
	}
	int nFileLen = (int)file.GetLength();

	// Allocate buffer for binary file data
	unsigned char* pBuffer = new unsigned char[nFileLen + 2];
	nFileLen = file.Read( pBuffer, nFileLen );
	file.Close();
	pBuffer[nFileLen] = '\0';
	pBuffer[nFileLen+1] = '\0'; // in case 2-byte encoded

	// Windows Unicode file is detected if starts with FEFF
	if ( pBuffer[0] == 0xff && pBuffer[1] == 0xfe )
	{
		// Contains byte order mark, so assume wide char content
		// non _UNICODE builds should perform UCS-2 (wide char) to UTF-8 conversion here
		strRealFileName = (LPCWSTR)(&pBuffer[2]);
		//csNotes += _T("File starts with hex FFFE, assumed to be wide char format. ");
	}
	else
	{
		// _UNICODE builds should perform UTF-8 to UCS-2 (wide char) conversion here
		strRealFileName = (LPCSTR)pBuffer;
	}
	delete [] pBuffer;

	return TRUE;
}

BOOL CSVGConverter::LoadSVG(const CString &strFileName, CDocument &document)
{
	CString strRealName;
	BOOL bResult = FALSE;

	if (_checkFileName(strFileName, strRealName))
	{
		CFlowIllustratorDoc &doc = reinterpret_cast<CFlowIllustratorDoc&>(document);
		CDrawingObjectMngr *pMngr = doc.GetDrawingObjectMngr();

		doc.IsLoadingSVG( TRUE );

		CSimpleXML svgDoc;
		bResult = svgDoc.LoadXML(strFileName);
		if (bResult)
		{
			shared_ptr<CSimpleXMLNode> pRoot = svgDoc.GetRootNode();

			shared_ptr<CSimpleXMLNode> svgNode = pRoot->PopChildNode(_T("svg"));
			if (svgNode)
			{
				shared_ptr<CSimpleXMLNode> transformNode = svgNode->PopChildNode(_T("g"));

				if (transformNode)
				{
					shared_ptr<CSimpleXMLNode> timeStep = transformNode->PopChildNode(_T("timestep"));
					CString strTimeStep = timeStep->strNodeValue;

					_ReconstructObjects(transformNode, pMngr);

					doc.GotoFrame( _tstoi(strTimeStep) );
				}
			}
		}
		doc.IsLoadingSVG( FALSE );
	}
	
	return bResult;
}

void CSVGConverter::_ReconstructObjects(shared_ptr<CSimpleXMLNode> objectNode, CDrawingObjectMngr *pMngr)
{
	hash_map<unsigned int, CDrawingObject*>	objCache;
	CDrawingObject *pNewObj;

	for (auto iter = objectNode->GetFirstChild(); iter != objectNode->GetLastChild(); ++iter)
	{
		shared_ptr<CSimpleXMLAttribute> attrType		= (*iter)->PopAttribute( DrawingObjNames.GetParamName(DOP_TYPE) );
		shared_ptr<CSimpleXMLAttribute> attrID			= (*iter)->PopAttribute(_T("id"));
		shared_ptr<CSimpleXMLAttribute> attrParentID	= (*iter)->PopAttribute(_T("parent"));
		
		unsigned int nID( (attrID)? _extractHexAttrib(attrID->strAttribValue, 0) : 0 );
		unsigned int parentID( (attrParentID)? _extractHexAttrib(attrParentID->strAttribValue, 0) : 0 );

		if (attrType)
		{
			int nObjType (_tstoi(attrType->strAttribValue));
			switch ( nObjType )
			{
			case DO_TRIANGLE:
				pNewObj = _getTriangle( (*iter) ); break;
			case DO_ELLIPSE:
				pNewObj = _getEllipse( (*iter) ); break;
			case DO_RECTANGLE:
				pNewObj = _getRectangle( (*iter) ); break;
			case DO_VORTEX:
				pNewObj = _getVortex( (*iter) ); break;
			case DO_STREAMLINE:
				pNewObj = _getStreamLine( (*iter) ); break;
			case DO_PATHLINE:
				pNewObj = _getPathLine( (*iter) ); break;
			case DO_STREAKLINE:
				pNewObj = _getStreakLine( (*iter) ); break;
			case DO_TIMELINE:
				pNewObj = _getTimeLine( (*iter) ); break;
			case DO_SPEEDLINE:
				pNewObj = _getSpeedLine( (*iter) ); break;
			default:
				pNewObj = nullptr;
			}

			if (pNewObj) 
			{
				objCache.insert( pair<unsigned int, CDrawingObject*>(nID, pNewObj) );

				if (parentID != 0) 
				{
					CDrawingObject *pParent = objCache.at(parentID);
					if (pParent) {	//ensure we actually have the parent
						bool bSuccess(false);

						//Is the cild a speed line?
						switch (pParent->GetType()) 
						{
							case DO_VORTEX:
								if (pNewObj->GetType() == DO_SPEEDLINE) {
									reinterpret_cast<CVortexObj*>(pParent)->AddChild( reinterpret_cast<CSpeedLine*>(pNewObj) );
									bSuccess = true;
								}
								break;
							case DO_TIMELINE:
								if (pNewObj->GetType() == DO_SPEEDLINE) {
									reinterpret_cast<CTimeLine*>(pParent)->AddTrajectory( reinterpret_cast<CSpeedLine*>(pNewObj) );
									bSuccess = true;
								}
								break;			
						}
						if (!bSuccess) {	//normal child object
							pParent->AddChild(pNewObj);
						}
					}
				}
				else 
				{
					pMngr->Add(pNewObj);
				}
			}
		}
	}
}

void CSVGConverter::_getParams(__in shared_ptr<CSimpleXMLNode> node, __out CDrawingObjectParams& params)
{
	for (auto iter = node->GetFirstAttribute(); iter != node->GetLastAttribute(); ++iter)
	{
		DrawinObjectParamName nParamName ( DrawingObjNames.GetParam((*iter)->strAttribName) );

		if (nParamName != DOP_INVALID) {
			params.SetValue( nParamName, _parseValue(nParamName, (*iter)->strAttribValue) );
		}
	}
}

CSimpleVariant CSVGConverter::_parseValue (DrawinObjectParamName nParamName, const CString& strVal)
{
	switch ( DrawingObjNames.GetParamDataType(nParamName) )
	{
		case DOT_UNSIGNED_INTEGER:
			return unsigned( _tstoi(strVal) );
		case DOT_FLOAT:
			return _tstof( strVal );
		case DOT_BOOLEAN:
			return bool( _tstoi( strVal ) );
		case DOT_COLOR:
			return _extractFloatColor( strVal );
		case DOT_POINT:
			return _extractPointf( strVal );
		case DOT_VECTOR:
			return _extractVector2D( strVal );
		case DOT_INTEGER:
		default:
			return _tstoi( strVal );
	}
}

CDrawingObject* CSVGConverter::_getTriangle(__in shared_ptr<CSimpleXMLNode> node)
{
	float fRotation(0.0f);
	floatColor color;

	auto pAttr = node->PopAttribute(_T("transform"));
	if (pAttr) {
		parseTransformString(pAttr->strAttribValue, fRotation);
	}

	pAttr = node->PopAttribute(_T("style"));
	if (pAttr) {
		parseStyleString(pAttr->strAttribValue, &color);
	}

	CDrawingObjectParams params( DO_TRIANGLE );
	_getParams( node, params );

	auto pRetVal = new CTriangle( params );
	pRetVal->SetColor(color);
	pRetVal->SetRotation(fRotation);

	pAttr = node->PopAttribute(_T("points"));
	if (pAttr) {
		vector<CPointf> *points = pRetVal->GetDataPoints();

		_getVertices(pAttr->strAttribValue, 3, points);

		CRectF rect;

		rect.m_Min.x = (*points)[0].x;
		rect.m_Min.y = (*points)[1].y;

		rect.m_Max.x = (*points)[2].x;
		rect.m_Max.y = (*points)[2].y;

		pRetVal->SetRect(rect);
	}

	return pRetVal;
}

CDrawingObject* CSVGConverter::_getEllipse(__in shared_ptr<CSimpleXMLNode> node)
{
	float fRotation(0.0f);
	floatColor color;

	auto pAttr = node->PopAttribute(_T("transform"));
	if (pAttr) {
		parseTransformString(pAttr->strAttribValue, fRotation);
	}

	pAttr = node->PopAttribute(_T("style"));
	if (pAttr) {
		parseStyleString(pAttr->strAttribValue, &color);
	}

	CDrawingObjectParams params( DO_ELLIPSE );
	_getParams( node, params );

	auto pRetVal = new CEllipsoid( params );
	pRetVal->SetRotation(fRotation);
	pRetVal->SetColor(color);

	return pRetVal;
}

CDrawingObject* CSVGConverter::_getRectangle(__in shared_ptr<CSimpleXMLNode> node)
{
	float fRotation(0.0f);
	floatColor color;

	auto pAttr = node->PopAttribute(_T("transform"));
	if (pAttr) {
		parseTransformString(pAttr->strAttribValue, fRotation);
	}

	pAttr = node->PopAttribute(_T("style"));
	if (pAttr) {
		parseStyleString(pAttr->strAttribValue, &color);
	}

	CDrawingObjectParams params( DO_RECTANGLE );
	_getParams( node, params );

	auto pRetVal = new CRectangle( params );
	pRetVal->SetRotation(fRotation);
	pRetVal->SetColor(color);

	return pRetVal;
}

CDrawingObject* CSVGConverter::_getVortex(__in shared_ptr<CSimpleXMLNode> node)
{
	CDrawingObjectParams params( DO_VORTEX );

	_getParams( node, params );

	return new CVortexObj( params );
}

CDrawingObject* CSVGConverter::_getStreamLine(__in shared_ptr<CSimpleXMLNode> node)
{
	return _get_X_Line(node, DO_STREAMLINE);
}

CDrawingObject* CSVGConverter::_getPathLine(__in shared_ptr<CSimpleXMLNode> node)
{
	return _get_X_Line(node, DO_PATHLINE);
}

CDrawingObject* CSVGConverter::_getStreakLine(__in shared_ptr<CSimpleXMLNode> node)
{
	return _get_X_Line(node, DO_STREAKLINE);
}

CDrawingObject* CSVGConverter::_getTimeLine(__in shared_ptr<CSimpleXMLNode> node)
{
	return _get_X_Line(node, DO_TIMELINE);
}

CDrawingObject* CSVGConverter::_getSpeedLine(__in shared_ptr<CSimpleXMLNode> node)
{
	return _get_X_Line(node, DO_SPEEDLINE);
}

CDrawingObject* CSVGConverter::_get_X_Line(__in shared_ptr<CSimpleXMLNode> node, DrawingObjectType nType)
{
	CDrawingObjectParams params( nType);

	auto attr = node->PopAttribute(_T("points"));

	_getParams( node, params );

	if (params.HasValue(DOP_DRAWASDROPLETS))
	{
		bool bAsDroplet = params.GetValueBool(DOP_DRAWASDROPLETS);
		params.RemoveValue(DOP_DRAWASDROPLETS);

		if (bAsDroplet) {
			UINT style = params.GetValueUInt(DOP_SPEEDLINE_STYLE) & 0xFF;
			style |= SL_DROPLET;
			params.SetValue(DOP_SPEEDLINE_STYLE, style);
		}
	}

	CPolyLine *pNewObj;

	switch (nType)
	{
		case DO_STREAKLINE:
			pNewObj = new CStreakLine( params );
			break;
		case DO_PATHLINE:
			pNewObj= new CPathLine( params );
			break;
		case DO_TIMELINE:
			pNewObj = new CTimeLine( params );
			break;
		case DO_SPEEDLINE:
			pNewObj = new CSpeedLine( params );
			break;
		case DO_STREAMLINE:
		default:
			pNewObj = new CStreamLine( params );
			break;
	}

	int nVertices = ( nType==DO_SPEEDLINE)? 50 : reinterpret_cast<CStreamLine*>(pNewObj)->GetMaxIntegrationLen();

	if (attr) {
		_getVertices(attr->strAttribValue, nVertices, pNewObj->GetDataPoints());
		pNewObj->CalcBoundingRect();
	}

	return pNewObj;
}

floatColor CSVGConverter::_extractFloatColor(const CString &str)
{
	CString buffer;

	if (!AfxExtractSubString(buffer, str, 1, _T('#'))) {
		AfxExtractSubString(buffer, str, 0, _T('#'));
	}

	COLORREF col;
	_stscanf_s(buffer, _T("%X"), &col);

	return floatColor ( GetBValue(col)/255.0f,
						GetGValue(col)/255.0f,
						GetRValue(col)/255.0f,
						1.0f);
}

CPointf CSVGConverter::_extractPointf(__in const CString &str)
{
	CString x,y;
	AfxExtractSubString(x, str, 0, _T(','));
	AfxExtractSubString(y, str, 1, _T(','));

	return CPointf( static_cast<float>(_tstof(x)),
					static_cast<float>(_tstof(y)) );
}

CVector2D CSVGConverter::_extractVector2D(__in const CString &str)
{
	CString x,y;
	AfxExtractSubString(x, str, 0, _T(','));
	AfxExtractSubString(y, str, 1, _T(','));

	return CVector2D( static_cast<float>(_tstof(x)),
					static_cast<float>(_tstof(y)) );
}

unsigned int CSVGConverter::_extractHexAttrib(const CString& strHexValue, unsigned int nDefault)
{
	if (strHexValue.IsEmpty())
		return nDefault;

	unsigned int retVal;
	_stscanf_s(strHexValue, _T("%x"), &retVal);

	return retVal;
}

CString CSVGConverter::GetSVGString(CDocument &document)
{
	CFlowIllustratorDoc &doc = reinterpret_cast<CFlowIllustratorDoc&>(document);
	CString str;
	const CAmiraVectorField2D *pVecField = doc.GetVectorfield();

	if (pVecField)
	{
		CRectF rcDomain = pVecField->GetDomainRect();
		const CDrawingObjectMngr *pMngr = doc.GetDrawingObjectMngr();
		float fLineWidthFactor = _getLineWidthFactor(document);

		str.Format(
			_T("<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>\
				\n<svg\
				\nxmlns=\"http://www.w3.org/2000/svg\"\
				\nwidth=\"%dpx\"\
				\nheight=\"%dpx\"\
				\nversion=\"1.1\"\
				\nviewBox=\"%f %f %f %f\">\n\
				<g transform=\"scale(1,-1)\">\n\
				<timestep>%d</timestep>\n\n"), 
				pVecField->GetExtentX(), pVecField->GetExtentY(), 
				rcDomain.m_Min.x, rcDomain.m_Min.y, rcDomain.getWidth(), rcDomain.getHeight(), 
				pVecField->GetCurrentTimeStep() );

		for (size_t i=0; i < pMngr->Size(); i++) {
			str += pMngr->GetAt(i)->toString(fLineWidthFactor);
		}

		str += _T("\n</g>\n\n</svg>");
	}

	return str;
}

float CSVGConverter::_getLineWidthFactor(CDocument &document)
{
	float fFactor = 1.0f;
	CFlowIllustratorDoc &doc = reinterpret_cast<CFlowIllustratorDoc&>(document);

	CView *pView = doc.GetActiveView();


	if (pView)
	{
		const CAmiraVectorField2D *pVecField = doc.GetVectorfield();

		if (pVecField)
		{
			CRectF rcDomain = pVecField->GetDomainRect();
			CRect rcClient;
			pView->GetClientRect(&rcClient);

			fFactor = static_cast<float>(rcClient.Width()) / rcDomain.getWidth();
		}
	}

	return fFactor;
}

void CSVGConverter::_getVertices(__in const CString& strVertices, __in int numVertices, __out vector<CPointf>* pDest)
{
	CString strCurrent, strX, strY;
	int nCurrPos=0;

	strCurrent= strVertices.Tokenize(_T(" "), nCurrPos);

	pDest->clear();
	pDest->reserve(numVertices);

	while ( strCurrent != _T("") )
	{
		if(AfxExtractSubString(strX, strCurrent, 0, _T(','))) {
			if(AfxExtractSubString(strY, strCurrent, 1, _T(','))) {

				pDest->push_back( CPointf( static_cast<float>(_tstof(strX)), static_cast<float>(_tstof(strY))) );
			}
		}

		strCurrent= strVertices.Tokenize(_T(" "), nCurrPos);
	}

	pDest->shrink_to_fit();
}

void CSVGConverter::parseStyleString(__in const CString& strSource, __out LPFLOATCOLOR pColor)
{
	CString dummy, dummy2, value;

	for (int i=0; AfxExtractSubString(dummy, strSource, i, _T(';')); i++)
	{
		if (AfxExtractSubString(dummy2, dummy, 0, _T(':')))
		{
			AfxExtractSubString(value, dummy, 1, _T(':'));

			if (dummy2 == _T("fill")) {
				if(value != _T("none")) {
					*pColor = _extractFloatColor(value);
				}
			} else if (dummy2 == _T("stroke")) {
				if (value != _T("none")) {
					*pColor = _extractFloatColor(value);
				}
			} 
		}
	}
}

void CSVGConverter::parseTransformString(__in const CString& strSource, __out float &rotation)
{
	CString dummy;

	if (AfxExtractSubString(dummy, strSource, 1, _T('(')))
	{
		AfxExtractSubString(dummy, dummy, 0, _T(','));
		rotation = static_cast<float>(_tstof(dummy));
	}
}

bool CSVGConverter::_getBoolAttrib(CMarkup &doc, const CString& strAttribName, bool bDefault)
{
	CString str(doc.GetAttrib(strAttribName));
	if (str.IsEmpty())
		return bDefault;

	return _tstoi(str)==0? false:true;
}