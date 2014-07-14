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
#include "StreamLine.h"

const floatColor dummyColor(0,0,0);

CStreamLine::CStreamLine(const CPointf &ptOrigin, unsigned int nNumIntegrationSteps, float fArrowSize, const floatColor& color, float fStepLength)
	: CPolyLine(CRectF(), color, DO_STREAMLINE)
{
	Init(nNumIntegrationSteps, fStepLength);
	SetArrowSize(fArrowSize);
	SetOrigin(ptOrigin);
	SetArrowColor(color);
}

CStreamLine::CStreamLine(const CPointf &ptOrigin, unsigned int nNumIntegrationSteps, float fArrowSize, const floatColor& color, DrawingObjectType type, float fStepLength)
	: CPolyLine(CRectF(), color, type)
{
	Init(nNumIntegrationSteps, fStepLength);
	SetArrowSize(fArrowSize);
	SetOrigin(ptOrigin);
	SetArrowColor(color);
}

CStreamLine::CStreamLine(const CDrawingObjectParams &params)
	: CPolyLine(CRectF(0.0f, 0.0f, 0.0f, 0.0f), params.GetValueFloatColor_r(DOP_COLOR), static_cast<DrawingObjectType>(params.GetValueInt(DOP_TYPE)))
{
	SetParams(params);
}

CStreamLine::CStreamLine(DrawingObjectType nType)
	:CPolyLine(CRectF(0.0f, 0.0f, 0.0f, 0.0f), dummyColor, nType)
{
}

void CStreamLine::Init(unsigned int nNumIntegrationSteps, float fStepLength)
{
	NeedRecalc(true);
	IsSolid(false);
	SetMaxIntegrationLen(nNumIntegrationSteps);
	SetStepSize(fStepLength);
	SetRotation(0.0f);	//Stream lines are never rotated
	
	ShowArrows(true);
	SetArrowCount(5);

	SetSmoothness(0);

	DrawAsDroplet(false);
	UseFixedStartFrame(false);
}

CStreamLine::~CStreamLine(void)
{
}

void CStreamLine::_OnParamsChanged()
{
}

void CStreamLine::Draw()
{
	bool bDrawAsDroplet(DrawAsDroplet());

	CPolyLine::Draw(bDrawAsDroplet);

	if (!bDrawAsDroplet)
	{
		if (m_Points.size() > 0) 
		{
			if (ShowArrows()) {		
				DrawArrows();

				for (auto iter = m_Arrows.begin(); iter < m_Arrows.end(); iter++)
					iter->Draw();
			}
		}
	}
}

bool CStreamLine::setParam(DrawinObjectParamName paramID, const CSimpleVariant &val)
{
	bool bResult(CPolyLine::setParam(paramID, val));

	if (!bResult)
	{
		switch (paramID)
		{
			case DOP_SHOWARROWS:
				ShowArrows(val.GetBoolVal() );
				bResult = true;
				break;
			case DOP_NUMARROWS:
				SetArrowCount(val.GetIntVal());
				bResult = true;
				break;
			case DOP_INTEGRATIONSTEPS:
			{
				int nNumSteps = val.GetIntVal();
				if (nNumSteps != GetMaxIntegrationLen()) {
					SetMaxIntegrationLen(nNumSteps);
				}
				bResult = true;
				break;
			}
			case DOP_STEPLENGTH:
				SetStepSize( val.GetFloatVal() );
				bResult = true;
				break;
			case DOP_ORIGIN:
				SetOrigin(val.GetPointfVal());
				bResult = true;
				break;
			case DOP_ARROWSIZE:
				SetArrowSize(val.GetFloatVal());
				bResult = true;
				break;
			case DOP_SMOOTHNESS:
				SetSmoothness(val.GetIntVal());
				bResult = true;
				break;
			case DOP_DRAWASDROPLETS:
				DrawAsDroplet(val.GetBoolVal());
				bResult = true;
				break;
			case DOP_ARROWCOLOR:
				SetArrowColor( val.GetColorVal() );
				bResult = true;
				break;
		}
	}
	return bResult;
}

void CStreamLine::OnSetParams()
{
	NeedRecalc(true);
}

void CStreamLine::_applyArrowColor()
{
	const floatColor& color(GetArrowColor());

	for (auto iter = m_Arrows.begin(); iter != m_Arrows.end(); ++iter) {
		iter->SetColor(color);
	}
}

CDrawingObject* CStreamLine::Duplicate() const
{
	CStreamLine* pRetVal = new CStreamLine(GetOrigin(), GetMaxIntegrationLen(), GetArrowSize(), GetColor(), GetStepSize());
	
	pRetVal->CopyVertices(this);
	pRetVal->CopyParams(this);

	pRetVal->CalcBoundingRect();

	return pRetVal;
}

