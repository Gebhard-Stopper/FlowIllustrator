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
#include "Pointf.h"
#include "Vector2D.h"
#include "helper.h"

/**
 * CRectF represents a rectangular structure with floating point precission.
 *
 */
class CRectF
{
public:
	CVector2D	m_Min;			/**< The lower-left edge of the rectangle */
	CVector2D	m_Max;			/**< The upper-right edge of the rectangle */
	float		m_fRotation;	/**< The rotation of the rectangle about the z-axis in degrees */

public:
	/**
	 *	Construct a new CRectF object.
	 *
	 *	@remarks This constructor creates an empty rectangle with m_Min=m_Max=(0,0)
	 */
	CRectF(void) 
		: m_Min(0.0f, 0.0f), m_Max(0.0f, 0.0f), m_fRotation(0.0f) {}
	
	/**
	 *	Construct a new CRectF object.
	 *
	 *	@param xMin	X-component of the lower-left edge
	 *	@param yMin Y-component of the lower-left edge
	 *	@param xMax X-component of the upper-right edge
	 *	@param yMax Y-component of the upper-right edge
	 */
	CRectF(float xMin, float yMin, float xMax, float yMax)
		: m_Min(xMin, yMin), m_Max(xMax, yMax), m_fRotation(0.0f) {}

	/**
	 *	Construct a new CRectF object.
	 *
	 *	@param ptMin The lower-left edge.
	 *	@param ptMax The lower-left edge.
	 */
	CRectF (const CPointf &ptMin, const CPointf &ptMax)
		: m_Min(CVector2D(ptMin.x, ptMin.y)), m_Max(CVector2D(ptMax.x, ptMax.y)), m_fRotation(0.0f) {}

	/**
	 *	Construct a new CRectF object.
	 *
	 *	@param rect CRectF to be copied.
	 */
	CRectF (const CRectF &rect)
		: m_Min(rect.m_Min), m_Max(rect.m_Max) {}

	CRectF (const CPointf &ptCenter, float fWidth, float fHeight) 
		: m_Min(ptCenter.x - fWidth/2.0f, ptCenter.y - fHeight/2.f), m_Max(ptCenter.x + fWidth/2.0f, ptCenter.y + fHeight/2.f) {}

	/**
	 *	Destroy this CRectF object and its data.
	 */
	~CRectF(void);

public:
	/**
	 *	Retrieves the height of this CRectF. This function does not consider the rotation of this CRectF.
	 *
	 *	@return The hight as float.
	 */
	__inline float	getHeight() const { return m_Max.y - m_Min.y; }

	/**
	 *	Retrieves the width of this CRectF. This function does not consider the rotation of this CRectF.
	 *
	 *	@return The width as float.
	 */
	__inline float	getWidth() const { return  m_Max.x - m_Min.x; }

	/**
	 *	This function tests if the specified point lies inside this CRectF.
	 *
	 *	@param x X-component of the point
	 *	@param y Y-component of the point
	 *
	 *	@return This function returns true, if the specified point lies inside the CRectf or exactly on its boundaries.
				Otherwise, this function return false.
	 */
	__inline bool	PtInRect(float x, float y) const {
		return PtInRect(CPointf(x,y));
	}

	/**
	 *	This function tests if the specified CPoint lies inside this CRectF.
	 *
	 *	@param point A reference to the CPoint to be tested.
	 *
	 *	@return This function returns true, if the specified point lies inside the CRectf or exactly on its boundaries.
				Otherwise, this function return false.
	 */
	__inline bool PtInRect(const CPoint& point) const {
		return PtInRect(CPointf( static_cast<float>(point.x), static_cast<float>(point.y) ));
	}

