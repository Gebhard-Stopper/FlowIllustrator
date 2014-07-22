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
#include <vector>

class CArrow :
	public CDrawingObject
{
public:
	/**
	 *	Create and initialize a new CArrow object.
	 *
	 *	@param ptStart The origin of the arrow.
	 *	@param dir The direction in which the arrow is pointing.
	 *	@param fLength The lenght of the arrow in domain space.
	 *	@param color The color of this CArrow as reference to a floatColor.
	 */
	CArrow(CPointf ptStart, CVector2D dir, float fLength, const floatColor& color);
	virtual ~CArrow(void);

protected:
	CPointf		m_ptStart;	/**< The origin of the arrow.*/
	CVector2D	m_vecDir;	/**< The direction in which the arrow is pointing. */
	float		m_fLength;	/**< The lenght of the arrow in domain space. */

public:
	/**
	 *	Draw the arrow on the screen.
	 */
	virtual void Draw();

	/**
	 *	Translate the arrow in x and y direction.
	 *
	 *	@param tx Translation in x-direction in domain coordinates.
	 *	@param ty Translation in y-direction in domain coordinates.
	 */
	virtual void Translate(float tx, float ty);

	/**
	 *	Generates and returns an exact copy of this CArrow.
	 *
	 *	@return A new CArrow object with an exact copy all parameters of this CArrow obejct.
	 */
	virtual CDrawingObject* Duplicate() const;

#ifdef WIN32
	/**
	 *	Generates a SVG compatible string representation of this CArrow.
	 *
	 *	@return A string in SVG format.
	 */
	virtual CString toString(float fLineWidthFactor) const;
#endif

private:
	void _getVertices(std::vector<CVector2D> &points) const;

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

	/**
	 *	Empty override for CArrow.
	 */
	virtual void _OnParamsChanged() {}
};

