/*
 *	Copyright (C) 2014, Max Planck Institut für Informatik, Saarbrücken.
 *	Implementation: 2014, Gebhard Stopper [ gebhard.stopper@gmail.com ]
 *	
 *	If you perform any changes on this file, please append your name to 
 *	the List of people who worked on this file.
 *
 *	If you add or modify functions or variables, please do not forget to
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
#include "PolyLine.h"
#include "Ellipseoid.h"

CPolyLine::CPolyLine(const CRectF& BBox, const floatColor& color, float fThickness)
	: CDrawingObject(BBox, DO_POLYLINE, color)
{
	Init(fThickness);
}

CPolyLine::CPolyLine(const floatColor& color, float fThickness)
	: CDrawingObject(CRectF(0,0,0,0), DO_POLYLINE, color)
{
	Init(fThickness);
	SetThicknessMin(fThickness);
}

CPolyLine::CPolyLine(const CRectF& BBox, const floatColor& color, DrawingObjectType nType)
	: CDrawingObject(BBox, nType, color)
{
	Init(2);
}

CPolyLine::CPolyLine(const floatColor& color, float fThickness, DrawingObjectType nType)
	: CDrawingObject(CRectF(0,0,0,0), nType, color)
{
	Init(fThickness);
}

CPolyLine::~CPolyLine(void)
{
}

void CPolyLine::Init(float fThickness)
{
	IsSolid(FALSE);
	Select(false);
	SetThickness(fThickness);
	SetDropletCount(1);

	SetNumGrowSteps(0);
	SetNumTransparentSteps(0);
	SetStyle(SL_THICKNESS_CONST | SL_TRANSPARENCY_CONST);
	SetAlpha(1.0f, 0.0f);
}

#ifdef WIN32

void CPolyLine::OnToString(CString &str, float /*fLineWidthFactor*/) const
{
	str += _T("points=\"");
	CString dummy;
	for (auto iter = m_Points.begin(); iter != m_Points.end(); ++iter) {
		str += iter->ToString() + _T(" ");
	}
	str += "\"\n";
}
#endif

void CPolyLine::_OnParamsChanged()
{
}

void CPolyLine::CalcVertexNormals()
{
	m_Normals.clear();

	if (m_Points.size() > 2) 
	{
		int nDropletCount = GetDropletCount();	//Number of droplets
		if (nDropletCount < 1) nDropletCount = 1;

		int nBands(nDropletCount == 1? m_Points.size()/2 : 200 ); //Number of vertices per droplet
		if (nBands < 200) nBands = 200;

		const int vertexCount(nDropletCount * nBands);	//Total number of vertices
		DWORD dwThicknessMode (GetStyle() & 0xFF0);

		vector<CPointf> vertices;
		vertices.reserve(vertexCount);

		for (int i=0; i<vertexCount; ++i) {
			vertices.push_back( _interpolateVertex(i, vertexCount) );
		}

	
		static auto calcCentralDiff = [] (const vector<CPointf>& points, int idx) -> CVector2D { 
			return ((points[idx+1] - points[idx-1]) / 2.0f);
		};

		static auto calcForwardDiff = [] (const vector<CPointf>& points, int idx) -> CVector2D {
			return  (points[idx+1] - points[idx]);
		};

		static auto calcBackwardDiff = [] (const vector<CPointf>& points, int idx) -> CVector2D {
			return  (points[idx] - points[idx-1]);
		};

		static auto rot90 = [] (const CVector2D& vec) -> CVector2D {
			return CVector2D(vec.y, -vec.x);
		};

		int nSteps( GetNumGrowSteps() );
		if (nSteps <= 0) {
			nSteps = nBands;
		}
		
		int nStartGrow = (dwThicknessMode == SL_THICKNESS_DECREASE)? (nBands-1)-nSteps : 0;


		float CurrThickness( _calcThickness(0, nSteps, dwThicknessMode) );

	
		m_Normals.reserve(2 * vertices.size());

		CVector2D dummy ( rot90(calcForwardDiff(vertices,0).Normalize()) );
		dummy *= CurrThickness;

		m_Normals.push_back( vertices[0].toVector2D() + dummy );
		m_Normals.push_back( vertices[0].toVector2D() - dummy );

		for (int d=0; d<nDropletCount; ++d)
		{
			for (int i=1; i < nBands-1; ++i) {
				int vIndx = (d*nBands)+i;
				if (i >= nStartGrow) {
					if (i<(nSteps+nStartGrow)) {
						CurrThickness = (  _calcThickness(i-nStartGrow, nSteps, dwThicknessMode) );
					}
				}

				dummy = rot90(calcCentralDiff(vertices, vIndx).Normalize());
				dummy *= CurrThickness;

				m_Normals.push_back( vertices[vIndx].toVector2D() + dummy );
				m_Normals.push_back( vertices[vIndx].toVector2D() - dummy );
			}
		}

		if (nStartGrow < m_Points.size()-1) {
			CurrThickness = (  _calcThickness(vertexCount-nStartGrow, nSteps, dwThicknessMode) );
		}

		dummy = rot90(calcBackwardDiff(vertices, vertices.size()-1).Normalize());
		dummy *= CurrThickness;

		m_Normals.push_back( vertices[vertices.size()-1].toVector2D() + dummy );
		m_Normals.push_back( vertices[vertices.size()-1].toVector2D() - dummy );
	}
}