	/**
	 *	This function tests if the specified CPointf lies inside this CRectF.
	 *
	 *	@param point A reference to the CPointf to be tested.
	 *
	 *	@return This function returns true, if the specified point lies inside the CRectf or exactly on its boundaries.
				Otherwise, this function return false.
	 */
	__inline bool PtInRect(const CPointf& point) const {
		return ((m_Min.x <= point.x && m_Max.x >= point.x) && (m_Min.y < point.y && m_Max.y >= point.y));
	}

	/**
	 *	Translates this CRectF in X and Y-direction.
	 *
	 *	@param tx Amount to translate this CRectF in X-direction
	 *	@param ty Amount to translate this CRectF in Y-direction
	 *
	 */
	__inline void Translate(float tx, float ty) {
		CVector2D dummy(tx, ty);
		m_Min += dummy;
		m_Max += dummy;
	}

	/**
	 *	Tests if a CRectF intersects this CRectF
	 *
	 *	@param rect Reference to the CRectF to test against.
	 *
	 *	@return This function returns true, if the two CRectF intersect, i.e. the rectangle
				defining the area occipied by both CRectF objects is not empty or negarive.
	 */
	__inline bool IntersectRet(const CRectF &rect) {
		CRectF rcDummy (	max (m_Min.x, rect.m_Min.x),
							max (m_Min.y, rect.m_Min.y),
							min (m_Max.x, rect.m_Max.x),
							min (m_Max.y, rect.m_Max.y) );

		return (rcDummy.m_Min.x < rcDummy.m_Max.x && rcDummy.m_Min.y < rcDummy.m_Max.y);
	}

	/**
	 *	This function ensures that m_Min <= m_Max.
	 *	
	 */
	__inline void Normalize() {
		if (m_Min.x > m_Max.x) {
			float dummy		= m_Min.x;
			m_Min.x			= m_Max.x;
			m_Max.x			= dummy;
		}
		if (m_Min.y > m_Max.y) {
			float dummy		= m_Min.y;
			m_Min.y			= m_Max.y;
			m_Max.y			= dummy;
		}
	}

	/**
	 *	InflateRect inflates CRectF by moving its sides away from its center.
	 *
	 *	@param x Specifies the number of units to inflate the left and right sides of CRectF.
	 *	@param y Specifies the number of units to inflate the top and bottom of CRectF.
	 */
	__inline void InflateRect(float x, float y) {
		m_Min.x -= x;
		m_Min.y -= y;
		m_Max.x += x;
		m_Max.y += y;
	}

	/**
	 *	InflateRect inflates CRectF by moving its sides away from its center.
	 *
	 *	@param rect Reference to a CRectF object that specifies the number of units to inflate each side.
	 */
	__inline void InfalteRect(const CRectF &rect) {
		m_Min.x -= rect.m_Min.x;
		m_Min.y -= rect.m_Min.y;
		m_Max.x += rect.m_Max.x;
		m_Max.y += rect.m_Max.y;
	}

	/**
	 *	Tests if two CRectF are identical.
	 *
	 *	@return Returns true if the two CRectF are identical, otherwise false.
	 */
	__inline bool EqualRect (const CRectF &rect) {
		return (m_Max == rect.m_Max && m_Min == rect.m_Min);
	}

	/**
	 *	Convert a CRectF to a CRect.
	 *
	 *	@return The resulting CRect.
	 *
	 *	@remarks	All values of the original CRectF are rounded down for the CRect.
					This may cause deviations from the original rect.
	 */
	__inline CRect GetCRect() const {
		CRect retVal(	static_cast<int>(m_Min.x), 
						static_cast<int>(m_Min.y), 
						static_cast<int>(m_Max.x), 
						static_cast<int>(m_Max.y) );

		retVal.NormalizeRect();
		return retVal;
	}

	/**
	 * Create a CRectF from a CRect.
	 *
	 *	@param rect The Source CRect.
	 */
	static __inline CRectF FromCRect(const CRect& rect) {
		CRectF retVal(	static_cast<float>(rect.left), 
						static_cast<float>(rect.top), 
						static_cast<float>(rect.right), 
						static_cast<float>(rect.bottom));

		//retVal.Normalize();
		return retVal;
	}

