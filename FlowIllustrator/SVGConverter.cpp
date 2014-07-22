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
	hash_map<unsigned int, CDrawingObject*>	objCache;

	

	if (_checkFileName(strFileName, strRealName))
	{
		CFlowIllustratorDoc &doc = reinterpret_cast<CFlowIllustratorDoc&>(document);
		CDrawingObjectMngr *pMngr = doc.GetDrawingObjectMngr();
		doc.IsLoadingSVG( TRUE );

		float fLinewidthFactor = _getLineWidthFactor(document);

		CMarkup svgDoc;
		bResult = svgDoc.SetDoc(strRealName);
		if (bResult)
		{

			if (svgDoc.FindElem(_T("svg")) ) //It is a SVG document
			{
				doc.GotoFrame( _stepInside(svgDoc, *pMngr, objCache, fLinewidthFactor) );
			}
		}
		doc.IsLoadingSVG( FALSE );
	}
	
	return bResult;
}

int CSVGConverter::_stepInside(CMarkup &doc, CDrawingObjectMngr &dataContainer, __out hash_map<unsigned int, CDrawingObject*> &objCache, float fLinewidthFactor)
{
	doc.IntoElem();	//Step into;
	int nFrameNo(0);

	while (doc.FindChildElem())
	{
		CString strTag = doc.GetChildTagName().MakeLower();

		if ( strTag == _T("g")) {
			_stepInside(doc, dataContainer, objCache, fLinewidthFactor);
		} else {
			if (strTag == _T("timestep")) {
				CString str = doc.GetChildData();
				nFrameNo = _tstoi(str);
			} else {
				_addNewDrawingObject(doc, dataContainer, objCache, fLinewidthFactor);
			}
		}
	}

	doc.OutOfElem();

	return nFrameNo;
}

