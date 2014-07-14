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

	m_ParamNames.Add(DOP_TYPE, "objectType");
	m_ParamNames.Add(DOP_COLOR, "color");
	m_ParamNames.Add(DOP_THICKNESS, "thickness");
	m_ParamNames.Add(DOP_ROTATION, "rotAngle");
	m_ParamNames.Add(DOP_ISSOLID, "IsSolid");
	m_ParamNames.Add(DOP_ALPHA, "alpha");
	m_ParamNames.Add(DOP_WIDTH, "width");
	m_ParamNames.Add(DOP_HEIGHT, "height");
	m_ParamNames.Add(DOP_CENTER, "center");
	m_ParamNames.Add(DOP_DRAW_STIPPLED, "drawStippled");
	m_ParamNames.Add(DOP_INTEGRATIONSTEPS, "numSteps");
	m_ParamNames.Add(DOP_STEPLENGTH, "stepLen");
	m_ParamNames.Add(DOP_ORIGIN, "origin");
	m_ParamNames.Add(DOP_RENDER_AS_PARTICLES, "asParticles");
	m_ParamNames.Add(DOP_STARTFRAME, "startFrame");
	m_ParamNames.Add(DOP_USE_STARTFRAME, "useFixedStartFrame");
	m_ParamNames.Add(DOP_DRAWASDROPLETS, "drawAsDroplet");
	m_ParamNames.Add(DOP_HALOCOLOR, "haloColor");
	m_ParamNames.Add(DOP_ARROWCOLOR, "arrowColor");
	m_ParamNames.Add(DOP_SMOOTHNESS, "smoothness");
	m_ParamNames.Add(DOP_VORTEXSTYLE, "vortexStyle");
	m_ParamNames.Add(DOP_VORTEXDIR, "rotationDir");
	m_ParamNames.Add(DOP_ARROWSIZE, "arrowLen");
	m_ParamNames.Add(DOP_THRESHOLD, "vortexThreshold");
	m_ParamNames.Add(DOP_AUTOADJUSTSIZE, "autoAdjustSize");
	m_ParamNames.Add(DOP_SHOWTRAJECTORY, "showTrajectory");
	m_ParamNames.Add(DOP_TRANSPARENTTRAJECTORY, "trajectoryTransparent");
	m_ParamNames.Add(DOP_REVOLUTIONS, "numRevolutions");
	m_ParamNames.Add(DOP_NUMARROWS, "numArrows");
	m_ParamNames.Add(DOP_TRAJECTORYSTEPS, "trajectorySteps");
	m_ParamNames.Add(DOP_SHOWARROWS, "showArrows");
	m_ParamNames.Add(DOP_APPEARANCE, "appearance");
	m_ParamNames.Add(DOP_ARROW_DIR, "arrowDir");
	m_ParamNames.Add(DOP_RADIUS1, "r1");
	m_ParamNames.Add(DOP_RADIUS2, "r2");
	m_ParamNames.Add(DOP_HATCHED, "hatched");
	m_ParamNames.Add(DOP_PTEND, "ptEnd");
	m_ParamNames.Add(DOP_NUM_SAMPLES, "numSamples");
	m_ParamNames.Add(DOP_SHOW_SEEDINGLINE, "showSeedline");

	m_ParamNames.Add(DOP_SPEEDLINE_STYLE, "speedLineStyle");
	m_ParamNames.Add(DOP_ALPHA_MIN, "alphaMin");
	m_ParamNames.Add(DOP_THICKNESS_MIN, "thicknessMin");

	m_ParamNames.Add(DOP_NUMDROPLETS, "numDroplets");
	m_ParamNames.Add(DOP_GROW_STEPS, "growSteps");
	m_ParamNames.Add(DOP_TRANSPARENT_STEPS, "transparentSteps");	
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
