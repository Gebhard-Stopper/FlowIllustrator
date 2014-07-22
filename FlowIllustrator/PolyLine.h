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

const DWORD SL_TRANSPARENCY_CONST		= 0x000;		/**< Constant transparency along the speed line. */
const DWORD SL_TRANSPARENCY_INCREASE	= 0x001;		/**< Transparency increases as speed line reaches further back in time. */
const DWORD SL_TRANSPARENCY_DECREASE	= 0x002;		/**< Transparency decreases as speed line reaches further back in time. */
const DWORD SL_THICKNESS_CONST			= 0x000;		/**< Constant thickness along the speed line. */
const DWORD SL_THICKNESS_INCREASE		= 0x010;		/**< Thickness increases as speed line reaches further back in time. */
const DWORD SL_THICKNESS_DECREASE		= 0x020;		/**< Thickness decreases as speed line reaches further back in time. */
const DWORD SL_DASHED					= 0x040;		/**< Lines are drawn dashed, s.t. half of a line is solid, and the other half is transparent. */
const DWORD SL_ARROW					= 0x080;		/**< The line is drawn as arrow, that bend along the curse of the line. */
const DWORD	SL_TRIANGLE					= 0x100;		/**< The line is drawn similar to SL_DASHED, just using triangular structures, instead of rectangular ones. */ 
const DWORD SL_DROPLET					= 0x200;		/**< The line is drawn consisting of droplets. */
const DWORD SL_ELLIPSE					= 0x400;		/**< The line is drawn consisting of elliptic shapes. */
/**
 *	CPolyLine is the base class of all line-type structures in FlowIllustrator.
 *	It provides abilities to render CPolyLine derived classes with increasing or decreasing transparency and thickness,
 *	starting from the first vertex up to a user definable end vertex.
 */