#ifdef WIN32

void CStreamLine::FinalizeToString(CString &str, float fLineWidthFactor) const
{
	if (ShowArrows()) {
		for (auto iter = m_Arrows.begin(); iter != m_Arrows.end(); ++iter) {
			str += iter->toString(fLineWidthFactor);
		}
	}

	CDrawingObject::FinalizeToString(str, fLineWidthFactor);
}
#endif

void CStreamLine::DrawArrows()
{
	m_Arrows.clear();

	if (ShowArrows())
	{
		const floatColor &color = GetArrowColor();

		int nArrowCunt(GetArrowCount());

		if (m_Points.size() > 7) {
			int nStartOfArrow;

			if (nArrowCunt > 1)
			{
				int nDiff = m_Points.size()/(nArrowCunt-1);
				for (size_t i=0; i < m_Points.size()-1; i+= nDiff)
				{
					if (i%nDiff == 0) {
						CVector2D dir = m_Points[i+1] - m_Points[i];
						CPointf ptStart;

						if (i==0) {
							nStartOfArrow = 0;
							ptStart =  CPointf(m_Points[nStartOfArrow].x, m_Points[nStartOfArrow].y);
						}
						else {
							nStartOfArrow = i- 2;
							ptStart =  CPointf(m_Points[nStartOfArrow].x+dir.x, m_Points[nStartOfArrow].y+dir.y);
						}

						m_Arrows.push_back(CArrow(ptStart, -dir, GetArrowSize(), color));
					}
				}

			}

			//Add Last Arrow
			nStartOfArrow = m_Points.size()-7;
			CVector2D dir = m_Points[m_Points.size()-4] - m_Points[m_Points.size()-5];
			CPointf ptStart (m_Points[nStartOfArrow].x+dir.x, m_Points[nStartOfArrow].y+dir.y);
			CArrow (ptStart, -dir, GetArrowSize(), color);
			m_Arrows.push_back(CArrow (ptStart, -dir, GetArrowSize(), color));
		}
	}
}

CBSpline* CStreamLine::GetStreamlineApprox(CPointf* pStreamLine) const
{
	std::vector<int> extrema;
	
	extrema.push_back(0);

	for (size_t i=1; i<m_Points.size()-2; i++)
	{
		if( ((pStreamLine[i].y >  pStreamLine[i-1].y) && (pStreamLine[i].y > pStreamLine[i+1].y)) ||
			((pStreamLine[i].y <  pStreamLine[i-1].y) && (pStreamLine[i].y < pStreamLine[i+1].y)) ||
			((pStreamLine[i].x >  pStreamLine[i-1].x) && (pStreamLine[i].x > pStreamLine[i+1].x)) ||
			((pStreamLine[i].x <  pStreamLine[i-1].x) && (pStreamLine[i].x < pStreamLine[i+1].x)) )
			extrema.push_back(i);
	}

	extrema.push_back(m_Points.size()-1);

	while (extrema.size() < 9 ) 
	{
		for (size_t i=1; i < extrema.size(); i+=2)
		{
			int dist = extrema[i] - extrema[i-1];
			extrema.insert(extrema.begin()+i, extrema[i-1]+dist/2);
		}
	}

	CBSpline *pSpline = new CBSpline(extrema.size(), GetColor());
	pSpline->CalcKnotVector();

	//ToDo: Least squares approximation
	//1) Build matrix A^T with m+1 rows and n+1 columns. 
	// m= number of data points P
	// n= number of control points Q

	//in our case m==n (for now)

	
	for (size_t i =0; i < extrema.size(); i++)
	{
		pSpline->m_pPoints[i] = pStreamLine[extrema[i]];
	}

	return pSpline;
}

void CStreamLine::Smooth(int nSmoothness)
{
	vector<CPointf> dummy(m_Points.size());	

	dummy[0] = m_Points[0];
	int size = static_cast<int>(m_Points.size());

	for (int i=1; i < size-1; i++) {
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

	dummy[m_Points.size()-1] = m_Points[m_Points.size()-1];

	m_Points.clear();
	m_Points.insert(m_Points.begin(), dummy.begin(), dummy.end());

	CalcBoundingRect();
}

bool CStreamLine::NeedRecalc() const 
{ 
	return m_bNeedRecalc; 
}

void CStreamLine::NeedRecalc(bool bNeedRecalc)
{ 
	m_bNeedRecalc = bNeedRecalc; 
}