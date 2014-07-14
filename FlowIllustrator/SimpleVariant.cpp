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
#include "SimpleVariant.h"

const floatColor	defaultColor(0,0,0);
const CPointf		defaultPoint(0,0);
const CVector2D		defaultVector(0,0);

#ifdef WIN32
CString CSimpleVariant::GetString(bool bHex) const
{
	CString retVal;

	switch(nType)
	{
		case SVT_INTEGER:
			(bHex)? retVal.Format(_T("0x%X"), iVal) : retVal.Format(_T("%d"), iVal);
			break;
		case SVT_UINTEGER:
			(bHex)? retVal.Format(_T("0x%X"), uiVal) : retVal.Format(_T("%d"), uiVal);
			break;
		case SVT_FLOAT:
			(bHex)? retVal.Format(_T("%A"), fVal) : retVal.Format(_T("%f"), fVal);
			break;
		case SVT_DOUBLE:
			(bHex)? retVal.Format(_T("%A"), dblVal) : retVal.Format(_T("%f"), dblVal);
			break;
		case SVT_BOOLEAN:
			retVal.Format(_T("%d"), bVal);
			break;
		case SVT_POINT:
			retVal.Format(_T("%f,%f "), point.x, point.y);
			break;
		case SVT_VECTOR2D:
			retVal.Format(_T("%f,%f "), vec2D.x, vec2D.y);
			break;
		case SVT_FLOATCOL:
			retVal = color.GetHexString();
			break;
	}

	return retVal;
}
#endif

int CSimpleVariant::GetIntVal() const
{
	switch(nType)
	{
		case SVT_INTEGER:	return iVal;
		case SVT_UINTEGER:	return static_cast<int>(uiVal);
		case SVT_FLOAT:		return static_cast<int>(fVal);
		case SVT_DOUBLE:	return static_cast<int>(dblVal);
		case SVT_BOOLEAN:	return static_cast<int>(bVal);
	}

	return 0;
}

unsigned int CSimpleVariant::GetUIntVal() const
{
	switch(nType)
	{
		case SVT_INTEGER:	return static_cast<unsigned int>(iVal);
		case SVT_UINTEGER:	return uiVal;
		case SVT_FLOAT:		return static_cast<unsigned int>(fVal);
		case SVT_DOUBLE:	return static_cast<unsigned int>(dblVal);
		case SVT_BOOLEAN:	return static_cast<unsigned int>(bVal);
	}

	return 0;
}

float CSimpleVariant::GetFloatVal() const
{
	switch(nType)
	{
		case SVT_INTEGER:	return static_cast<float>(iVal);
		case SVT_UINTEGER:	return static_cast<float>(uiVal);
		case SVT_FLOAT:		return fVal;
		case SVT_DOUBLE:	return static_cast<float>(dblVal);
		case SVT_BOOLEAN:	return static_cast<float>(bVal);
	}

	return 0.0f;
}

double CSimpleVariant::GetDblVal() const
{
	switch(nType)
	{
		case SVT_INTEGER:	return static_cast<double>(iVal);
		case SVT_UINTEGER:	return static_cast<double>(uiVal);
		case SVT_FLOAT:		return static_cast<double>(fVal);
		case SVT_DOUBLE:	return dblVal;
		case SVT_BOOLEAN:	return static_cast<double>(bVal);
	}

	return 0.0;
}

bool CSimpleVariant::GetBoolVal() const
{
	switch(nType)
	{
		case SVT_INTEGER:	return iVal==0?false:true;
		case SVT_UINTEGER:	return uiVal==0?false:true;
		case SVT_BOOLEAN:	return bVal;
	}

	return false;
}

const floatColor& CSimpleVariant::GetColorVal() const
{
	if(nType == SVT_FLOATCOL)
	{
		return color;
	}
		
	return defaultColor;
}

const CPointf& CSimpleVariant::GetPointfVal() const
{
	if(nType == SVT_POINT)
	{
		return point;
	}
		
	return defaultPoint;
}

const CVector2D& CSimpleVariant::GetVector2DVal() const
{
	if(nType == SVT_VECTOR2D)
	{
		return vec2D;
	}
		
	return defaultVector;
}
