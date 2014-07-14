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
#include "SpeedLine.h"


CSpeedLine::CSpeedLine(const floatColor& color, float fThickness, float fAlpha)
	:	CPolyLine(color, fThickness, DO_SPEEDLINE)
{
	SetStyle(SL_TRANSPARENCY_DECREASE | SL_THICKNESS_CONST);
	SetAlpha(fAlpha, fAlpha/100);
	SetThicknessMin(fThickness/100);
}

CSpeedLine::CSpeedLine(const CDrawingObjectParams &params)
	: CPolyLine(CRectF(0.0f, 0.0f, 0.0f, 0.0f), params.GetValueFloatColor(DOP_COLOR), static_cast<DrawingObjectType>(params.GetValueInt(DOP_TYPE)))
{
	SetParams(params);
}

CSpeedLine::~CSpeedLine(void)
{
}

CDrawingObject* CSpeedLine::Duplicate() const
{
	CSpeedLine *pNewObj = new CSpeedLine( GetColor(), GetThickness(), GetAlphaMax());

	pNewObj->CopyParams(this);
	pNewObj->CopyVertices(this);
	
	return pNewObj;
}

/*void CSpeedLine::Draw()
{
	CDrawingObject::Draw();

	if ( DrawHalo() ) 
	{
		float thickness = GetThickness();
		floatColor color = GetColor();

		SetThickness(2*thickness);
		SetColor(GetHaloColor());
		_draw();

		SetThickness(thickness);
		color.alpha = 1.0;
		SetColor(color);
	}

	_draw();
}*/

/*void CSpeedLine::_draw()
{
	float deltaA, deltaT;
	float alphaStart, alphaEnd;
	float thicknessStart, thicknessEnd;

	switch (GetStyle() & 0x0F) {
		case SL_TRANSPARENCY_INCREASE:
			deltaA = (GetAlphaMax() - GetAlphaMin()) / m_Points.size();
			alphaStart	= GetAlphaMin();
			alphaEnd	= GetAlphaMax();
			break;
		case SL_TRANSPARENCY_DECREASE:
			deltaA = -(GetAlphaMax() - GetAlphaMin()) / m_Points.size();
			alphaStart	= GetAlphaMax();
			alphaEnd	= GetAlphaMin();
			break;
		default:
			deltaA = 0;
			alphaStart = alphaEnd = GetAlphaMax();
			break;
	}

	float fThickness(GetThickness());
	float fThicknessMin(GetThicknessMin());

	switch (GetStyle() & 0xF0) {
		case SL_THICKNESS_INCREASE:
			deltaT = (fThickness - fThicknessMin) / m_Points.size();
			thicknessStart = fThicknessMin;
			thicknessEnd = fThickness;
			break;
		case SL_THICKNESS_DECREASE:
			deltaT = -(fThickness - fThicknessMin) / m_Points.size();
			thicknessStart = fThickness;
			thicknessEnd = fThicknessMin;
			break;
		default:
			deltaT = 0;
			thicknessStart = thicknessEnd = fThickness;
			break;
	}

	floatColor col = GetColor();
	col.alpha = alphaStart;
	
	fThickness = thicknessStart;

	for (size_t i=1; i < m_Points.size(); ++i)
	{
		glLineWidth(fThickness);
		glColor4fv(reinterpret_cast<float*>(&col));

		glBegin(GL_LINES);
		glVertex2fv(reinterpret_cast<float*>(&m_Points[i-1]));
		glVertex2fv(reinterpret_cast<float*>(&m_Points[i]));
		glEnd();

		col.alpha	+= deltaA;
		fThickness	+= deltaT;
	}
}*/

/*bool CSpeedLine::SetParams(const CDrawingObjectParams &params)
{
	bool bResult (CDrawingObject::SetParams(params));
	
	if (bResult)
	{
		CSimpleVariant buffer;
		if (params.GetValue(DOP_SPEEDLINE_STYLE, buffer)) {
			SetStyle( buffer.uiVal );
		}
	
		if (params.GetValue(DOP_ALPHA, buffer)) {
			SetAlphaMax(buffer.fVal);
		}

		if (params.GetValue(DOP_ALPHA_MIN, buffer)) {
			SetAlphaMin(buffer.fVal);
		}

		if (params.GetValue(DOP_THICKNESS_MIN, buffer)) {
			SetThicknessMin( buffer.fVal );
		}
	}

	return bResult;
}*/