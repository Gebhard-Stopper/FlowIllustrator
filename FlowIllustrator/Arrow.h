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
	CArrow(CPointf ptStart, CVector2D dir, float fLength, const floatColor& color);
	virtual ~CArrow(void);

protected:
	CPointf		m_ptStart;
	CVector2D	m_vecDir;
	float		m_fLength;

public:
	virtual void Draw();
	virtual void Translate(float tx, float ty);
	virtual CDrawingObject* Duplicate() const;

#ifdef WIN32
	virtual CString toString(float fLineWidthFactor) const;
#endif

private:
	void _getVertices(std::vector<CVector2D> &points) const;

protected:
	virtual bool setParam(DrawinObjectParamName paramID, const CSimpleVariant &val);
	virtual void _OnParamsChanged() {}
};