void CSVGConverter::_addNewDrawingObject(CMarkup &doc, CDrawingObjectMngr &dataContainer, __out hash_map<unsigned int, CDrawingObject*> &objCache, float fLinewidthFactor)
{
	doc.IntoElem();

	DrawingObjectType nObjType = _getDrawingObjectType(doc);
	CDrawingObject *pNewObj;

	switch(nObjType)
	{
	case DO_TRIANGLE:
		pNewObj = _getTriangle(doc);
		pNewObj->SetThickness( pNewObj->GetThickness()*fLinewidthFactor);
		break;
	case DO_ELLIPSE:
		pNewObj = _getEllipse(doc);
		pNewObj->SetThickness( pNewObj->GetThickness()*fLinewidthFactor);
		break;
	case DO_RECTANGLE:
		pNewObj = _getRectangle(doc);
		pNewObj->SetThickness( pNewObj->GetThickness()*fLinewidthFactor);
		break;
	case DO_VORTEX:
		pNewObj = _getVortex(doc);
		break;
	case DO_STREAMLINE:
		pNewObj = _getStreamLine(doc);
		break;
	case DO_PATHLINE:
		pNewObj = _getPathLine(doc);
		break;
	case DO_STREAKLINE:
		pNewObj = _getStreakLine(doc);
		break;
	case DO_TIMELINE:
		pNewObj = _getTimeLine(doc);
		break;
	case DO_SPEEDLINE:
		pNewObj = _getSpeedLine(doc);
		break;
	default:
		pNewObj = nullptr;
	}

	//Rebuild object hierarchy and add the new object to the CDrawingObjectMngr 
	if (pNewObj) 
	{
		unsigned int nID = _getHexAttrib(doc, _T("id"));
		objCache.insert( pair<unsigned int, CDrawingObject*>(nID, pNewObj) );

		unsigned int parentID = _getHexAttrib(doc, _T("parent"));
		if (parentID != 0) {
			CDrawingObject *pParent = objCache.at(parentID);
			if (pParent) {	//ensure we actually have the parent
				bool bSuccess(false);

				//Is the cild a speed line?
				switch (pParent->GetType()) 
				{
					case DO_VORTEX:
						if (pNewObj->GetType() == DO_SPEEDLINE) {
							//reinterpret_cast<CVortexObj*>(pParent)->SetTrajectory( reinterpret_cast<CSpeedLine*>(pNewObj) );
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

		if (nObjType != DO_SPEEDLINE) {
			dataContainer.Add(pNewObj);
		}
	}

	doc.OutOfElem();
}

float CSVGConverter::_getFloatAttrib(CMarkup &doc, const CString& strAttribName, float fDefault)
{
	CString str(doc.GetAttrib(strAttribName));
	if (str.IsEmpty())
		return fDefault;

	return static_cast<float>(_tstof(str));
}

int CSVGConverter::_getIntAttrib(CMarkup &doc, const CString& strAttribName, int nDefault)
{
	CString str(doc.GetAttrib(strAttribName));
	if (str.IsEmpty())
		return nDefault;

	return static_cast<int>(_tstoi(str));
}

unsigned int CSVGConverter::_getHexAttrib(CMarkup &doc, const CString& strAttribName, int nDefault)
{
	CString str(doc.GetAttrib(strAttribName));
	if (str.IsEmpty())
		return nDefault;

	unsigned int retVal;
	_stscanf_s(str, _T("%x"), &retVal);

	return retVal;
}

bool CSVGConverter::_getBoolAttrib(CMarkup &doc, const CString& strAttribName, bool bDefault)
{
	CString str(doc.GetAttrib(strAttribName));
	if (str.IsEmpty())
		return bDefault;

	return _tstoi(str)==0? false:true;
}

DrawingObjectType CSVGConverter::_getDrawingObjectType(CMarkup &doc)
{
	return static_cast<DrawingObjectType>(_tstoi(doc.GetAttrib(_T("objectType"))));
}

 floatColor CSVGConverter::_getColorAttrib(__in CMarkup &doc, __in const CString& strAttribName, const floatColor &defaultValue)
 {
	 CString val = doc.GetAttrib(strAttribName);
	 if (val.IsEmpty()) return defaultValue;

	 return _extractFloatColor(val);
 }

 CPointf CSVGConverter::_getPointfAttrib(__in CMarkup &doc, __in const CString& strAttribName, const CPointf &defaultValue)
 {
	CString val = doc.GetAttrib(strAttribName);
	if (val.IsEmpty()) return defaultValue;

	CString x,y;
	AfxExtractSubString(x, val, 0, _T(','));
	AfxExtractSubString(y, val, 1, _T(','));

	return CPointf( static_cast<float>(_tstof(x)),
					static_cast<float>(_tstof(y)) );
 }

CDrawingObject* CSVGConverter::_getTriangle(CMarkup &doc)
{
	CString points = doc.GetAttrib(_T("points"));
	CRectF rect;
	CString dummy, x,y;
	//Get the points
	AfxExtractSubString(dummy, points, 0, _T(' '));			//xMin
	AfxExtractSubString(x, dummy, 0, _T(','));

	rect.m_Min.x = static_cast<float>(_tstof(x));

	AfxExtractSubString(dummy, points, 1, _T(' '));			
	AfxExtractSubString(x, dummy, 0, _T(','));	//xMax
	AfxExtractSubString(y, dummy, 1, _T(',')); //yMin

	rect.m_Min.y = static_cast<float>(_tstof(y));
	rect.m_Max.x = static_cast<float>(_tstof(x));

	AfxExtractSubString(dummy, points, 2, _T(' '));		
	AfxExtractSubString(y, dummy, 1, _T(',')); //yMax

	rect.m_Max.y = static_cast<float>(_tstof(y));

	floatColor color (0.0f ,0.0f ,0.0f ,1.0f);
	BOOL bSolid = true;
	float fRotation = 0.0f, fThickness = 1.0f;

	bool bHatched = _getBoolAttrib(doc, _T("hatched"));

	parseStyleString(doc.GetAttrib(_T("style")), &color, bSolid, fThickness);
	parseTransformString(doc.GetAttrib(_T("transform")), fRotation);

	bSolid = _getBoolAttrib(doc, _T("IsSolid"));

	CTriangle *pNewObj = new CTriangle(rect, color);
	pNewObj->SetRotation(fRotation);
	pNewObj->SetThickness(fThickness);
	pNewObj->DrawStippled(bHatched);
	pNewObj->IsSolid(bSolid?true:false);

	return pNewObj;
}

CDrawingObject* CSVGConverter::_getEllipse(CMarkup &doc)
{
	float x		= _getFloatAttrib(doc, _T("cx"));
	float y		= _getFloatAttrib(doc, _T("cy"));
	float r1	= _getFloatAttrib(doc, _T("rx")); 
	float r2	= _getFloatAttrib(doc, _T("ry")); 

	bool bHatched = _getBoolAttrib(doc, _T("hatched"));

	floatColor color (0.0f ,0.0f ,0.0f ,1.0f);
	BOOL bSolid = TRUE;
	float fRotation = 0.0f, fThickness = 1.0f;
	CString id, dummy;

	parseStyleString(doc.GetAttrib(_T("style")), &color, bSolid, fThickness);
	parseTransformString(doc.GetAttrib(_T("transform")), fRotation);

	bSolid = _getBoolAttrib(doc, _T("IsSolid"));

	CEllipsoid *pNewObj = new CEllipsoid(x,y,r1,r2, color, bSolid?true:false);

	pNewObj->SetThickness(fThickness);
	pNewObj->SetRotation(fRotation);
	pNewObj->DrawStippled(bHatched);
	pNewObj->IsSolid(bSolid?true:false);

	return pNewObj;
}

CDrawingObject* CSVGConverter::_getRectangle(CMarkup &doc)
{
	float x = _getFloatAttrib(doc, _T("x"));
	float y = _getFloatAttrib(doc, _T("y"));
	float w = _getFloatAttrib(doc, _T("width"));
	float h = _getFloatAttrib(doc, _T("height"));
	bool bHatched = _getBoolAttrib(doc, _T("hatched"));

	floatColor color (0.0f ,0.0f ,0.0f ,1.0f);
	BOOL bSolid = true;
	float fRotation = 0.0f, fThickness = 1.0f;

	parseStyleString(doc.GetAttrib(_T("style")), &color, bSolid, fThickness);
	parseTransformString(doc.GetAttrib(_T("transform")), fRotation);

	bSolid = _getBoolAttrib(doc, _T("IsSolid"));

	CRectangle *pNewObj = new CRectangle(	CRectF( x, y, x+w, y+h),
											color,
											bSolid?true:false);
	pNewObj->SetRotation(fRotation);
	pNewObj->SetThickness(fThickness);
	pNewObj->DrawStippled(bHatched);

	return pNewObj;
}

CDrawingObject* CSVGConverter::_getVortex(CMarkup &doc)
{
	CDrawingObjectParams params(DO_VORTEX);
	floatColor color (0.0f ,0.0f ,0.0f ,1.0f);

	params.SetValue(DOP_COLOR,					_getColorAttrib(doc, DrawingObjNames.GetParamName(DOP_COLOR), color));
	params.SetValue(DOP_HALOCOLOR,				_getColorAttrib(doc, DrawingObjNames.GetParamName(DOP_HALOCOLOR), color));
	params.SetValue(DOP_ARROWCOLOR,				_getColorAttrib(doc, DrawingObjNames.GetParamName(DOP_ARROWCOLOR), color));
	params.SetValue(DOP_THICKNESS,				_getFloatAttrib(doc, DrawingObjNames.GetParamName(DOP_THICKNESS)));
	params.SetValue(DOP_CENTER,					_getPointfAttrib(doc, DrawingObjNames.GetParamName(DOP_CENTER)));
	params.SetValue(DOP_RADIUS1,				_getFloatAttrib(doc, DrawingObjNames.GetParamName(DOP_RADIUS1)));
	params.SetValue(DOP_RADIUS2,				_getFloatAttrib(doc, DrawingObjNames.GetParamName(DOP_RADIUS2)));
	params.SetValue(DOP_ARROWSIZE,				_getFloatAttrib(doc, DrawingObjNames.GetParamName(DOP_ARROWSIZE)));
	params.SetValue(DOP_ROTATION,				_getFloatAttrib(doc, DrawingObjNames.GetParamName(DOP_ROTATION)));
	params.SetValue(DOP_THRESHOLD,				_getFloatAttrib(doc, DrawingObjNames.GetParamName(DOP_THRESHOLD)));
	params.SetValue(DOP_AUTOADJUSTSIZE,			_getBoolAttrib(doc, DrawingObjNames.GetParamName(DOP_AUTOADJUSTSIZE)));
	params.SetValue(DOP_SHOWTRAJECTORY,			_getBoolAttrib(doc, DrawingObjNames.GetParamName(DOP_SHOWTRAJECTORY)));
	params.SetValue(DOP_TRANSPARENTTRAJECTORY,	_getBoolAttrib(doc, DrawingObjNames.GetParamName(DOP_TRANSPARENTTRAJECTORY)));
	params.SetValue(DOP_SMOOTHNESS,				_getIntAttrib(doc, DrawingObjNames.GetParamName(DOP_SMOOTHNESS)));
	params.SetValue(DOP_TRAJECTORYSTEPS,		_getIntAttrib(doc, DrawingObjNames.GetParamName(DOP_TRAJECTORYSTEPS)));
	params.SetValue(DOP_NUMARROWS,				_getIntAttrib(doc, DrawingObjNames.GetParamName(DOP_NUMARROWS)));
	params.SetValue(DOP_APPEARANCE,				_getIntAttrib(doc, DrawingObjNames.GetParamName(DOP_APPEARANCE)));
	params.SetValue(DOP_VORTEXSTYLE,			_getIntAttrib(doc, DrawingObjNames.GetParamName(DOP_VORTEXSTYLE)));
	params.SetValue(DOP_VORTEXDIR,				_getIntAttrib(doc, DrawingObjNames.GetParamName(DOP_VORTEXDIR)));
	params.SetValue(DOP_REVOLUTIONS,			_getFloatAttrib(doc, DrawingObjNames.GetParamName(DOP_REVOLUTIONS)));
	params.SetValue(DOP_ISSOLID, false);
	params.SetValue(DOP_HATCHED, false);

	return new CVortexObj(params);
}

CDrawingObject* CSVGConverter::_getStreamLine(CMarkup &doc)
{
	CDrawingObjectParams params(DO_STREAMLINE);
	_getStreamLineParams(doc, params);

	CStreamLine *pNewObj = new CStreamLine(params);

	_getVertices(doc, _T("points"), pNewObj->GetDataPoints(), pNewObj->GetMaxIntegrationLen());
		
	pNewObj->CalcBoundingRect();

	return pNewObj;
}

CDrawingObject* CSVGConverter::_getPathLine(CMarkup &doc)
{
	CDrawingObjectParams params(DO_PATHLINE);
	_getStreamLineParams(doc, params);

	params.SetValue(DOP_STARTFRAME, _getIntAttrib(doc, DrawingObjNames.GetParamName(DOP_STARTFRAME) ));
	params.SetValue(DOP_USE_STARTFRAME, _getBoolAttrib(doc, DrawingObjNames.GetParamName(DOP_USE_STARTFRAME) ));

	CPathLine *pNewObj = new CPathLine(params);

	_getVertices(doc, _T("points"), pNewObj->GetDataPoints(), pNewObj->GetMaxIntegrationLen());

	pNewObj->CalcBoundingRect();

	return pNewObj;
}

CDrawingObject* CSVGConverter::_getStreakLine(__in CMarkup &doc)
{
	CDrawingObjectParams params(DO_STREAKLINE);
	_getStreamLineParams(doc, params);

	params.SetValue(DOP_STARTFRAME, _getIntAttrib(doc, DrawingObjNames.GetParamName(DOP_STARTFRAME) ));
	params.SetValue(DOP_USE_STARTFRAME, _getBoolAttrib(doc, DrawingObjNames.GetParamName(DOP_USE_STARTFRAME) ));
	params.SetValue(DOP_RENDER_AS_PARTICLES, _getBoolAttrib(doc, DrawingObjNames.GetParamName(DOP_RENDER_AS_PARTICLES) ));

	CStreakLine *pNewObj = new CStreakLine(params);

	_getVertices(doc, _T("points"), pNewObj->GetDataPoints(), pNewObj->GetMaxIntegrationLen());

	pNewObj->CalcBoundingRect();
	pNewObj->NeedRecalc(false);

	return pNewObj;
}

CDrawingObject* CSVGConverter::_getTimeLine(__in CMarkup &doc)
{
	CDrawingObjectParams params(DO_TIMELINE);
	_getStreamLineParams(doc, params);

	params.SetValue(DOP_STARTFRAME,			_getIntAttrib(doc, DrawingObjNames.GetParamName(DOP_STARTFRAME) ));
	params.SetValue(DOP_USE_STARTFRAME,		_getBoolAttrib(doc, DrawingObjNames.GetParamName(DOP_USE_STARTFRAME) ));
	params.SetValue(DOP_PTEND,				_getPointfAttrib(doc, DrawingObjNames.GetParamName(DOP_PTEND) ));
	params.SetValue(DOP_NUM_SAMPLES,		_getIntAttrib(doc, DrawingObjNames.GetParamName(DOP_NUM_SAMPLES) ));
	params.SetValue(DOP_SHOW_SEEDINGLINE,	_getBoolAttrib(doc, DrawingObjNames.GetParamName(DOP_SHOW_SEEDINGLINE) ));
	params.SetValue(DOP_SHOWTRAJECTORY,		_getBoolAttrib(doc, DrawingObjNames.GetParamName(DOP_SHOWTRAJECTORY) ));

	CTimeLine *pNewObj = new CTimeLine(params);

	_getVertices(doc, _T("points"), pNewObj->GetDataPoints(), pNewObj->GetMaxIntegrationLen());

	pNewObj->CalcBoundingRect();

	return pNewObj;
}

CDrawingObject* CSVGConverter::_getSpeedLine(__in CMarkup &doc)
{
	CDrawingObjectParams params(DO_SPEEDLINE);
	_getStreamLineParams(doc, params);

	params.SetValue(DOP_ALPHA,				_getFloatAttrib(doc, DrawingObjNames.GetParamName(DOP_ALPHA) ));
	params.SetValue(DOP_ALPHA_MIN,			_getFloatAttrib(doc, DrawingObjNames.GetParamName(DOP_ALPHA_MIN) ));
	params.SetValue(DOP_THICKNESS_MIN,		_getFloatAttrib(doc, DrawingObjNames.GetParamName(DOP_THICKNESS_MIN) ));

	CSpeedLine *pNewObj = new CSpeedLine(params);

	_getVertices(doc, _T("points"), pNewObj->GetDataPoints(), 30);

	pNewObj->CalcBoundingRect();

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

void CSVGConverter::parseStyleString(__in const CString& strSource, __out LPFLOATCOLOR pColor, __out BOOL &bSolid, __out float &fLineWidth)
{
	CString dummy, dummy2, value;

	for (int i=0; AfxExtractSubString(dummy, strSource, i, _T(';')); i++)
	{
		if (AfxExtractSubString(dummy2, dummy, 0, _T(':')))
		{
			AfxExtractSubString(value, dummy, 1, _T(':'));

			if (dummy2 == _T("fill")) {
				if(value == _T("none")) {
					bSolid = false;
				} else{
					bSolid = true;
					*pColor = _extractFloatColor(value);
				}
			} else if (dummy2 == _T("stroke")) {
				if (value == _T("none")) {
					bSolid = true;
				} else {
					bSolid = false;
					*pColor = _extractFloatColor(value);
				}

			} else if (dummy2 == _T("stroke-width")) {
				fLineWidth = (static_cast<float>(_tstof(value)));
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

void CSVGConverter::_getVertices(__in CMarkup &doc, __in const CString& strAttribName, __out vector<CPointf>* pDest, int numVertices)
{
	CString strVertices = doc.GetAttrib(strAttribName);
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

void CSVGConverter::_getBaseParams(__in CMarkup& doc, CDrawingObjectParams &params)
{
	params.SetValue(DOP_TYPE, _getIntAttrib(doc, DrawingObjNames.GetParamName(DOP_TYPE)));
	params.SetValue(DOP_COLOR, _getColorAttrib(doc, DrawingObjNames.GetParamName(DOP_COLOR), floatColor(0,0,0) ));
	params.SetValue(DOP_THICKNESS, _getFloatAttrib(doc, DrawingObjNames.GetParamName(DOP_THICKNESS), 1.0f));
	params.SetValue(DOP_ROTATION, _getFloatAttrib(doc, DrawingObjNames.GetParamName(DOP_ROTATION)));
	params.SetValue(DOP_ISSOLID, _getBoolAttrib(doc, DrawingObjNames.GetParamName(DOP_ISSOLID)));
	params.SetValue(DOP_ALPHA, _getFloatAttrib(doc, DrawingObjNames.GetParamName(DOP_ALPHA), 1.0f));
	params.SetValue(DOP_HALOCOLOR, _getColorAttrib(doc, DrawingObjNames.GetParamName(DOP_HALOCOLOR), floatColor(1,1,1) ));
	params.SetValue(DOP_HATCHED, _getBoolAttrib(doc, DrawingObjNames.GetParamName(DOP_HATCHED) ));
}

void CSVGConverter::_getStreamLineParams(__in CMarkup& doc, CDrawingObjectParams &params)
{
	_getBaseParams(doc, params);

	params.SetValue(DOP_ARROWCOLOR, _getColorAttrib(doc, DrawingObjNames.GetParamName(DOP_ARROWCOLOR), floatColor(1,1,1)));
	params.SetValue(DOP_ORIGIN, _getPointfAttrib(doc, DrawingObjNames.GetParamName(DOP_ORIGIN)));
	params.SetValue(DOP_INTEGRATIONSTEPS, _getIntAttrib(doc, DrawingObjNames.GetParamName(DOP_INTEGRATIONSTEPS)));
	params.SetValue(DOP_STEPLENGTH, _getFloatAttrib(doc, DrawingObjNames.GetParamName(DOP_STEPLENGTH)));
	params.SetValue(DOP_SHOWARROWS, _getBoolAttrib(doc, DrawingObjNames.GetParamName(DOP_SHOWARROWS)));
	params.SetValue(DOP_NUMARROWS, _getIntAttrib(doc, DrawingObjNames.GetParamName(DOP_NUMARROWS)));
	params.SetValue(DOP_ARROWSIZE, _getFloatAttrib(doc, DrawingObjNames.GetParamName(DOP_ARROWSIZE)));
	params.SetValue(DOP_SMOOTHNESS, _getIntAttrib(doc, DrawingObjNames.GetParamName(DOP_SMOOTHNESS)));
	params.SetValue(DOP_NUMDROPLETS, _getIntAttrib(doc, DrawingObjNames.GetParamName(DOP_NUMDROPLETS),1));
	params.SetValue(DOP_SPEEDLINE_STYLE,	_getIntAttrib(doc, DrawingObjNames.GetParamName(DOP_SPEEDLINE_STYLE) ));

	bool bDrawAsDroplet =  _getBoolAttrib(doc, DrawingObjNames.GetParamName(DOP_DRAWASDROPLETS));
	if (bDrawAsDroplet) {
		UINT style = params.GetValueUInt(DOP_SPEEDLINE_STYLE) & 0xFF;
		style |= SL_DROPLET;
		params.SetValue(DOP_SPEEDLINE_STYLE, style);
	}
}