void CPolyLine::Draw(bool bDrawAsDroplet, int nCutoff)
{
	floatColor color = GetColor();
	float fThickness = GetThickness();
		
	if (DrawHalo()) 
	{
		SetThickness(2*fThickness);
		SetColor(GetHaloColor());
		srand (42);
		_draw(0);
		SetThickness(fThickness);
		color.alpha = 1.0f;
		SetColor(color);
	}
	srand (42);
	_draw(nCutoff);
}

/*
	To be optimized!!

	calculates the gradient for every vertex and then uses
	this information to draw a poly line using quads.
	->	polylines maintain a constant width, and no artefacts,
		if line thickness is to great.
*/
void CPolyLine::_draw(int nCutoff)
{
	CDrawingObject::Draw();

	CalcVertexNormals();

	int nMaxVertIdx( m_Normals.size()-(nCutoff*2)  );

	float deltaA;
	float alphaStart, alphaEnd;

	int nTransparentSteps ( GetNumTransparentSteps() );
	if (nTransparentSteps<=0) {
		nTransparentSteps = m_Points.size();
	}

	int nTransparencyStart (0);

	switch (GetStyle() & 0x0F) {
		case SL_TRANSPARENCY_INCREASE:
			deltaA = (GetAlphaMax() - GetAlphaMin()) / nTransparentSteps;
			alphaStart	= GetAlphaMin();
			alphaEnd	= GetAlphaMax();
			break;
		case SL_TRANSPARENCY_DECREASE:
			deltaA = -(GetAlphaMax() - GetAlphaMin()) / nTransparentSteps;
			alphaStart	= GetAlphaMax();
			alphaEnd	= GetAlphaMin();
			nTransparencyStart = (m_Normals.size()-2)-(nTransparentSteps*2);
			break;
		default:
			deltaA = 0;
			alphaStart = alphaEnd = GetAlphaMax();
			nTransparentSteps = 0;
			break;
	}

	nTransparentSteps *= 2;
	int nTransparencyEnd = nTransparencyStart+nTransparentSteps;

	floatColor col = GetColor();
	col.alpha = alphaStart;

	glBegin(GL_QUAD_STRIP);
	for (int i=(nCutoff*2); i<nMaxVertIdx;) {
		glColor4fv(reinterpret_cast<float*>(&col));
		glVertex2fv(reinterpret_cast<float*>(&m_Normals[i++]));
		glVertex2fv(reinterpret_cast<float*>(&m_Normals[i++]));
		if (i>=nTransparencyStart && i < nTransparencyEnd)
			col.alpha	+= deltaA;
	}
	glEnd();
}

bool CPolyLine::setParam(DrawinObjectParamName paramID, const CSimpleVariant &val)
{
	bool bResult(CDrawingObject::setParam(paramID, val));

	if (!bResult)
	{
		switch (paramID)
		{
			case DOP_NUMDROPLETS:
				SetDropletCount( val.GetIntVal() );
				bResult = true;
				break;
			case DOP_SPEEDLINE_STYLE:
				SetStyle( val.GetUIntVal() );
				bResult = true;
				break;

			case DOP_ALPHA:
				SetAlphaMax( val.GetFloatVal() );
				bResult = true;
				break;
			case DOP_ALPHA_MIN:
				SetAlphaMin(val.fVal);
				bResult = true;
				break;
			case DOP_THICKNESS_MIN:
				SetThicknessMin( val.fVal );
				bResult = true;
				break;
		}
	}

	return bResult;
}

void CPolyLine::OnSetParams()
{
	CalcBoundingRect();
}

void CPolyLine::CalcBoundingRect()
{
	if (m_Points.size() == 0) return;

	CRectF rcBounds;
	rcBounds.m_Min = rcBounds.m_Max = CVector2D(m_Points[0].x, m_Points[0].y);

	for (auto iter = m_Points.begin(); iter != m_Points.end(); ++iter)
	{
		if (iter->x < rcBounds.m_Min.x)
			rcBounds.m_Min.x = iter->x;
		else if (iter->x > rcBounds.m_Max.x)
			rcBounds.m_Max.x = iter->x;

		if (iter->y < rcBounds.m_Min.y)
			rcBounds.m_Min.y = iter->y;
		else if (iter->y > rcBounds.m_Max.y)
			rcBounds.m_Max.y = iter->y;
	}

	SetRect(rcBounds);
}

