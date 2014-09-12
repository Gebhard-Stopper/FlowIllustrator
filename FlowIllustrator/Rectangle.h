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


class CRectangle :
	public CDrawingObject
{
private:
	CVector2D	m_rotationOrigin;
	bool		m_bUseRotationCenter;

public:
	CRectangle(const CRectF& BBox, const floatColor& color, bool bSolid = false);
	CRectangle(CDrawingObjectParams& params);

	virtual ~CRectangle(void);

public:
	virtual void Draw();
	virtual void GetParams(CDrawingObjectParams &params) const; 

	virtual CDrawingObject* Duplicate() const;

#ifdef WIN32
	virtual CString toString(float fLineWidthFactor) const;
#else
	virtual std::string toString() const;
#endif

	virtual __inline void SetRect(const CRectF& rect) { 
		CDrawingObject::SetRect(rect);
		_OnParamsChanged();
	}

	__inline void SetRotationOrigin(const CVector2D & ro) {
		m_rotationOrigin = ro;
		m_bUseRotationCenter = true;
	}

	__inline void ResetRotationOrigin() {
		m_bUseRotationCenter = false;
		CPointf c(_getCenter());
		m_rotationOrigin.x = c.x;
		m_rotationOrigin.y = c.y;
	}

	void SetWidth(float fWidth);
	void SetHeight(float fHeight);

protected:
	__inline CVector2D _getRotationCenter() const {
		if (m_bUseRotationCenter) 
			return m_rotationOrigin;
		
		CPointf c(_getCenter());
		return CVector2D(c.x, c.y);
	}

	
private:
	CPointf _getCenter() const;
	void _draw();
};

