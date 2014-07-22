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
public:
	CBSpline(int numPoints, const floatColor& color);
	~CBSpline(void);

public:
	int			 m_nNumCtrlPoints;	/**< Number of control points. */
	CPointf		*m_pPoints;			/**< Pointer to an array of CPointf, holding the deBoor points. */
	float		*m_pKnots;			/**< Pointer to an array of float, holding the knot vector. */

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
	/**
	 *	Set one or more parameters to a CDrawingObject.
	 *
	 *	@param	paramID The unique DrawinObjectParamName of this parameter.
	 *	@param	val Reference to a CSimpleVariant, representing the value of the parameter.
	 *	
	 *	@remarks	This function must be implemented in derived classes to ensure that all parameters are
	 *				set correctly.
	 *
	 *	@see CDrawingObject::setParam()
	 */
	virtual bool setParam(DrawinObjectParamName paramID, const CSimpleVariant &val);
	virtual void _OnParamsChanged() {}

protected:
	CPointf deBoor(int j, int i, float t);
	int getR(float t);
};

