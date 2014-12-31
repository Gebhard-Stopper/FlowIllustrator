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
#include "DrawingObjectDataTypes.h"

CDONames DrawingObjNames;

void CDONames::Init()
{
	m_TypeNames.Add(DO_NONE, "Unknown");
	m_TypeNames.Add(DO_VORTEX,"Vortex");
	m_TypeNames.Add(DO_CHARACTERISTIC_LINE, "Characteristic curve");
	m_TypeNames.Add(DO_STREAMLINE,"Stream line");
	m_TypeNames.Add(DO_STREAKLINE, "Streak line");
	m_TypeNames.Add(DO_PATHLINE, "Path line");
	m_TypeNames.Add(DO_TIMELINE, "Time line");
	m_TypeNames.Add(DO_RECTANGLE, "Rectangle");
	m_TypeNames.Add(DO_TRIANGLE, "Triangle");
	m_TypeNames.Add(DO_ELLIPSE, "Ellipse");
	m_TypeNames.Add(DO_LINE, "Line");
	m_TypeNames.Add(DO_POINT, "Point");
	m_TypeNames.Add(DO_BSPLINE, "B-Spline");
	m_TypeNames.Add(DO_ARROW, "Arrow");
	m_TypeNames.Add(DO_POLYLINE, "Poly line");
	m_TypeNames.Add(DO_SPEEDLINE, "Speed line");

	RegisterParameter(DOP_TYPE,						_T("objectType"),				DOT_INTEGER);
	RegisterParameter(DOP_COLOR,					_T("color"),					DOT_COLOR);
	RegisterParameter(DOP_THICKNESS,				_T("thickness"),				DOT_FLOAT);
	RegisterParameter(DOP_ROTATION,					_T("rotAngle"),					DOT_FLOAT);
	RegisterParameter(DOP_ISSOLID,					_T("IsSolid"),					DOT_BOOLEAN);
	RegisterParameter(DOP_ALPHA,					_T("alpha"),					DOT_FLOAT);
	RegisterParameter(DOP_WIDTH,					_T("width"),					DOT_FLOAT);
	RegisterParameter(DOP_HEIGHT,					_T("height"),					DOT_FLOAT);
	RegisterParameter(DOP_CENTER,					_T("center"),					DOT_POINT);
	RegisterParameter(DOP_CENTER_X,					_T("cx"),						DOT_FLOAT);
	RegisterParameter(DOP_CENTER_Y,					_T("cy"),						DOT_FLOAT);
	RegisterAlternativeName(DOP_CENTER_X,			_T("x"),						DOT_FLOAT);
	RegisterAlternativeName(DOP_CENTER_Y,			_T("y"),						DOT_FLOAT);
	RegisterParameter(DOP_DRAW_STIPPLED,			_T("drawStippled"),				DOT_BOOLEAN);
	RegisterParameter(DOP_INTEGRATIONSTEPS,			_T("numSteps"),					DOT_INTEGER);
	RegisterParameter(DOP_STEPLENGTH,				_T("stepLen"),					DOT_FLOAT);
	RegisterParameter(DOP_ORIGIN,					_T("origin"),					DOT_POINT);
	RegisterParameter(DOP_RENDER_AS_PARTICLES,		_T("asParticles"),				DOT_BOOLEAN);
	RegisterParameter(DOP_STARTFRAME,				_T("startFrame"),				DOT_INTEGER);
	RegisterParameter(DOP_USE_STARTFRAME,			_T("useFixedStartFrame"),		DOT_BOOLEAN);
	RegisterParameter(DOP_DRAWASDROPLETS,			_T("drawAsDroplet"),			DOT_BOOLEAN);
	RegisterParameter(DOP_HALOCOLOR,				_T("haloColor"),				DOT_COLOR);
	RegisterParameter(DOP_ARROWCOLOR,				_T("arrowColor"),				DOT_COLOR);
	RegisterParameter(DOP_SMOOTHNESS,				_T("smoothness"),				DOT_INTEGER);
	RegisterParameter(DOP_VORTEXSTYLE,				_T("vortexStyle"),				DOT_INTEGER);
	RegisterParameter(DOP_VORTEXDIR,				_T("rotationDir"),				DOT_INTEGER);
	RegisterParameter(DOP_ARROWSIZE,				_T("arrowLen"),					DOT_FLOAT);
	RegisterParameter(DOP_THRESHOLD,				_T("vortexThreshold"),			DOT_FLOAT);
	RegisterParameter(DOP_AUTOADJUSTSIZE,			_T("autoAdjustSize"),			DOT_BOOLEAN);
	RegisterParameter(DOP_SHOWTRAJECTORY,			_T("showTrajectory"),			DOT_BOOLEAN);
	RegisterParameter(DOP_TRANSPARENTTRAJECTORY,	_T("trajectoryTransparent"),	DOT_BOOLEAN);
	RegisterParameter(DOP_REVOLUTIONS,				_T("numRevolutions"),			DOT_FLOAT);
	RegisterParameter(DOP_NUMARROWS,				_T("numArrows"),				DOT_INTEGER);
	RegisterParameter(DOP_TRAJECTORYSTEPS,			_T("trajectorySteps"),			DOT_INTEGER);
	RegisterParameter(DOP_SHOWARROWS,				_T("showArrows"),				DOT_BOOLEAN);
	RegisterParameter(DOP_APPEARANCE,				_T("appearance"),				DOT_INTEGER);
	RegisterParameter(DOP_ARROW_DIR,				_T("arrowDir"),					DOT_VECTOR);
	RegisterParameter(DOP_RADIUS1,					_T("rx"),						DOT_FLOAT);
	RegisterParameter(DOP_RADIUS2,					_T("ry"),						DOT_FLOAT);
	RegisterAlternativeName(DOP_RADIUS1,			_T("r1"),						DOT_FLOAT);
	RegisterAlternativeName(DOP_RADIUS2,			_T("r2"),						DOT_FLOAT);
	RegisterParameter(DOP_HATCHED,					_T("hatched"),					DOT_BOOLEAN);
	RegisterParameter(DOP_PTEND,					_T("ptEnd"),					DOT_POINT);
	RegisterParameter(DOP_NUM_SAMPLES,				_T("numSamples"),				DOT_INTEGER);
	RegisterParameter(DOP_SHOW_SEEDINGLINE,			_T("showSeedline"),				DOT_BOOLEAN);
	RegisterParameter(DOP_SPEEDLINE_STYLE,			_T("speedLineStyle"),			DOT_INTEGER);
	RegisterParameter(DOP_ALPHA_MIN,				_T("alphaMin"),					DOT_FLOAT);
	RegisterParameter(DOP_THICKNESS_MIN,			_T("thicknessMin"),				DOT_FLOAT);
	RegisterParameter(DOP_NUMDROPLETS,				_T("numDroplets"),				DOT_INTEGER);
	RegisterParameter(DOP_GROW_STEPS,				_T("growSteps"),				DOT_INTEGER);
	RegisterParameter(DOP_TRANSPARENT_STEPS,		_T("transparentSteps"),			DOT_INTEGER);	
}

