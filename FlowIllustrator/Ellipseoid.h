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
#include "Pointf.h"

#define NUM_ELLIPSE_POINTS 70.0f

/**
 *	This class is used to represent ellipses and provides an appropriate drawin mechanism.
 *	An ellipse is primarily defined by two perpendicular radii and a central point.
 */
class CEllipsoid :
	public CDrawingObject
{
private:
	int		m_nNumPoints;		/**< Number of vertices to represent the ellipse */

private:
	GLfloat *m_pVertBuff;		/**< Vertex buffer to hold the vertices defining the circumcircle of the ellipse */

public:
	/**
	 *	Create a new CEllipsoid object.
	 *
	 *	@param xPos X-component of the center.
	 *	@param yPos Y-component of the center.
	 *	@param radius1 Radius along the horizontal axis of the ellipse.
	 *	@param radius2 Radius along the vertical axis of the ellipse.
	 *	@param color Pointer to a floatColor, defining the ellipses main color.
	 *	@param bSolid Flag, which indicates if the ellipse is to be drawn solid or as contour.
	 */
	CEllipsoid(float xPos, float yPos, float radius1, float radius2, const floatColor& color, bool bSolid = true);

	/**
	 *	Create a new CEllipsoid object.
	 *
	 *	@param ptCenter Center point of the ellipse.
	 *	@param radius1 Radius along the horizontal axis of the ellipse.
	 *	@param radius2 Radius along the vertical axis of the ellipse.
	 *	@param color Pointer to a floatColor, defining the ellipses main color.
	 *	@param bSolid Flag, which indicates if the ellipse is to be drawn solid or as contour.
	 */
	CEllipsoid(const CPointf& ptCenter, float radius1, float radius2, const floatColor& color, bool bSolid = true);

	/**
	 *	Create a new CEllipsoid object.
	 *
	 *	@param rect Bounding box of the ellipse. The width and height of the CRectF are use as the ellipses horizointal and vertical radius, respectively.
	 *	@param color Pointer to a floatColor, defining the ellipses main color.
	 *	@param bSolid Flag, which indicates if the ellipse is to be drawn solid or as contour.
	 */
	CEllipsoid(const CRectF &rect, const floatColor& color, bool bSolid = true);

	/**
	 *	Destroy the ellipse and all of its data.
	 */
	virtual ~CEllipsoid(void);

protected:
	CEllipsoid(float xPos, float yPos, float radius1, float radius2, const floatColor& color, bool bSolid, DrawingObjectType type );
	CEllipsoid(const CPointf& ptCenter, float radius1, float radius2, const floatColor& color, bool bSolid, DrawingObjectType type );
	CEllipsoid(DrawingObjectType type);

public:
	/**
	 *	Draw the ellipse.
	 */
	virtual void Draw();

	/**
	 *	Create a deep copy of the ellipse.
	 *
	 *	@return A new CEllipsoid, identical to this CEllipsoid.
	 *
	 *	@remarks The user has to delete the returned CEllipsoid object, if no longer needed.
	 */
	virtual CDrawingObject* Duplicate() const;

protected:
	virtual void GetParams(CDrawingObjectParams &params) const; 

	/**
	 *	Set one or more parameters to a CEllipsoid.
	 *
	 *	@param	params A reference to a CDrawingObjectParams object that holds the parameters to be set.
	 *
	 *	@see CDrawingObject
	 */
	virtual bool setParam(DrawinObjectParamName paramID, const CSimpleVariant &val);

#ifdef WIN32
	/**
	 *	Retrieve a textual representation of this CEllipsoid.
	 *	By default, this produces a SVG compatible representation of this object.
	 *
	 *	@param fLineWidthFactor Factor to downscale or upscale object sizes, to match the dimensions of the output format.
	 *
	 *	@return Textual representation of this object as CString.
	 *
	 *	@remarks This function internally calls GetParams() to retrieve all parameters of a CDrawingObject.
	 *			 Only the parameters obtained by GetParams() are appended to the output string by default.
	 */
	virtual CString toString(float fLineWidthFactor) const;

protected:
	virtual void OnToString(CString &str, float fLineWidthFactor) const;
#else

#endif

public:
	/**
	 *	Retrieve the horizontal radius of this CEllipsoid.
	 *
	 *	@return The radius as float.
	 */
	__inline float GetRadius1() const {
		return GetWidth()/2.0f;
	} 

	/**
	 *	Retrieve the vertical radius of this CEllipsoid.
	 *
	 *	@return The radius as float.
	 */
	__inline float GetRadius2() const {
		return GetHeight()/2.0f;
	} 

	/**
	 *	Set the horizontal radius of this CEllipsoid.
	 *
	 *	@param radius The radius as float.
	 */
	void SetRadius1(float radius);

	/**
	 *	Set the vertical radius of this CEllipsoid.
	 *
	 *	@param radius The radius as float.
	 */
	void SetRadius2(float radius);

private:
	void _calcGlVertexPos();
	void _init(bool bSolid);
	void _draw();

protected:
	virtual void _OnParamsChanged();
	virtual void _DestroyVertexBuffer();

friend class CVortexObj;
};