class CPolyLine :
	public CDrawingObject
{
protected:
	vector<CPointf> m_Points;		/**< std::vector holding all the vertices of a polyline. */
	vector<CVector2D> m_Normals;	/**< std::vector holding the normal vectors, corresponding to the vertices of a polyline. */

public:
	/**
	 *	Create a new CpolyLine object
	 *
	 *	@param color The main color of the new poly line.
	 *  @param fThickness The thickness of the poly line in domain space.
	 */
	CPolyLine(const floatColor& color, float fThickness);

	/**
	 *	Destroy this CPolyLine object and all of its associated data.
	 */
	virtual ~CPolyLine(void);

protected:
	CPolyLine(const CRectF& BBox, const floatColor& color, float fThickness);
	CPolyLine(const CRectF& BBox, const floatColor& color, DrawingObjectType nType);
	CPolyLine(const floatColor& color, float fThickness, DrawingObjectType nType);
	void Init(float fThickness);

public:
#ifdef WIN32
	/**
	 *	@see CDrawingObject::OnToString()
	 */
	virtual void OnToString(CString &str, float fLineWidthFactor) const;
#endif

protected:
	virtual void _OnParamsChanged();

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

	/**
	 *	@see CDrawingObject::OnSetParams()
	 */
	virtual void OnSetParams();

	/**
	 *	Copies the vertices from the m_Points member of the CPolyLine supplied in the argument into the m_Points member of this CPolyLine.
	 *
	 *	@param pSrc Pointer to a CPolyLine whose vertices are to be copied.
	 */
	void CopyVertices(const CPolyLine *pSrc);

public:
	/**
	 *	Draws this CPolyLine objec to the screen.
	 *
	 *	@param bDrawAsDroplet If true, this CPolyLine object is draw as droplet(s).
	 *	@param nCutoff Number of vertices ignored at the beginning and end of the CPolyLIne.
	 *
	 *	@remarks	If bDrawAsDroplet is set to true, this CPolyLine is drawn with as many droplets, as specified in the
	 *				latest call to SetDropletCount(). Default is one droplet.
	 */
	virtual void Draw(bool bDrawAsDroplet, int nCutoff=0);

	/**
	 *	Draws this CPolyLine objec to the screen.
	 *	By default, this object is drawn as a line.
	 */
	virtual void Draw() { Draw(false); }

	/**
	 *	@see CDrawingObject::Translate()
	 */
	virtual void Translate(float tx, float ty);

	/**
	 *	Returns a deep copy of this CPolyLine, including all parameters and vertices.
	 *
	 *	@return A pointer to a new CPolyLine object.
	 */
	virtual CDrawingObject* Duplicate() const;

	/**
	 *	Returns the number of vertices of this CPolyLine.
	 *
	 *	@return The number of vertices as size_t.
	 */
	__inline size_t Size() { return m_Points.size(); }

	/**
	 *	Calculates the bounding rectangle of this CPolyLine.
	 *	The bounding rect can tehn be retrieven by calling the GetRect() member function.
	 */
	void CalcBoundingRect();

	/**
	 *	Smoothes the poly line by applying a box filter, using the smoothness supplied in the function argument as 
	 *	kernel half-size.
	 *
	 *	@param nSmoothness Kernel half-size of the applied bolx filter.
	 */
	virtual void Smooth(int nSmoothness = 3);

	/**
	 *	Returns a pointer to the m_Points member of this CPolyLine object. This allows to directly modify this CPolyLine's vertex data.
	 *
	 *	@return A pointer to a std::vector, pointing to m_Points.
	 *
	 *	@remarks After modifying the vertices of a CPolyLine, the user must ensure the corresponding normals are also updated.
	 */
	vector<CPointf>* GetDataPoints();

	/** 
	 *	Set the number of droplets used to render this CPolyLine.
	 *
	 *	@param numDroplets The number of droplets as int.
	 *
	 *	@remarks This only has an effect, if this CPolyLine is rendered as droplets.
	 */
	__inline void SetDropletCount(int numDroplets) {
		if (numDroplets < 1) numDroplets = 1;
		m_pParams->SetValue(DOP_NUMDROPLETS, numDroplets);
	}

	/**
	 *	Retrieve the number of droplets used to render this CPolyLine.
	 *
	 *	@return The droplet count as int.
	 */
	__inline int GetDropletCount() const {
		return m_pParams->GetValueInt(DOP_NUMDROPLETS);
	}

	/**
	 *	Calculates the vertex normals and stores them in the m_Normals member of this CPolyLine.
	 *	
	 *	@remakrs The normal vectors are are stored two times, in positive and negative normal direction.
	 */
	void CalcVertexNormals();

		/**
	 *	Set the drawing style of this CSpeedLine object.
	 *
	 *	@param dwStyle The new Style. The style may be a combination of one of each of the following transparency and thickness flags:
	 *  <TABLE>
	 *	<TR><TH>Value</TH><TH>Description</TH></TR>
	 *	<TR><TD>SL_TRANSPARENCY_CONST</TD>		<TD>Constant transparency along the speed line.</TD></TR>
	 *	<TR><TD>SL_TRANSPARENCY_INCREASE</TD>	<TD>Transparency increases as speed line reaches further back in time. </TD></TR>
	 *	<TR><TD>SL_TRANSPARENCY_DECREASE</TD>	<TD>Transparency decreases as speed line reaches further back in time. </TD></TR>
	 *	<TR><TD>SL_THICKNESS_CONST</TD>			<TD>Constant thickness along the speed line. </TD></TR>
	 *	<TR><TD>SL_THICKNESS_INCREASE</TD>		<TD>Thickness increases as speed line reaches further back in time. </TD></TR>
	 *	<TR><TD>SL_THICKNESS_DECREASE</TD>		<TD>Thickness decreases as speed line reaches further back in time. </TD></TR>
	 *	</TABLE>
	 */
	__inline void SetStyle(DWORD dwStyle) {
		m_pParams->SetValue(DOP_SPEEDLINE_STYLE, static_cast<unsigned int>(dwStyle)); 
	}

	/**
	 *	Retrieves the Style of this CPolyLine.
	 *
	 *	@return The style as DWORD.
	 *
	 *	@see SetStyle() for details.
	 */
	__inline DWORD GetStyle() const {
		return static_cast<DWORD>(m_pParams->GetValueUInt(DOP_SPEEDLINE_STYLE));
	}

	/**
	 *	Set the minimum thickness of this CPolyLine.
	 *
	 *	@param fThickness The minumum thickness as float, in domain space.
	 *
	 *	@remarks The minimum thickness is onyl used, if this poly line has the SL_THICKNESS_INCREASE or SL_THICKNESS_DECREASE style.
	 */
	__inline void SetThicknessMin(float fThickness) {
		m_pParams->SetValue(DOP_THICKNESS_MIN, fThickness);
	}

	/**
	 *	Retrieve the minimum thickness of this CPolyLine.
	 *
	 *	@return The minumum thickness as float, in domain space.
	 *
	 *	@remarks The minimum thickness is onyl used, if this poly line has the SL_THICKNESS_INCREASE or SL_THICKNESS_DECREASE style.
	 */
	__inline float GetThicknessMin() const {
		return m_pParams->GetValueFloat(DOP_THICKNESS_MIN);
	}

	/**
	 *	Retrieve the minimum and maximum alpha transparency values of this CSpeedLine.
	 *
	 *	@param fAlpha Reference to a float to receive the maximum alpha value.
	 *	@param fAlphaMin Reference to a float to receive the minimum alpha value.
	 */
	__inline void GetAlpha(float &fAlpha, float &fAlphaMin) const { 
		fAlpha = GetAlphaMax();
		fAlphaMin = GetAlphaMin();
	}

	/**
	 *	Set the minimum and maximum alpha transparency values of this CSpeedLine.
	 *
	 *	@param fAlpha A float, specifying the new maximum alpah value.
	 *	@param fAlphaMin A float, specifying the new minimum alpah value.
	 */
	__inline void SetAlpha(float fAlpha, float fAlphaMin) { 
		SetAlphaMax(fAlpha);
		SetAlphaMin(fAlphaMin);
	}

	/**
	 *	Retrieve the minimum alpah transparency value used to render this CPolyLine.
	 *
	 *	@return The minumum alpha value as float.
	 */
	__inline float GetAlphaMin() const {
		return m_pParams->GetValueFloat(DOP_ALPHA_MIN);
	}

	/**
	 *	Retrieve the maximum alpah transparency value used to render this CPolyLine.
	 *
	 *	@return The maximum alpha value as float.
	 */
	__inline float GetAlphaMax() const {
		return m_pParams->GetValueFloat(DOP_ALPHA);
	}

	/**
	 *	Set the minumum alpah transparency value used to render this CPolyLine.
	 *
	 *	@param fAlpha The minumum alpha value as float. Must be in range [0 .. 1].
	 *
	 *	@remarks This value is only used, if this poly line has either the SL_TRANSPARENCY_INCREASE or SL_TRANSPARENCY_DECREASE style.
	 */
	__inline void SetAlphaMin(float fAlpha) {
		m_pParams->SetValue(DOP_ALPHA_MIN, fAlpha);
	}


	/**
	 *	Set the maximum alpah transparency value used to render this CPolyLine.
	 *
	 *	@param fAlpha The maximum alpha value as float. Must be in range [0 .. 1].
	 *
	 *	@remarks This value is only used, if this poly line has either the SL_TRANSPARENCY_INCREASE or SL_TRANSPARENCY_DECREASE style.
	 */
	__inline void SetAlphaMax(float fAlpha) {
		m_pParams->SetValue(DOP_ALPHA, fAlpha);
	}

	/**
	 *	Set the number of steps (vertices) used for increasing/decreasing the thickness of this CPolyLine.
	 *
	 *	@param nSteps The number of steps as int.
	 */
	__inline void SetNumGrowSteps(int nSteps) {
		m_pParams->SetValue(DOP_GROW_STEPS, nSteps);
	}

	/**
	 *	Retrieve the number of steps (vertices) used for increasing/decreasing the thickness of this CPolyLine.
	 *
	 *	@return The number of steps as int.
	 */
	__inline int GetNumGrowSteps() const {
		return m_pParams->GetValueInt(DOP_GROW_STEPS);
	}

	/**
	 *	Set the number of steps (vertices) used for increasing/decreasing the transparency of this CPolyLine.
	 *
	 *	@param nSteps The number of steps as int.
	 */
	__inline void SetNumTransparentSteps(int nSteps) {
		m_pParams->SetValue(DOP_TRANSPARENT_STEPS, nSteps);
	}


	/**
	 *	Retrieve the number of steps (vertices) used for increasing/decreasing the transparency of this CPolyLine.
	 *
	 *	@param nSteps The number of steps as int.
	 */
	__inline int GetNumTransparentSteps() const {
		return m_pParams->GetValueInt(DOP_TRANSPARENT_STEPS);
	}

private:
	/**
	 *	Draws this CPolyLine, ignoring the first and last nCutoff vertices.
	 */
	void _draw(int nCutoff);

	CPointf _interpolateVertex(int nIndx, int nVertexCount);
	float _calcThickness(int nIndx, int nVertexCount, DWORD dwThicknessFunc);

public:
	friend class CStreamLine;
	friend class CSpeedLine;
};

