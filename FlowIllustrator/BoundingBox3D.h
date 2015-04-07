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
#include "Vector3D.h"
using namespace FICore;

class CBoundingBox3D
{
public:
	public:
	CVector3D	m_Min;
	CVector3D	m_Max;
	float		m_fRotation;

public:
	CBoundingBox3D(void) 
		: m_Min(0.0f, 0.0f, 0.0f), m_Max(0.0f, 0.0f, 0.0f), m_fRotation(0.0f) {}
	
	CBoundingBox3D(float xMin, float yMin, float zMin, float xMax, float yMax, float zMax)
		: m_Min(xMin, yMin, zMin), m_Max(xMax, yMax, zMax), m_fRotation(0.0f) {}

	~CBoundingBox3D(void);

public:
	__inline float	getHeight() const { return m_Max.y - m_Min.y; }
	__inline float	getWidth() const { return  m_Max.x - m_Min.x; }
	__inline float	getDepth() const { return  m_Max.x - m_Min.x; }

	//Simple test it the specified point is on or within the boundaries of the rectangle
	__inline bool PtInRect(float x, float y, float z) const {
		return ((m_Min.x <= x && m_Max.x >= x) && (m_Min.y < y && m_Max.y >= y) && (m_Min.z < z && m_Max.z >= z));
	}

	__inline void Translate(float tx, float ty, float tz) {
		CVector3D dummy(tx, ty, tz);
		m_Min += dummy;
		m_Max += dummy;
	}

	__inline bool IntersectBoundingBox(const CBoundingBox3D &rect) {
		CBoundingBox3D rcDummy (	max (m_Min.x, rect.m_Min.x),
									max (m_Min.y, rect.m_Min.y),
									max (m_Min.z, rect.m_Min.z),
									min (m_Max.x, rect.m_Max.x),
									min (m_Max.y, rect.m_Max.y),
									min (m_Max.z, rect.m_Max.z));

		return (rcDummy.m_Min.x < rcDummy.m_Max.x && rcDummy.m_Min.y < rcDummy.m_Max.y && rcDummy.m_Min.z < rcDummy.m_Max.z);
	}

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

	__inline void InflateBoundingBox(float x, float y, float z) {
		CVector3D dummy(x,y,z);
		m_Min -= dummy;
		m_Max += dummy;
	}

	__inline void InflateBoundingBox(const CBoundingBox3D &rect) {
		m_Min -= rect.m_Min;
		m_Max += rect.m_Max;
	}

	__inline bool EqualRect (const CBoundingBox3D &rect) {
		return (m_Max == rect.m_Max && m_Min == rect.m_Min);
	}


	__inline CVector3D GetCenter() const { return _getCenter(); }
	
	__inline void SetCenter(const CVector3D& center) {
		CVector3D dummy(getWidth()/2.0f, getHeight()/2.0f, getDepth()/2.0f);
		m_Min = center - dummy;
		m_Max = center + dummy;		
	}

	__inline void SetWidth(float fWidth) {
		CVector3D ptCenter(GetCenter());

		m_Min.x = ptCenter.x - (fWidth/2.0f);
		m_Max.x = ptCenter.x + (fWidth/2.0f);
	}

	__inline void SetHeight(float fHeight) {
		CVector3D ptCenter(GetCenter());

		m_Min.y = ptCenter.y - (fHeight/2.0f);
		m_Max.y = ptCenter.y + (fHeight/2.0f);
	}

	__inline void SetDepth(float fDepth) {
		CVector3D ptCenter(GetCenter());

		m_Min.z = ptCenter.z - (fDepth/2.0f);
		m_Max.z = ptCenter.z + (fDepth/2.0f);
	}

	__inline bool IsEmptyBoundingBox() const {
		return (getHeight() == 0 || getWidth() ==0 || getDepth() == 0);
	}

private:
	__inline CVector3D _getCenter() const {
		return CVector3D(m_Min.x + getWidth()/2.0f, m_Min.y + getHeight()/2.0f, m_Min.z + getDepth()/2.0f);
	}

	__inline void _SetRotation(float nDegree) {
		m_fRotation = (nDegree < 0)? 360.0f+fmod(nDegree, 360.0f) : fmod(nDegree, 360.0f);
	}
};

