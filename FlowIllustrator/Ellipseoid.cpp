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
#include "Ellipseoid.h"
#include "Vector2D.h"
#include "helper.h"
#include "stipplepattern.h"


CEllipsoid::CEllipsoid(float xPos, float yPos, float radius1, float radius2, const floatColor& color, bool bSolid)
	: CDrawingObject(CRectF(xPos-radius1, yPos-radius2, xPos+radius1, yPos+radius2), DO_ELLIPSE, color)
{
	_init(bSolid);
}

CEllipsoid::CEllipsoid(const CPointf& ptCenter, float radius1, float radius2, const floatColor& color, bool bSolid)
	: CDrawingObject(CRectF(ptCenter.x-radius1, ptCenter.y-radius2, ptCenter.x+radius1, ptCenter.y+radius2), DO_ELLIPSE, color)
{
	_init(bSolid);
}

CEllipsoid::CEllipsoid(const CRectF &rect, const floatColor& color, bool bSolid)
	: CDrawingObject(rect, DO_ELLIPSE, color)
{
	_init(bSolid);
}

CEllipsoid::CEllipsoid(DrawingObjectType type)
	: CDrawingObject(type)
{}

CEllipsoid::CEllipsoid(float xPos, float yPos, float radius1, float radius2, const floatColor& color, bool bSolid, DrawingObjectType type)
	: CDrawingObject(CRectF(xPos-radius1, yPos-radius2, xPos+radius1, yPos+radius2), type, color)
{
	_init(bSolid);
}

CEllipsoid::CEllipsoid(const CPointf& ptCenter, float radius1, float radius2, const floatColor& color, bool bSolid, DrawingObjectType type )
	: CDrawingObject(CRectF(ptCenter.x-radius1, ptCenter.y-radius2, ptCenter.x+radius1, ptCenter.y+radius2), type, color)
{
	_init(bSolid);
}

CEllipsoid::CEllipsoid(CDrawingObjectParams& params)
	:CDrawingObject ( static_cast<DrawingObjectType>(params.GetValueInt(DOP_TYPE)) )
{

	if (params.HasValue(DOP_CENTER_X) && params.HasValue(DOP_CENTER_Y)) {
		CSimpleVariant dummy1, dummy2;

		params.popValue(DOP_CENTER_X, dummy1);
		params.popValue(DOP_CENTER_Y, dummy2);

		SetCenter( CPointf(dummy1.GetFloatVal(), dummy2.GetFloatVal()) );
	}

	_init(params.GetValueInt(DOP_ISSOLID));
	SetParams(params);
}

void CEllipsoid::_init(bool bSolid)
{
	IsSolid(bSolid);
	m_pVertBuff = new GLfloat[2*static_cast<int>(NUM_ELLIPSE_POINTS+2)];
	_OnParamsChanged();
}

void CEllipsoid::_DestroyVertexBuffer()
{
	if (m_pVertBuff) {
		delete [] m_pVertBuff;
		m_pVertBuff = nullptr;
	}
}

CEllipsoid::~CEllipsoid(void)
{
	_DestroyVertexBuffer();
}

void CEllipsoid::Draw()
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

void CEllipsoid::_draw()
{
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(2, GL_FLOAT, 0, m_pVertBuff);

	if (IsSolid()) {
		glDrawArrays( GL_TRIANGLE_FAN, 0, m_nNumPoints);
	} else {
		glDrawArrays( GL_LINE_LOOP, 1, m_nNumPoints-1);
	}

	glDisableClientState(GL_VERTEX_ARRAY);
}

bool CEllipsoid::setParam(DrawinObjectParamName paramID, const CSimpleVariant &val)
{
	bool bResult(CDrawingObject::setParam(paramID, val));

	if (!bResult)
	{
		switch (paramID)
		{
			case DOP_RADIUS1:
				SetRadius1(val.GetFloatVal());
				bResult = true;
				break;
			case DOP_RADIUS2:
				SetRadius2(val.GetFloatVal());
				bResult = true;
				break;
		}
	}

	return bResult;
}

void CEllipsoid::GetParams(CDrawingObjectParams &params) const
{
	CDrawingObject::GetParams(params);

	params.SetValue(DOP_RADIUS1, GetRadius1());
	params.SetValue(DOP_RADIUS2, GetRadius2());
}

void CEllipsoid::SetRadius1(float radius)
{ 
	setWidth(2.0f * radius);
	_OnParamsChanged();
}

void CEllipsoid::SetRadius2(float radius) 
{ 
	setHeight(2.0f*radius);
	_OnParamsChanged();
}

void CEllipsoid::SetCenter(const CPointf &point)
{
	CDrawingObject::SetCenter(point);
	_OnParamsChanged();
}


void CEllipsoid::_calcGlVertexPos()
{
	float step = static_cast<float>(2.0*M_PI/NUM_ELLIPSE_POINTS);
	float rad = DegToRad(GetRotation());

	int nStart		= 0;
	m_nNumPoints	= static_cast<int>(NUM_ELLIPSE_POINTS);

	CPointf center = GetCenter();
	float r1 = GetRadius1();
	float r2 = GetRadius2();

	if (IsSolid())
	{
		nStart			= 1;
		m_nNumPoints	+= 2;
		reinterpret_cast<CPointf*>(m_pVertBuff)[0] = CVector2D(center.x, center.y);
	}

	//calculate points around origin and rotate them
	for (int i=nStart; i < m_nNumPoints; i++)
	{
		reinterpret_cast<CPointf*>(m_pVertBuff)[i] = center + CVector2D(	r1 * cos(i*step),
			r2 * sin(i*step)).Rotate(rad);
	}

	if (IsSolid())
	{
		reinterpret_cast<CPointf*>(m_pVertBuff)[m_nNumPoints-1] = reinterpret_cast<CPointf*>(m_pVertBuff)[1];
	}
}

void CEllipsoid::_OnParamsChanged()
{
	_calcGlVertexPos();
}

#ifdef WIN32

CString CEllipsoid::toString(float fLineWidthFactor) const
{
	CString str;
	CString style	= getStyleString(fLineWidthFactor);

	CRectF rect = GetRect();
	CVector2D center = rect.GetCenter();

	str.Format(_T("<ellipse\n\
					objectType=\"%d\"\n\
					style=%s\n\
					cx=\"%f\"\n\
					cy=\"%f\"\n\
					rx=\"%f\"\n\
					ry=\"%f\"\n\
					transform=\"rotate(%f,%f,%f)\"\n\
					hatched=\"%d\"\n\
					IsSolid=\"%d\"\n\
					id=\"%p\"\n\
					/>\n"),
		GetType(), style, center.x, center.y, GetRadius1(), GetRadius2(),
		GetRotation(), center.x, center.y, DrawStippled(), IsSolid(),
		this);

	return str;
}

void CEllipsoid::OnToString(CString &str, float /*fLineWidthFactor*/) const
{
	str += _T("points=\"");
	CString dummy;
	for (int i=0; i < m_nNumPoints; i+=2) {
		dummy.Format(_T("%f,%f "), m_pVertBuff[i], m_pVertBuff[i+1]);
		str += dummy;
	}
	str += "\"\n";
}
#else

#endif

CDrawingObject* CEllipsoid::Duplicate() const
{
	CEllipsoid *pNewObj = new CEllipsoid( GetRect(), GetColor(), IsSolid());

	pNewObj->CopyParams(this);

	return pNewObj;
}