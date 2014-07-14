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
#include "PathLine.h"


CPathLine::CPathLine(const CPointf &ptOrigin, unsigned int nNumIntegrationSteps, int nStartFrame, float fArrowLength, const floatColor& color, float fStepLength)
	:	CStreamLine(ptOrigin, nNumIntegrationSteps, fArrowLength, color, DO_PATHLINE, fStepLength)
{
	SetStartFrame(nStartFrame);
}

CPathLine::CPathLine(const CPointf &ptOrigin, unsigned int nNumIntegrationSteps, int nStartFrame, float fArrowLength, const floatColor& color, DrawingObjectType type, float fStepLength)
	:	CStreamLine(ptOrigin, nNumIntegrationSteps, fArrowLength, color, type, fStepLength)
{
	SetStartFrame(nStartFrame);
}

CPathLine::CPathLine(const CDrawingObjectParams &params)
	: CStreamLine(DO_PATHLINE)
{
	SetParams(params);
}

CPathLine::CPathLine(DrawingObjectType nType)
	: CStreamLine(nType)
{
}

CPathLine::~CPathLine(void)
{
}

CDrawingObject* CPathLine::Duplicate() const
{
	CPathLine* pRetVal = new CPathLine(GetOrigin(), GetMaxIntegrationLen(), GetStartFrame(), GetArrowSize(), GetColor(), GetStepSize());
	
	pRetVal->CopyVertices(this);
	pRetVal->CopyParams(this);

	pRetVal->CalcBoundingRect();

	return pRetVal;
}

bool CPathLine::setParam(DrawinObjectParamName paramID, const CSimpleVariant &val)
{
	bool bResult(CStreamLine::setParam(paramID, val));

	if (!bResult)
	{
		switch (paramID)
		{
			case DOP_STARTFRAME:
				SetStartFrame(val.GetIntVal());
				bResult = true;
				break;
			case DOP_USE_STARTFRAME:
				UseFixedStartFrame(val.GetBoolVal());
				bResult = true;
				break;
		}
	}

	return bResult;
}