CString CDONames::GetTypeName(DrawingObjectType nType) const
{
	return m_TypeNames.Get(nType);
}

DrawingObjectType CDONames::GetType(const CString& strName) const
{
	return static_cast<DrawingObjectType>(m_TypeNames.Get(strName));
}

CString CDONames::GetParamName(DrawinObjectParamName nParam) const
{
	return m_ParamNames.Get(nParam);
}

DrawinObjectParamName CDONames::GetParam(const CString &strName) const
{
	return static_cast<DrawinObjectParamName>(m_ParamNames.Get(strName));
}

void CDONames::RegisterParameter(DrawinObjectParamName nParamName, const CString& strParamName, DrawinObjectParamType nParamDataType)
{
	CT2CA pszConvertedAnsiString (strParamName);
	string str (pszConvertedAnsiString);

	m_ParamNames.Add(nParamName, str);
	m_ParamDataTypes.insert( pair<int, int>(nParamName, nParamDataType) );
}

void CDONames::RegisterAlternativeName(DrawinObjectParamName nParamName, const CString& strParamName, DrawinObjectParamType nParamDataType)
{
	CT2CA pszConvertedAnsiString (strParamName);
	string str (pszConvertedAnsiString);

	m_ParamNames.AddAlternative(nParamName, str);
	m_ParamDataTypes.insert( pair<int, int>(nParamName, nParamDataType) );
}

DrawinObjectParamType CDONames::GetParamDataType(DrawinObjectParamName nParamName) const
{
	auto iter = m_ParamDataTypes.find (nParamName);
	if (iter != m_ParamDataTypes.end()) 
		return static_cast<DrawinObjectParamType>((*iter).second);

	return DOT_INVALID;
}