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
#include "DrawingObject.h"
#include "helper.h"


CDrawingObject::CDrawingObject(const CRectF& BBox, DrawingObjectType type, const floatColor& color)
	:  m_pParams(new CDrawingObjectParams), m_bSelected(false), m_pParent(nullptr)
{
	_init(type);

	SetRect(BBox);
	SetColor(color);
}

CDrawingObject::CDrawingObject(DrawingObjectType type)
	: m_pParams(new CDrawingObjectParams), m_pParent(nullptr)
{
	_init(type);
	SetColor(floatColor(0.0f, 0.0f, 0.0f));
}

void CDrawingObject::_init(DrawingObjectType type)
{
	SetType(type);
	SetRotation(0);
	IsSolid(true);
	DrawHalo(true);
	DrawStippled(false);
	SetThickness(1.5f);
	SetHaloColor(floatColor(1.0f, 1.0f, 1.0f));
}

CDrawingObject::~CDrawingObject(void)
{
}

void CDrawingObject::Draw()
{
	glLineWidth(GetThickness());
	glColor4fv(GetColorGL());
}

bool CDrawingObject::SetParams(const CDrawingObjectParams &params)
{
	CSimpleVariant buffer;
	if ( !params.GetValue(DOP_TYPE, buffer) ) return false;
	
	if (GetType() != static_cast<DrawingObjectType>(buffer.iVal)) {
		SetChildObjParams(params);
		return false;
	}

	for (auto iter = params.dataMap.begin(); iter != params.dataMap.end(); ++iter)
	{
		setParam( iter->first, iter->second );
	}

	OnSetParams();

	return true;
}

bool CDrawingObject::setParam(DrawinObjectParamName paramID, const CSimpleVariant &val)
{
	switch (paramID)
	{
		case DOP_COLOR:
			SetColor( val.GetColorVal() );
			return true;
		case DOP_HALOCOLOR:
			SetHaloColor( val.GetColorVal() );
			return true;
		case DOP_THICKNESS:
			SetThickness( val.GetFloatVal() );
			return true;
		case DOP_ROTATION:
			SetRotation( val.GetFloatVal() );
			return true;
		case DOP_ISSOLID:
			IsSolid( val.GetBoolVal() );
			return true;
		case DOP_HATCHED:
			DrawStippled( val.GetBoolVal() );
			return true;
		case DOP_WIDTH:
			setWidth(val.GetFloatVal());
			return true;
		case DOP_HEIGHT:
			setHeight(val.GetFloatVal());
			return true;
		case DOP_CENTER:
			SetCenter(val.GetPointfVal());
			return true;
	}

	return false;
}

void CDrawingObject::OnSetParams()
{
}

bool CDrawingObject::SetChildObjParams(const CDrawingObjectParams &params)
{
	if (m_children.empty()) {
		return false;
	}

	bool bResult (true);

	for (auto iter = m_children.begin(); iter != m_children.end(); ++iter) {
		if ( !(*iter)->SetParams(params))
			bResult = false;
	}

	return bResult;
}

void CDrawingObject::GetParams(CDrawingObjectParams &params) const
{
	params.Clear();
	params = *m_pParams;
}

CDrawingObjectParams CDrawingObject::GetParams() const
{
	CDrawingObjectParams params;
	GetParams(params);
	return params;
}

#ifdef WIN32
CString CDrawingObject::getStyleString(float fLineWidthFactor) const
{
	CString style;
	const floatColor& color = GetColor();

	if (IsSolid() && !DrawStippled()) {
		style.Format(_T("\"fill:%s;stroke:none\""),
			color.GetHexString());
	} else {
		style.Format(_T("\"fill:none;stroke:%s;stroke-width:%f\""),
			color.GetHexString(), GetThickness()/fLineWidthFactor);
	}

	return style;
}
#endif

CString CDrawingObject::toString(float fLineWidthFactor) const
{
	CString strRetVal, strDummy;

	strRetVal.Format(_T("<%s\n"), _T("PolyLine")); //Opening tag for SVG
	
	strDummy.Format( _T("style=%s\n"), getStyleString(fLineWidthFactor) );
	strRetVal += strDummy;

	//Loop over all parameters an add them to the output string
	for ( auto iter = m_pParams->dataMap.begin();iter != m_pParams->dataMap.end(); ++iter )
	{
		strDummy.Format(_T("%s=\"%s\"\n"), DrawingObjNames.GetParamName((*iter).first), (*iter).second.GetString());
		strRetVal += strDummy;
	}

	//if this object has a parent, then write it to the output string.
	if (m_pParent) {
		strDummy.Format(_T("parent=\"%p\"\n"), m_pParent);
		strRetVal += strDummy;
	}

	strDummy.Format(_T("id=\"%p\"\n"), this); // this-pointer serves as uinque ID in file
	strRetVal += strDummy;

	OnToString(strRetVal, fLineWidthFactor); //call user-definable handler

	strRetVal += _T("/>\n");	//Closing tag for SVG

	FinalizeToString(strRetVal, fLineWidthFactor);

	return strRetVal;
}

void CDrawingObject::OnToString(CString& /*str*/, float /*fLineWidthFactor*/) const
{
}

void CDrawingObject::FinalizeToString(CString& str, float fLineWidthFactor) const
{
	for (auto iter = m_children.begin(); iter != m_children.end(); ++iter) {
		str += (*iter)->toString(fLineWidthFactor);
	}
}

void CDrawingObject::CopyParams(const CDrawingObject *pSrc)
{
	CDrawingObjectParams params;
	pSrc->GetParams(params);
	SetParams(params);
}

void CDrawingObject::SetCenter(const CPointf &point)
{
	m_pParams->SetValue(DOP_CENTER, point);
	/*CPointf center = GetCenter();

	float distX = point.x - center.x;
	float distY = point.y - center.y;

	Translate(distX, distY);*/
}