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

using namespace std;

class CTriangle :
	public CDrawingObject
{
public:
	CTriangle(const CRectF &rect, const floatColor& color);
	CTriangle(const CDrawingObjectParams& params);

	virtual ~CTriangle(void);

private:
	vector<CPointf> m_Vertices;	//Vertex buffer object

public:
	virtual void Draw();
	virtual void Translate(float tx, float ty);
	virtual CDrawingObject* Duplicate() const;

#ifdef WIN32
	virtual CString toString(float fLineWidthFactor) const;
#else
	virtual std::string toString() const;
#endif


	virtual __inline void SetRect(const CRectF& rect) { 
		CDrawingObject::SetRect(rect);

		m_Vertices[0] = CPointf(rect.m_Min.x, rect.m_Min.y + rect.getHeight()/2.0f);
		m_Vertices[1] = CPointf(rect.m_Max.x, rect.m_Min.y);
		m_Vertices[2] = CPointf(rect.m_Max.x, rect.m_Max.y);

		_OnParamsChanged();
	}

	vector<CPointf>* GetDataPoints() {
		return& m_Vertices;
	}

protected:
	virtual void GetParams(CDrawingObjectParams &params) const; 
	virtual void OnSetParams();

private:
	CPointf _getCenter() const;
	void _calcGlVertexPos();
	void _draw();
	void _initVertexBuffer();

protected:
	virtual void _OnParamsChanged();
};

