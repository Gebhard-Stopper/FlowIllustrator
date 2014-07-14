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

void CPolyLine::CalcVertexGradients()
{
	m_Gradients.clear();
	
	if (m_Points.size() > 2) 
	{
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

		float thicknessStart, thicknessEnd, deltaT;
		float fThickness(GetThickness());
		float fThicknessMin(GetThicknessMin());

		int nSteps( GetNumGrowSteps() );
		if (nSteps <= 0) {
			nSteps = m_Points.size();
		}
		int nStartGrow (0);

		switch (GetStyle() & 0xF0) {
			case SL_THICKNESS_INCREASE:
				deltaT = (fThickness - fThicknessMin) / nSteps;
				thicknessStart = fThicknessMin;
				thicknessEnd = fThickness;
				break;
			case SL_THICKNESS_DECREASE:
				deltaT = -(fThickness - fThicknessMin) / nSteps;
				thicknessStart = fThickness;
				thicknessEnd = fThicknessMin;
				nStartGrow = (m_Points.size()-1)-nSteps;
				break;
			default:
				deltaT = 0;
				thicknessStart = thicknessEnd = fThickness;
				nStartGrow = m_Points.size();
				break;
		}
	
		fThickness = thicknessStart;

		float CurrThickness( fThickness*(gPixelPerUnit/2.0f) );
		m_Gradients.reserve(2 * m_Points.size());

		CVector2D dummy ( rot90(calcForwardDiff(m_Points,0).Normalize()) );
		dummy *= CurrThickness;

		m_Gradients.push_back( m_Points[0].toVector2D() + dummy );
		m_Gradients.push_back( m_Points[0].toVector2D() - dummy );

		for (int i=1; i < m_Points.size()-1; ++i) {
			if (i >= nStartGrow) {
				if (i<(nSteps+nStartGrow)) {
					fThickness += deltaT;
				}
			}

			CurrThickness = ( fThickness*(gPixelPerUnit/2.0f) );
			dummy = rot90(calcCentralDiff(m_Points, i).Normalize());
			dummy *= CurrThickness;

			m_Gradients.push_back( m_Points[i].toVector2D() + dummy );
			m_Gradients.push_back( m_Points[i].toVector2D() - dummy );
		}

		if (nStartGrow < m_Points.size()-1) {
			fThickness += deltaT;
		}

		CurrThickness = ( fThickness*(gPixelPerUnit/2.0f) );

		dummy = rot90(calcBackwardDiff(m_Points, m_Points.size()-1).Normalize());
		dummy *= CurrThickness;

		m_Gradients.push_back( m_Points[m_Points.size()-1].toVector2D() + dummy );
		m_Gradients.push_back( m_Points[m_Points.size()-1].toVector2D() - dummy );
	}
}

void CPolyLine::_drawDroplet(CPolyLine &polyLine)
{
	static const int step(4);
	float thickness = GetThickness() * gPixelPerUnit;
		
	float growStep = thickness/ (polyLine.m_Points.size()/ static_cast<float>(step+0.5f) );

	const floatColor& col = polyLine.GetColor();
	thickness=0;
	CEllipsoid dummy(0,0,0,0,col);

	for (size_t i=0; i < polyLine.m_Points.size(); i+=step)
	{
		int x = i;
		while (i>0 && i < (polyLine.m_Points.size()-1) && (polyLine.m_Points[i] - polyLine.m_Points[x]).abs() < thickness/(step*2.0f)  ) {
			i+=step;
			thickness += growStep/2.0f;
		}

		if (i < polyLine.m_Points.size()) {
			dummy.SetCenter(polyLine.m_Points[i]);
			dummy.SetRadius1(thickness);
			dummy.SetRadius2(thickness);
			dummy.Draw();
			thickness += growStep;
		}
	}
}

void CPolyLine::Draw(bool bDrawAsDroplet, int nCutoff)
{
	floatColor color = GetColor();
	float fThickness = GetThickness();

	if (bDrawAsDroplet)
	{
		CRectF rect(GetRect());
		int numDroplets(GetDropletCount());

		int numPoints = numDroplets>1? m_Points.size()/(numDroplets+1) : m_Points.size();
		size_t ptFirst = 0;
		size_t ptLast = numPoints;
		int diff = numDroplets > 1? numPoints/(numDroplets-1) : 0;

		for (int i=0; i< numDroplets;++i) {
			CPolyLine pl(rect, color, fThickness);
			if (ptLast >= m_Points.size()) {
				ptLast = m_Points.size()-1;
			}
			pl.m_Points.insert( pl.m_Points.begin(), m_Points.begin() + ptFirst, m_Points.begin()+ptLast);

			if (DrawHalo()) {
				pl.SetColor(GetHaloColor());
				SetThickness(2.0f*fThickness);
				_drawDroplet(pl);
				SetThickness(fThickness);
				color.alpha = 1.0f;
				pl.SetColor(color);
			}
			_drawDroplet(pl);	
			ptFirst+= numPoints + diff;
			ptLast = ptFirst + numPoints;
		}
	}
	else
	{
		if (DrawHalo()) {
			SetThickness(2*fThickness);
			SetColor(GetHaloColor());
			_draw(0);
			SetThickness(fThickness);
			color.alpha = 1.0f;
			SetColor(color);
		}
		_draw(nCutoff);
	}
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

	CalcVertexGradients();

	int nMaxVertIdx( m_Gradients.size()-(nCutoff*2)  );

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
			nTransparencyStart = (m_Gradients.size()-2)-(nTransparentSteps*2);
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
		glVertex2fv(reinterpret_cast<float*>(&m_Gradients[i++]));
		glVertex2fv(reinterpret_cast<float*>(&m_Gradients[i++]));
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