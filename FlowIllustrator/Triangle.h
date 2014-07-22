/*
 *	Copyright (C) 2014, Max Planck Institut f�r Informatik, Saarbr�cken.
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



class CTriangle :
	public CDrawingObject
{
public:
	CTriangle(const CRectF &rect, const floatColor& color);
	virtual ~CTriangle(void);

private:
	GLfloat *m_pVertBuff;	//Vertex buffer object

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

		reinterpret_cast<CVector2D*>(m_pVertBuff)[0] = CVector2D(rect.m_Min.x, rect.m_Min.y + rect.getHeight()/2.0f);
		reinterpret_cast<CVector2D*>(m_pVertBuff)[1] = CVector2D(rect.m_Max.x, rect.m_Min.y);
		reinterpret_cast<CVector2D*>(m_pVertBuff)[2] = CVector2D(rect.m_Max.x, rect.m_Max.y);

		_OnParamsChanged();
	}

protected:
	virtual void GetParams(CDrawingObjectParams &params) const; 

	/**
	 *	Set one or more parameters to a CDrawingObject.
	 *
	 *	@param	paramID The unique DrawinObjectParamName of this parameter.
	 *	@param	val Reference to a CSimpleVariant, representing the value of the parameter.
	 *	
	 *	@remarks	This function must be implemented in derived classes to ensure that all parameters are
	 *				set correctly.
	 *
	 *	@see CDrawingObject::setParam()
	 */
	virtual bool setParam(DrawinObjectParamName paramID, const CSimpleVariant &val);
	virtual void OnSetParams();

private:
	CPointf _getCenter() const;
	void _calcGlVertexPos();
	void _draw();

protected:
	virtual void _OnParamsChanged();
};

