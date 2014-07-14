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
#include "Rectangle.h"
#include "stipplepattern.h"
#include "PolyLine.h"

CRectangle::CRectangle(const CRectF& BBox, const floatColor& color, bool bSolid)
	: CDrawingObject(BBox, DO_RECTANGLE, color)
{
	IsSolid(bSolid);

	CPointf center			= _getCenter();
	m_rotationOrigin		= CVector2D(center.x, center.y);
	m_bUseRotationCenter	= false;

	_OnParamsChanged();
}


CRectangle::~CRectangle(void)
{
}

void CRectangle::Draw()
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

void CRectangle::_draw()
{
	CRectF rect(GetRect());

	CVector2D corners[4] = {	CVector2D(rect.m_Min.x, rect.m_Min.y),
								CVector2D(rect.m_Min.x, rect.m_Max.y), 
								CVector2D(rect.m_Max.x, rect.m_Max.y),
								CVector2D(rect.m_Max.x, rect.m_Min.y) };

	CVector2D translationVec = _getRotationCenter();
	float rad = DegToRad(GetRotation());

	for (int i=0; i<4; i++) {
		corners[i] -= translationVec;
		corners[i].Rotate(rad);
		corners[i] += translationVec;
	}

	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(2, GL_FLOAT, 0, &corners);
	glDrawArrays(IsSolid()? GL_QUADS : GL_LINE_LOOP, 0, 4);
	glDisableClientState(GL_VERTEX_ARRAY);	
}

bool CRectangle::setParam(DrawinObjectParamName paramID, const CSimpleVariant &val)
{
	bool bResult(CDrawingObject::setParam(paramID, val));

	if (!bResult)
	{
		switch (paramID)
		{
			/*case DOP_WIDTH:
				SetWidth( val.GetFloatVal() );
				bResult = true;
				break;
			case DOP_HEIGHT:
				SetHeight( val.GetFloatVal() );
				bResult = true;
				break;
			case DOP_CENTER:
				SetCenter( val.GetPointfVal() );
				bResult = true;
				break;*/
		}
	}

	return bResult;
}

void CRectangle::GetParams(CDrawingObjectParams &params) const
{
	CDrawingObject::GetParams(params);

	/*CRectF rect = GetRect();

	params.SetValue( DOP_WIDTH, rect.getWidth() );
	params.SetValue( DOP_HEIGHT, rect.getHeight() );
	params.SetValue( DOP_CENTER, rect.GetCenter() );*/
}

CPointf CRectangle::_getCenter() const
{
	CRectF rect = GetRect();
	return CPointf(rect.getWidth()/2.0f + rect.m_Min.x, rect.getHeight()/2.0f + rect.m_Min.y);
}
	
#ifdef WIN32
CString CRectangle::toString(float fLineWidthFactor) const
{
	CString str;
	CString style	= getStyleString(fLineWidthFactor);
	CRectF rect		= GetRect();
	CVector2D center = rect.GetCenter();

	str.Format(_T("<rect\n\
				  objectType=\"%d\"\n\
				  style=%s\n\
				  x=\"%f\"\n\
				  y=\"%f\"\n\
				  width=\"%f\"\n\
				  height=\"%f\"\n\
				  transform=\"rotate(%f,%f,%f)\"\n\
				  hatched=\"%d\"\n\
				  IsSolid=\"%d\"\n\
				  id=\"%p\"\n\
				  />\n"),
		GetType(), style,
		rect.m_Min.x, rect.m_Min.y,
		rect.getWidth(), rect.getHeight(),
		GetRotation(), center.x, center.y,
		DrawStippled(), IsSolid(), this);

	return str;
}
#else
std::string CRectangle::toString() const
{
	char buffer [2048];
	char strStyle [512];

	CRectF rect = GetRect();
	LPFLOATCOLOR pColor = GetColor();

	if (m_bIsSolid) {
		sprintf(strStyle, 
			"\"fill:#%x%x%x;stroke:none\"",
			static_cast<int>(pColor->red*255),
			static_cast<int>(pColor->green*255),
			static_cast<int>(pColor->blue*255));
	} else {
		sprintf(strStyle, 
			"\"fill:none;stroke:#%x%x%x;stroke-width:%f\"",
			static_cast<int>(pColor->red*255),
			static_cast<int>(pColor->green*255),
			static_cast<int>(pColor->blue*255),
			GetThickness());
	}

	sprintf(buffer,
		"<rect\nstyle=\"%s\"\nx=\"%f\"\ny=\"%f\"\nwidth=\"%f\"\nheight=\"%f\"/>",
		strStyle,
		rect.m_Min.x, rect.m_Min.y,
		rect.getWidth(), rect.getHeight());
	
	
	return std::string(buffer);
}
#endif

void CRectangle::SetWidth(float fWidth)
{
	CRectF rect = GetRect();
	rect.SetWidth(fWidth);

	SetRect(rect);
}

void CRectangle::SetHeight(float fHeight)
{
	CRectF rect = GetRect();
	rect.SetHeight(fHeight);

	SetRect(rect);
}

/*void CRectangle::SetCenter(const CPointf &center)
{
	CRectF rect = GetRect();
	rect.SetCenter(CVector2D(center.x, center.y));

	SetRect(rect);
}*/

CDrawingObject* CRectangle::Duplicate() const
{
	CRectangle *pNewObj = new CRectangle(GetRect(), GetColor(), IsSolid());

	pNewObj->CopyParams(this);

	return pNewObj;
}