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
#include "StreakLine.h"


CStreakLine::CStreakLine(const CPointf &ptOrigin, unsigned int nNumIntegrationSteps, int nStartFrame, float fArrowLength, const floatColor& color, float fStepLength)
	:	CPathLine(ptOrigin, nNumIntegrationSteps, nStartFrame, fArrowLength, color, DO_STREAKLINE, fStepLength)
{
	UseFixedStartFrame(false);
	RenderAsParticles(false);
}

CStreakLine::CStreakLine(const CDrawingObjectParams &params)
	:CPathLine(DO_STREAKLINE)
{
	SetParams(params);
}

CStreakLine::~CStreakLine(void)
{
}

CDrawingObject* CStreakLine::Duplicate() const
{
	CStreakLine* pRetVal = new CStreakLine(GetOrigin(), GetMaxIntegrationLen(), GetStartFrame(), GetArrowSize(), GetColor(), GetStepSize());
	
	pRetVal->CopyVertices(this);
	pRetVal->CopyParams(this);

	pRetVal->CalcBoundingRect();

	return pRetVal;
}

bool CStreakLine::setParam(DrawinObjectParamName paramID, const CSimpleVariant &val)
{
	bool bResult(CPathLine::setParam(paramID, val));

	if (!bResult)
	{
		switch (paramID)
		{
			case DOP_RENDER_AS_PARTICLES:
				RenderAsParticles(val.GetBoolVal());
				bResult = true;
				break;
		}
	}

	return bResult;
}

void CStreakLine::Draw()
{
	float thickness = GetThickness();
	floatColor color = GetColor();

	color.alpha = 1.0;

	SetThickness(2*thickness);
	SetColor(GetHaloColor());
	_draw();
	
	SetThickness(thickness);
	SetColor(color);
	_draw();

	if (m_Points.size() > 0 && !DrawAsDroplet()) 
	{
		if (ShowArrows()) {
			DrawArrows();
			for (auto iter = m_Arrows.begin();iter < m_Arrows.end(); iter++) {
				iter->Draw();
			}
		}
	}
}

void CStreakLine::_draw()
{
	if (DrawAsDroplet()) {
		CPolyLine::Draw(DrawAsDroplet());
		return;
	} else {
		CDrawingObject::Draw();
	}

	glPointSize(GetThickness());

	float fOriginalSize;
	glGetFloatv(GL_POINT_SIZE, &fOriginalSize);

	if (m_Points.size() > 0) {
		glEnableClientState(GL_VERTEX_ARRAY);
		glVertexPointer(2, GL_FLOAT, 0, &m_Points[0]);
		glDrawArrays( (RenderAsParticles())? GL_POINTS : GL_LINE_STRIP, 0, m_Points.size());
		glDisableClientState(GL_VERTEX_ARRAY);
	}

	glPointSize(fOriginalSize);
}
