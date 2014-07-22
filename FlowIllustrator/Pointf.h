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

#include "Vector2D.h"

/**
 *	Class that represents a two-dimensional point.
 *	The coordinates of the point are stored as variables of type float.
 */
class CPointf
{
public:
	float x;	/**< X-coordinate of this point. */
	float y;	/**< Y-coordinate of this point. */

public:
	/**
	 *	Construct a new CPointf object.
	 *	The coordinates of the point are set to (0.0f, 0.0f).
	 */
	__inline CPointf() : x(0.0f), y(0.0f) {}

	/**
	 *	Construct a new CPointf object, by copying the coordinates of the point supplied as the parameter.
	 *		
	 *	@param point Point to copy the coordinates from
	 */

	__inline CPointf(const CPoint& point) : x(static_cast<float>(point.x)), y(static_cast<float>(point.y)) {}

	/**
	 *	Construct a new CPointf object, by copying the coordinates of the supplied x and y-components.
	 *		
	 *	@param x The x-component of the point.
	 *	@param y The y-component of the point.
	 */
	__inline CPointf(float xPos, float yPos) : x(xPos), y(yPos) {}

public:
	/**
	 *	Scaling operator.
	 *		
	 *	@param f The scale factor
	 *
	 *	@return A scaled copy of the original CPointf.
	 */
	__inline CPointf operator * (float f) const {
		return CPointf(x*f, y*f);
	}

	/**
	 *	Translation operator.
	 *		
	 *	@param point CPointf, whose componets are use to translate a CPoint f in x and y-direction respectively.
	 *
	 *	@return A translated copy of the original CPointf.
	 *
	 */
	__inline CPointf operator + (const CPointf& point) const{
		return CPointf(x+point.x, y+point.y);
	}

	/**
	 *	Translation operator.
	 *		
	 *	@param point CPointf, whose componets are use to translate a CPoint f in x and y-direction respectively.
	 *
	 *	@return A reference to the translated point.
	 *
	 */
	__inline CPointf& operator += (const CPointf& point) {
		x += point.x;
		y += point.y;
		return *this;
	}

	/**
	 *	Translation operator.
	 *		
	 *	@param vec CVector2D, whose componets are use to translate a CPoint f in x and y-direction respectively.
	 *
	 *	@return The translated point as CVector2D.
	 *
	 */
	__inline CVector2D operator + (const CVector2D& vec) {
		return CVector2D(x+vec.x, y+vec.y);
	}

	/**
	 *	Translation operator.
	 *		
	 *	@param point CPointf, to be subtracted from this CPointf.
	 *
	 *	@return The resulting CVector2D.
	 *
	 */
	__inline CVector2D operator - (const CPointf point) const {
		return CVector2D(x-point.x, y-point.y);
	}

	/**
	 *	Negation operator.
	 *
	 *	@return A negated copy of the original CPointf.
	 *
	 */
	__inline CPointf operator - () const{
		return CPointf(-x, -y);
	}


	/**
	 *	Converts a CPoint to CPointf.
	 *
	 *	@param point Reference to the point to be converted.
	 *
	 *	@return The corresponding CPointf.
	 */
	__inline static CPointf GetPointf(const CPoint& point) {
		return CPointf( static_cast<float>(point.x), static_cast<float>(point.y) );
	}

	/**
	 *	Converts a CPointf to CPoint.
	 *
	 *	@return The corresponding CPoint.
	 */
	__inline CPoint ToCPoint() const {
		return CPoint(static_cast<int>(x), static_cast<int>(y));
	}

	/**
	 *	Converts a CVector2D to CPointf.
	 *
	 *	@param vector Reference to the CVector2D to be converted
	 *
	 *	@return The corresponding CPointf.
	 */
	__inline CPointf& operator = (const CVector2D& vector) {
		x = vector.x;
		y = vector.y;
		return *this;
	}

	/**
	 *	Converts a CPointf to CVector2D.
	 *
	 *	@param point Reference to the CPointf to be converted
	 *
	 *	@return The corresponding CVector2D.
	 */
	static __inline CVector2D toVector2D(const CPointf &point) {
		return CVector2D(point.x, point.y);
	}

	/**
	 *	Converts this CPointf to CVector2D.
	 *
	 *	@return The corresponding CVector2D.
	 */
	__inline CVector2D toVector2D() const {
		return CVector2D(x,y);
	}

	/**
	 *	Created a CPointf from a CVector2D
	 *
	 *	@param vec Reference to the CVector2D to be converted.
	 *
	 *	@return The corresponding CPointf.
	 */
	static __inline CPointf fromVector2D(const CVector2D &vec) {
		return CPointf(vec.x, vec.y);
	}

	/**
	 *	Retrieve a textual representation of this CPointf.
	 *
	 *	@return A CString with the comma-separated x and y components.
	 */
	__inline CString ToString() const {
		CString str;
		str.Format(_T("%f,%f"), x, y);
		return str;		
	}
};