void CPolyLine::Smooth(int nSmoothness)
{
	vector<CPointf> dummy(m_Points.size());	
	int size( static_cast<int>(m_Points.size()) );

	for (int i=0; i < size; i++) {
		int nNum = 0;
		for (int k=-nSmoothness; k<=nSmoothness; k++) {

			if (i+k >= 0 && i+k < size)
			{
				dummy[i] += m_Points[i+k];
				nNum++;
			}
		}

		dummy[i].x /= nNum;
		dummy[i].y /= nNum;
	}

	m_Points.clear();
	m_Points.insert(m_Points.begin(), dummy.begin(), dummy.end());

	CalcBoundingRect();
}

void CPolyLine::Translate(float tx, float ty)
{
	CPointf pt(tx, ty);

	for (auto iter = m_Points.begin(); iter != m_Points.end(); ++iter) {
		(*iter) += pt;
	}

	CDrawingObject::Translate(tx, ty);
	CalcBoundingRect();
}

vector<CPointf>* CPolyLine::GetDataPoints()
{
	return &m_Points;
}

CDrawingObject* CPolyLine::Duplicate() const
{
	CPolyLine *pNewObj = new CPolyLine(GetRect(), GetColor(), GetThickness());

	pNewObj->CopyVertices(this);
	pNewObj->CopyParams(this);

	return pNewObj;
}

void CPolyLine::CopyVertices(const CPolyLine *pSrc)
{
	m_Points.clear();
	m_Points.insert(m_Points.begin(), pSrc->m_Points.begin(), pSrc->m_Points.end());
}

CPointf CPolyLine::_interpolateVertex(int nIndx, int nVertexCount)
{
	float fRealIndx ( (m_Points.size()  * (static_cast<float>(nIndx)/nVertexCount)) );
	
	int nLBound (static_cast<int>(fRealIndx));
	int nUBound (nLBound + 1);
	float fWeight (nUBound - fRealIndx);

	if (nUBound > (m_Points.size()-1)) {	//just to be on the safe side
		return m_Points[m_Points.size()-1];
	}

	return m_Points[nLBound]*fWeight + m_Points[nUBound]*(1.0f-fWeight);
}

float CPolyLine::_calcThickness(int nIndx, int nVertexCount, DWORD dwThicknessFunc)
{
	float fThickness(GetThickness());
	float fThicknessMin(GetThicknessMin());

	int nSteps( GetNumGrowSteps() );
	if (nSteps <= 0) 
		nSteps = nVertexCount;

	float retVal;

	switch (dwThicknessFunc) 
	{
		case SL_THICKNESS_INCREASE:
			retVal = ((fThickness - fThicknessMin) / nSteps) * (static_cast<float>(nIndx));
			break;
		case SL_THICKNESS_DECREASE:
			retVal = fThickness - ((fThickness - fThicknessMin) / nSteps) * (static_cast<float>(nIndx));
			break;
		case SL_DASHED:
			{
				float f ( (static_cast<float>(nIndx)/nSteps ) );
				if ( f< 1/4.0f || f > 3/4.0f) {
					retVal = fThickness;
				} else {
					retVal = 0.0f;
				}
				break;
			}
		case SL_ARROW:
			{
				nSteps *= .95f;
				float f ( (static_cast<float>(nIndx)/nSteps ) );
				if (f < 1.0f)
				{
					if (f < .8f) {
						retVal = f*fThickness;
					} else if (f == .8f) {
						retVal = 2*fThickness;
					} else {
						float n = nSteps - (.8f)*nSteps;
						float i = nIndx - (.8f)*nSteps;

						retVal = 2*fThickness * (1.0f - (i / n));
					}
				}
				else
				{
					retVal = 0.0f;
				}
				break;
			}
		case SL_TRIANGLE:
			{
				float f ( (static_cast<float>(nIndx)/nSteps ) );
				if (f <= .5)
				{
					retVal = fThickness - ((fThickness) * (static_cast<float>(nIndx*2) / nSteps));
				} else {
					retVal = 0;
				}
				break;
			}
		case SL_DROPLET:
			{
				nSteps *= .97;

				float f ( (static_cast<float>(nIndx)/(nSteps) ) );
				if ( f <= 1)
				{
					if (f <.90) {
						retVal = ((fThickness) * (static_cast<float>(nIndx) / (nSteps*.9)));
					} else {
						float s = nSteps*.1f;
						float i = nIndx - (nSteps*.9f);

						retVal = sqrt( 1.0f - pow ( i/s, 2.0f) ) * fThickness;
					}
				} else {
					retVal = 0.0f;
				}
				break;
			}
		case SL_ELLIPSE:
			{
				float s = nSteps *.95f;
				float f ( (static_cast<float>(nIndx)/s ) );

				if (f < 1.0f) {
					f -= .5f;
					f *= 2.0f;
					retVal = sqrt( 1.0f - pow ( f, 2.0f) ) * fThickness;
				} else {
					retVal = 0.0f;
				}
				break;
			}
		default:
			retVal = fThickness;
			break;
	}

	return retVal*(gPixelPerUnit/2.0f);
}