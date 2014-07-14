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

#pragma once
#include "drawingobject.h"
#include "Pointf.h"


//The order of the b-Spline, for now we are fixed (=degree 3)
#define ORDER 4	
#define DEGREE (ORDER-1)

class CBSpline :
	public CDrawingObject
{
protected:
	//CBSpline() : CDrawingObject(CRectF(0.0f, 0.0f, 0.0f, 0.0f), DO_BSPLINE) {}

public:
	CBSpline(int numPoints, const floatColor& color);
	~CBSpline(void);

public:
	int			 m_nNumPoints;	//Number of control points (n)
	CPointf		*m_pPoints;		//deBoor points
	float		*m_pKnots;		//knot vector

public:
	virtual void Draw();
	virtual CDrawingObject* Duplicate() const;
#ifdef WIN32
	virtual CString toString(float fLineWidthFactor) const;
#else
	virtual std::string toString() const;
#endif

	void CalcKnotVector();

protected:
	virtual bool setParam(DrawinObjectParamName paramID, const CSimpleVariant &val);
	virtual void _OnParamsChanged() {}

protected:
	CPointf deBoor(int j, int i, float t);
	int getR(float t);
};

