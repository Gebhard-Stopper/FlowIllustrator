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
#include "Triangle.h"
#include "stipplepattern.h"

CTriangle::CTriangle(const CRectF &rect, const floatColor& color)
	:CDrawingObject(rect, DO_TRIANGLE, color)
{
	IsSolid(true);
	_initVertexBuffer();
	SetRect(rect);
}

CTriangle::CTriangle(const CDrawingObjectParams& params)
	: CDrawingObject( DO_TRIANGLE )
{
	_initVertexBuffer();
	SetParams(params);
}

void CTriangle::_initVertexBuffer()
{
	m_Vertices.clear();
	m_Vertices.reserve(3);

	for (int i=0; i<3; ++i) {
		m_Vertices.push_back(CPointf(0.0f,0.0f));
	}
}


CTriangle::~CTriangle(void)
{
	m_Vertices.clear();
}

void CTriangle::Draw()
{
	CDrawingObject::Draw();

	if (DrawStippled())
	{
		if (IsSolid()) {
			glPolygonStipple(pPattern2);
			glEnable(GL_POLYGON_STIPPLE);
			_draw();
			glDisable(GL_POLYGON_STIPPLE);

			//draw outline
			IsSolid(false);
			_draw();
			IsSolid(true);
		} else {
			glLineStipple(1, 0xAAAA); 
			glEnable(GL_LINE_STIPPLE);

			_draw();

			glDisable(GL_LINE_STIPPLE);
		}
	} else {
		_draw();
	}
}

void CTriangle::_draw()
{
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(2, GL_FLOAT, 0, &m_Vertices[0]);
	glDrawArrays( (IsSolid())? GL_TRIANGLES: GL_LINE_LOOP, 0, 3);
	glDisableClientState(GL_VERTEX_ARRAY);
}

void CTriangle::OnSetParams()
{
	_OnParamsChanged();
}

void CTriangle::GetParams(CDrawingObjectParams &params) const
{
	CDrawingObject::GetParams(params);

	CRectF rect = GetRect();

	params.SetValue( DOP_WIDTH, rect.getWidth() );
	params.SetValue( DOP_HEIGHT, rect.getHeight() );
	params.SetValue( DOP_CENTER, rect.GetCenter() );
}
 
void CTriangle::Translate(float tx, float ty)
{
	CDrawingObject::Translate(tx, ty);

	CPointf dummy(tx, ty);
	for (int i=0; i<3; i++) {
		m_Vertices[i] += dummy;
	}

	_OnParamsChanged();
}

CPointf CTriangle::_getCenter() const
{
	return CDrawingObject::GetCenter();
}

void CTriangle::_calcGlVertexPos()
{
	CRectF rect = GetRect();

	m_Vertices[0] = CPointf(rect.m_Min.x, rect.m_Min.y + rect.getHeight()/2.0f);
	m_Vertices[1] = CPointf(rect.m_Max.x, rect.m_Min.y);
	m_Vertices[2] = CPointf(rect.m_Max.x, rect.m_Max.y);

	CPointf center = _getCenter();
	CVector2D transVec(center.x, center.y);

	float rad = DegToRad(GetRotation());

	for (int i=0; i<3; i++) {
		m_Vertices[i] -= transVec;
		m_Vertices[i].Rotate(rad);
		m_Vertices[i] += transVec;
	}
}

void CTriangle::_OnParamsChanged() 
{
	_calcGlVertexPos();
}

#ifdef WIN32
CString CTriangle::toString(float fLineWidthFactor) const
{
	CString str;
	CString style	= getStyleString(fLineWidthFactor);

	CRectF rect = GetRect();
	rect.Normalize();

	CVector2D center = rect.GetCenter();

	str.Format(_T("<polygon\n\
					objectType=\"%d\"\n\
					style=%s\n\
					points=\"%f,%f %f,%f %f,%f\"\n\
					transform=\"rotate(%f,%f,%f)\"\n\
					hatched=\"%d\"\n\
					IsSolid=\"%d\"\n\
					id=\"%p\"\n\
					/>\n"),
		GetType(), style,
		rect.m_Min.x, rect.m_Min.y + rect.getHeight()/2.0f,
		rect.m_Max.x, rect.m_Min.y,
		rect.m_Max.x, rect.m_Max.y,
		GetRotation(), center.x, center.y, DrawStippled(),
		IsSolid(), this);

	return str;
}
#else
std::string CTriangle::toString() const
{
	std::string str;

	return str;
}
#endif

CDrawingObject* CTriangle::Duplicate() const
{
	CTriangle *pNewObj = new CTriangle(GetRect(), GetColor());

	pNewObj->CopyParams(this);

	return pNewObj;
}