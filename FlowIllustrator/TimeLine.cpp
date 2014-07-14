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
#include "TimeLine.h"


CTimeLine::CTimeLine(const CPointf &ptSeedLineStart, const CPointf &ptSeedLineEnd, int numSamples, int numIntegrationSteps, float fStepLen, float fThickness, const floatColor& color)
	:	CStreamLine(ptSeedLineStart, numIntegrationSteps, 0, color, DO_TIMELINE, fStepLen)
{
	_init();

	SetThickness(fThickness);
	SetStartFrame(0);
	UseFixedStartFrame(false);
	SetNumSamples(numSamples);
	ShowTrajectory(false);
	DrawSeedingLine(false);
	SetSeedLineEnd(ptSeedLineEnd);
}

CTimeLine::CTimeLine(const CDrawingObjectParams &params)
	:	CStreamLine(params.GetValuePointf(DOP_ORIGIN), params.GetValueInt(DOP_INTEGRATIONSTEPS), 0, params.GetValueFloatColor_r(DOP_COLOR), DO_TIMELINE, params.GetValueFloat(DOP_STEPLENGTH))
{
	SetParams(params);
}

void CTimeLine::_init()
{
	m_Points.clear();
	CPointf ptOrigin(GetOrigin());

	int numSamples (GetNumSamples());

	//generate the original seeding line
	CVector2D step ((GetSeedLineEnd() - ptOrigin) / static_cast<float>(numSamples) );
	CVector2D pos (ptOrigin.x, ptOrigin.y);

	for (int i=0; i < numSamples; i++) {
		m_Points.push_back( CPointf::fromVector2D(pos) );
		pos += step;
	}
}

CTimeLine::~CTimeLine(void)
{
}

void CTimeLine::GetParams(CDrawingObjectParams &params) const
{
	CStreamLine::GetParams(params);
	
	if (HasChildren()) {
		params.SetChildParams( m_SpeedLineParams );
	}
}

bool CTimeLine::setParam(DrawinObjectParamName paramID, const CSimpleVariant &val)
{
	bool bResult(CStreamLine::setParam(paramID, val));

	if (!bResult)
	{
		switch (paramID)
		{
			case DOP_PTEND:
				SetSeedLineEnd(val.GetPointfVal());
				bResult = true;
				break;
			case DOP_NUM_SAMPLES:
				SetNumSamples(val.GetIntVal());
				bResult = true;
				break;
			case DOP_SHOW_SEEDINGLINE:
				DrawSeedingLine(val.GetBoolVal());
				bResult = true;
				break;
			case DOP_USE_STARTFRAME:
				UseFixedStartFrame(val.GetBoolVal());
				bResult = true;
				break;
			case DOP_STARTFRAME:
				SetStartFrame(val.GetIntVal());
				bResult = true;
				break;
			case DOP_SHOWTRAJECTORY:
				ShowTrajectory(val.GetBoolVal());
				bResult = true;
				break;
		}
	}
	return bResult;
}

void CTimeLine::OnSetParams()
{
	_init();
}

void CTimeLine::Draw()
{
	if (ShowTrajectory()) {
		_drawChildren();
	}

	CStreamLine::Draw();

	if (DrawSeedingLine())
	{
		const floatColor& col = GetColor();
		CPointf ptOrigin(GetOrigin());

		int numSamples (GetNumSamples());

		CVector2D step ((GetSeedLineEnd() - ptOrigin) / static_cast<float>(numSamples) );
		CVector2D pos (ptOrigin.x, ptOrigin.y);

		glLineStipple(3, 0xAAAA);
		glEnable(GL_LINE_STIPPLE);

		glColor4f( col.red, col.green, col.blue, col.alpha/2.0f);

		glBegin(GL_LINE_STRIP);
		for (int i=0; i < numSamples; i++) {
			glVertex2fv( reinterpret_cast<GLfloat*>(&pos) );
			pos += step;
		}
		glEnd();

		glDisable(GL_LINE_STIPPLE);

		glColor4fv( const_cast<GLfloat*>(reinterpret_cast<const GLfloat*>(&col)) );
	}
}

void CTimeLine::AddTrajectory(CSpeedLine *pTrajectory)
{
	if (!m_SpeedLineParams) {
		CDrawingObjectParams *pParams = new CDrawingObjectParams(DO_SPEEDLINE);
		pTrajectory->GetParams( *pParams );
		m_SpeedLineParams = shared_ptr<CDrawingObjectParams>( pParams );
	} else {
		pTrajectory->SetParams( *m_SpeedLineParams.get() );
	}

	CDrawingObject::AddChild( pTrajectory );
}

bool CTimeLine::SetChildObjParams(const CDrawingObjectParams &params)
{
	bool bResult (CStreamLine::SetChildObjParams(params));

	if (bResult)
	{
		//update our struct
		if (params.GetValueInt(DOP_TYPE) == DO_SPEEDLINE)
		{
			for (auto iter = params.dataMap.begin(); iter != params.dataMap.end(); ++iter)
			{
				m_SpeedLineParams->SetValue( iter->first, iter->second );
			}
		}
	}

	return bResult;
}

CDrawingObject* CTimeLine::Duplicate() const
{
	CTimeLine* pRetVal = new CTimeLine(GetOrigin(), GetSeedLineEnd(), GetMaxIntegrationLen(), GetMaxIntegrationLen(), GetStepSize(), GetThickness(), GetColor());
	
	pRetVal->CopyVertices(this);
	pRetVal->CopyParams(this);

	pRetVal->CalcBoundingRect();

	return pRetVal;
}