	/**
	 *	Retrieve the center point of this CRectF.
	 *
	 *	@retval The Center as CVector2D.
	 */
	__inline CVector2D GetCenter() const { return _getCenter(); }
	
	/**
	 *	Modify the center of this CRectF.
	 *	This function maintains the extent of the CRectF, but translates it, according to the new center location.
	 *
	 *	@param center A reference to a CVector2D, specifying the new center of the CRectF.
	 */
	__inline void SetCenter(const CVector2D& center) {
		float w = getWidth();
		float h = getHeight();

		m_Min.x = center.x - (w/2.0f);
		m_Min.y = center.y - (h/2.0f);
		m_Max.x = center.x + (w/2.0f);
		m_Max.y = center.y + (h/2.0f);
	}

	/**
	 *	Modify the center of this CRectF.
	 *	This function maintains the extent of the CRectF, but translates it, according to the new center location.
	 *
	 *	@param center A reference to a CPointf, specifying the new center of the CRectF.
	 */
	__inline void SetCenter(const CPointf& center) {
		float w = getWidth();
		float h = getHeight();

		m_Min.x = center.x - (w/2.0f);
		m_Min.y = center.y - (h/2.0f);
		m_Max.x = center.x + (w/2.0f);
		m_Max.y = center.y + (h/2.0f);
	}

	/**
	 *	Modify the width of this CRectF.
	 *	This function modifies the X-Components of m_Min and m_Max, to meet the new width.
	 *
	 *	@param fWidth The new width as float.
	 */
	__inline void SetWidth(float fWidth) {
		CVector2D ptCenter(GetCenter());

		m_Min.x = ptCenter.x - (fWidth/2.0f);
		m_Max.x = ptCenter.x + (fWidth/2.0f);
	}

	/**
	 *	Modify the height of this CRectF.
	 *	This function modifies the >-Components of m_Min and m_Max, to meet the new width.
	 *
	 *	@param fHeight The new height as float.
	 */
	__inline void SetHeight(float fHeight) {
		CVector2D ptCenter(GetCenter());

		m_Min.y = ptCenter.y - (fHeight/2.0f);
		m_Max.y = ptCenter.y + (fHeight/2.0f);
	}

	/**
	 *	This function determines, if this CRectF is empty.
	 *	A CRectF is empty, if either one of its width or its height is zero.
	 *
	 *	@return This function returns true, if the CRectF is empty, otherwise false.
	 */
	__inline bool IsEmptyRect() const {
		return (getHeight() == 0 || getWidth() ==0);
	}

	/**
	 *	Scale this CRectF, by multiplying the components of the lower-left and upper-right corners with their respective factors.
	 *
	 *	@param fScalefactorX Scaling factor for the X-components of the corners.
	 *	@param fScalefactorY Scaling factor for the Y-components of the corners.
	 */
	__inline void Scale( float fScalefactorX, float fScalefactorY) {
		m_Min.x *= fScalefactorX;
		m_Max.x *= fScalefactorX;
		m_Min.y *= fScalefactorY;
		m_Max.y *= fScalefactorY;
	}

	__inline CRectF DiffRect(const CRectF& rect) {
		return CRectF ( rect.m_Min.x - m_Min.x,
						rect.m_Min.y - m_Min.y,
						m_Max.x - rect.m_Max.x,
						m_Max.y - rect.m_Max.y );
	}

private:
	__inline CVector2D _getCenter() const {
		return CVector2D(m_Min.x + getWidth()/2.0f, m_Min.y + getHeight()/2.0f);
	}

	__inline void _SetRotation(float nDegree) {
		m_fRotation = (nDegree < 0)? 360.0f+fmod(nDegree, 360.0f) : fmod(nDegree, 360.0f);
	}
};

