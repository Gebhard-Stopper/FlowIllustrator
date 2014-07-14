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

const DWORD SL_TRANSPARENCY_CONST		= 0x00;		/**< Constant transparency along the speed line. */
const DWORD SL_TRANSPARENCY_INCREASE	= 0x01;		/**< Transparency increases as speed line reaches further back in time. */
const DWORD SL_TRANSPARENCY_DECREASE	= 0x03;		/**< Transparency decreases as speed line reaches further back in time. */
const DWORD SL_THICKNESS_CONST			= 0x00;		/**< Constant thickness along the speed line. */
const DWORD SL_THICKNESS_INCREASE		= 0x10;		/**< Thickness increases as speed line reaches further back in time. */
const DWORD SL_THICKNESS_DECREASE		= 0x30;		/**< Thickness decreases as speed line reaches further back in time. */

class CPolyLine :
	public CDrawingObject
{
protected:
	vector<CPointf> m_Points;	/**< Vector holding all the vertices of a polyline. */
	vector<CVector2D> m_Gradients;

public:
	CPolyLine(const CRectF& BBox, const floatColor& color, float fThickness);
	CPolyLine(const floatColor& color, float fThickness);

	virtual ~CPolyLine(void);

protected:
	CPolyLine(const CRectF& BBox, const floatColor& color, DrawingObjectType nType);
	CPolyLine(const floatColor& color, float fThickness, DrawingObjectType nType);

	void Init(float fThickness);

public:
#ifdef WIN32
	virtual void OnToString(CString &str, float fLineWidthFactor) const;
#endif

protected:
	virtual void _OnParamsChanged();
	virtual bool setParam(DrawinObjectParamName paramID, const CSimpleVariant &val);
	virtual void OnSetParams();
	void CopyVertices(const CPolyLine *pSrc);

public:
	virtual void Draw(bool bDrawAsDroplet, int nCutoff=0);
	virtual void Draw() { Draw(false); }
	virtual void Translate(float tx, float ty);
	virtual CDrawingObject* Duplicate() const;

	__inline size_t Size() { return m_Points.size(); }

	void CalcBoundingRect();
	virtual void Smooth(int nSmoothness = 3);
	vector<CPointf>* GetDataPoints();

	__inline void SetDropletCount(int numDroplets) {
		if (numDroplets < 1) numDroplets = 1;
		m_pParams->SetValue(DOP_NUMDROPLETS, numDroplets);
	}

	__inline int GetDropletCount() const {
		return m_pParams->GetValueInt(DOP_NUMDROPLETS);
	}

	void CalcVertexGradients();

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

	__inline DWORD GetStyle() const {
		return static_cast<DWORD>(m_pParams->GetValueUInt(DOP_SPEEDLINE_STYLE));
	}

	__inline void SetThicknessMin(float fThickness) {
		m_pParams->SetValue(DOP_THICKNESS_MIN, fThickness);
	}

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

	__inline float GetAlphaMin() const {
		return m_pParams->GetValueFloat(DOP_ALPHA_MIN);
	}

	__inline float GetAlphaMax() const {
		return m_pParams->GetValueFloat(DOP_ALPHA);
	}

	__inline void SetAlphaMin(float fAlpha) {
		m_pParams->SetValue(DOP_ALPHA_MIN, fAlpha);
	}

	__inline void SetAlphaMax(float fAlpha) {
		m_pParams->SetValue(DOP_ALPHA, fAlpha);
	}

	__inline void SetNumGrowSteps(int nSteps) {
		m_pParams->SetValue(DOP_GROW_STEPS, nSteps);
	}

	__inline int GetNumGrowSteps() const {
		return m_pParams->GetValueInt(DOP_GROW_STEPS);
	}

	__inline void SetNumTransparentSteps(int nSteps) {
		m_pParams->SetValue(DOP_TRANSPARENT_STEPS, nSteps);
	}

	__inline int GetNumTransparentSteps() const {
		return m_pParams->GetValueInt(DOP_TRANSPARENT_STEPS);
	}

private:
	void _drawDroplet(CPolyLine &polyLine);
	void _draw(int nCutoff);

public:
	friend class CStreamLine;
	friend class CSpeedLine;
};

