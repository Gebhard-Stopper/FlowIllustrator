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
#include "PolyLine.h"
#include "AmiraVectorField2D.h"
#include "Arrow.h"
#include "BSpline.h"
#include <vector>

using namespace std;
/**
 *	This class represents stream lines in a two dimensional vector field.
 *	It further serves as base class for other characteristc curves of a vector field, 
 *	such as CPathLine, CStreakLine and CTimeLine.
 *
 *	CStreamLine is derived from CPolyLine, to inherit the basic functionality and drawing mechanism of a line-type structure.
 *	Further, it enhances CPolyLines functionality, by additional parameters and drawing styles.
 *	These additionl drawing styles include (optional) arrow glyphs, attached to a CStreamLine and a flexible mechanism to draw halos.
 *	
 *	Halos can be used, to create various visual effects, including shadows, contours or "glowing" effects.
 *	
 *
 *	Stream line integration is not performed by CStreamLine itself. Instead, CFlowIllustratorView and CAmiraVectorField provide the required functionality.
 *	This also applies for classes derived from CStreamLine.
 *
 *	@see CFlowIllustratorView
 *	@see CAmiraVectorField2D
 *	@see CFlowIllustratorView::calcCharacteristicLine()
 *	@see CFlowIllustratorView::calcStreamLine()
 *	@see CAmiraVectorField2D::integrateRK4()
 */
class CStreamLine :
	public CPolyLine
{
public:
	/**
	 *	Create a new CStreamLine object.
	 *
	 *	@param ptOrigin Reference to a CPointf, spcifying the spatial origin of the path line, in domain coordinates.
	 *	@param nNumIntegrationSteps Number of integration steps to perform for path line integration.
	 *	@param fArrowLength Lenght of arrows, displayed along the path line.
	 *	@param color Main color of the path line.
	 *	@param fStepLength Step length used for path line integration.
	 */
	CStreamLine(const CPointf &ptOrigin, unsigned int nNumIntegrationSteps, float fArrowSize, const floatColor& color, float fStepLength = 0.01f);

	/**
	 *	Destroy this CStreamLine and all of its data.
	 */
	virtual ~CStreamLine(void);

protected:
	CStreamLine(const CPointf &ptOrigin, unsigned int nNumIntegrationSteps, float fArrowSize, const floatColor& color, DrawingObjectType type, float fStepLength = 0.01f);
	CStreamLine(const CDrawingObjectParams &params);
	CStreamLine(DrawingObjectType nType);

protected:
	bool			m_bNeedRecalc;			/**< Flag, which indicates, if this CStreamLine needs to be recalculated.*/
	vector<CArrow>	m_Arrows;				/**< Vector, storing the arrows to be displayed along this CStreamLine.*/

protected:
	virtual void _OnParamsChanged();
	void Init(unsigned int nNumIntegrationSteps, float fStepLength);

public:
	/**
	 *	Retrieve the origin of this CStreamLine.
	 *
	 *	@return The origin as CPointf.
	 */
	__inline CPointf GetOrigin() const { 
		return m_pParams->GetValuePointf(DOP_ORIGIN); 
	}

	/**
	 *	Retrieve the maximum number of integrationSteps of this CStreamLine.
	 *
	 *	@return The maximum number of integration steps as size_t.
	 */
	__inline int GetMaxIntegrationLen() const{ 
		return m_pParams->GetValueInt(DOP_INTEGRATIONSTEPS); 
	}

	/**
	 *	Retrieve the step size, used to integrate this CStreamLine.
	 *
	 *	@return The step size as float;
	 */
	__inline float GetStepSize() const { 
		return m_pParams->GetValueFloat(DOP_STEPLENGTH); 
	}

	/**
	 *	Retrieve, if this CStreamLine needs to be re-calculated.
	 *
	 *	@return This function returns true, if this CStreamLine needs to be re-calculated, otherwise false.
	 */
	bool NeedRecalc() const; 

	/**
	 *	Set, if this CStreamLine needs to be re-calculated.
	 *
	 *	@param bNeedRecalc The new value as bool.
	 */
	void NeedRecalc(bool bNeedRecalc); 

	/**
	 *	Set the maximum integration lenght of this CStreamLine.
	 *
	 *	@param The new integration lenght as size_t.
	 *
	 *	@remarks	This function sets m_bNeedRecalc to true and clears all vertices of this CStreamLine,
	 *				requiring the stream line to be re-caclulated before the next drawing operation.
	 */
	__inline void SetMaxIntegrationLen(int nLen) {
		NeedRecalc(true);
		m_pParams->SetValue(DOP_INTEGRATIONSTEPS, nLen<1?1:nLen);
	}

	/**
	 *	Set the integration step size.
	 *
	 *	@param fStepSize The new step size as float.
	 */
	__inline void SetStepSize(float fStepSize) { 
		m_pParams->SetValue(DOP_STEPLENGTH, fStepSize); 
		NeedRecalc(true);
	}

	/**
	 *	Set a new origin for this CStreamLine.
	 *
	 *	@param point Reference to a CPoint, specifying the new origin.
	 */
	__inline void SetOrigin(const CPoint& point) { 
		SetOrigin(CPointf( static_cast<float>(point.x), static_cast<float>(point.y) ));
	}

	/**
	 *	Set a new origin for this CStreamLine.
	 *
	 *	@param point Reference to a CPointf, specifying the new origin.
	 */
	virtual __inline void SetOrigin(const CPointf& point) { 
		m_pParams->SetValue(DOP_ORIGIN, point); 
		NeedRecalc(true);
	}

	/**
	 *	Retrieve a deep copy of this CStreamLine object.
	 *	
	 *	@return A new CStreamLine object, identical to this.
	 *
	 *	@remarks The used must delete this object, if it is no longer needed.
	 */
	virtual CDrawingObject* Duplicate() const;

	/**
	 *	Enables or disables arrows to be displayed along this CStreamLine.
	 *
	 *	@param nNumArrows The number of arrows to be displayed.
	 *	@param bShow Flag, which indicates if the arrows are displayed.
	 */
	//void ShowArrows(int nNumArrows, bool bShow = true);
	__inline void ShowArrows(bool bShowArrows) {
		m_pParams->SetValue(DOP_SHOWARROWS, bShowArrows);
	}

	__inline bool ShowArrows() const {
		return m_pParams->GetValueBool(DOP_SHOWARROWS);
	}

	__inline void SetArrowCount(int nArrowCount) {
		m_pParams->SetValue(DOP_NUMARROWS, nArrowCount);
	}

	__inline int GetArrowCount() const {
		return m_pParams->GetValueInt(DOP_NUMARROWS);
	}

	/**
	 *	Modify the length of the arrows alogn this CStreamLine.
	 *
	 *	@param fLen The new length.
	 *
	 *	@remarks	The width of the arrows is scaled proprtionally to the length,
	 *				wiht a relation of 2:3 (width:lenght).
	 */
	__inline void SetArrowSize(float fSize) {
		m_pParams->SetValue(DOP_ARROWSIZE, fSize);
	}

	__inline float GetArrowSize() const {
		return m_pParams->GetValueFloat(DOP_ARROWSIZE);
	}

	/**
	 *	Retrieve the smoothing factor of this CStreamLine.
	 *
	 *	@return The smoothing factor as int.
	 */
	__inline int GetSmoothness() const {
		return m_pParams->GetValueBool(DOP_SMOOTHNESS);
	}

	/**
	 *	Set the smoothing factor of this CStreamLine.
	 *
	 *	@param nSmoothness the New Smoothing factor.
	 *
	 *	@remarks The smoothing factor is used as kernel half-size for a box filter.
	 */
	__inline void SetSmoothness(int nSmoothness) {
		m_pParams->SetValue(DOP_SMOOTHNESS, nSmoothness);
	}

	/**
	 *	Retrieve, if this CStreamLine is displayed as droplet / streamlet.
	 *
	 *	@return IF true, this CStreamLine is displayed as droplet / streamlet.
	 */
	__inline bool DrawAsDroplet() const { return m_pParams->GetValueBool(DOP_DRAWASDROPLETS); }

	/**
	 *	Set,if this CStreamLine is displayed as droplet / streamlet.
	 *
	 *	@param bDrawAsDroplet The new drawing style as bool (true = droplet / streamlet , false = poly line with optional arrows).
	 */
	__inline void DrawAsDroplet(bool bDrawAsDroplet) { m_pParams->SetValue(DOP_DRAWASDROPLETS, bDrawAsDroplet); }

	/**
	 *	Set the new arrow color of this CStreamLine.
	 *
	 *	@param color The new arrow color as reference to a floatColor.
	 */
	__inline void SetArrowColor(const floatColor &color) { 
		m_pParams->SetValue(DOP_ARROWCOLOR, color); 
		_applyArrowColor(); 
	}

	inline const floatColor& GetArrowColor() const {
		return m_pParams->GetValueFloatColor_r(DOP_ARROWCOLOR);
	}

	/**
	 *	Translate this CStreamLine.
	 *
	 *	@param tx Translation in X-direction
	 *	@param ty Translation in Y-direction
	 */
	virtual __inline void Translate(float tx, float ty) { 
		CDrawingObject::Translate(tx, ty);
		CPointf pt(GetOrigin());
		pt.x += tx;
		pt.y += ty;
		SetOrigin(pt);
	}

		/**
	 *	Retrieve, if a fixed start frame is used.
	 *
	 *	@return true, if this path line uses a fixed start frame.
	 */
	__inline bool UseFixedStartFrame() const { return m_pParams->GetValueBool(DOP_USE_STARTFRAME); }

	/**
	 *	Set / reset, if this CPathLine uses a fixed start frame.
	 *
	 *	@oparam m_bUseStartFrame Frlg, which indicates, if a fixed start frame is to be used.
	 */
	__inline void UseFixedStartFrame(bool bUse) { 
		m_pParams->SetValue(DOP_USE_STARTFRAME, bUse); 
		NeedRecalc(true);
	}

	/**
	 *	Retrieve the starting frame of this CPathLine.
	 *
	 *	@return The start frame as int.
	 */
	__inline unsigned int GetStartFrame() const { 
		return m_pParams->GetValueInt(DOP_STARTFRAME); 
	}

	__inline void SetStartFrame(int nStartFrame) {
		m_pParams->SetValue(DOP_STARTFRAME, nStartFrame);
		NeedRecalc(true);
	}
	 
protected: 
	CBSpline* GetStreamlineApprox(CPointf* pStreamLine) const; //Experimental

	/**
	 *	Internal functions to apply new parameters to this CStreamLine.
	 */
	virtual bool setParam(DrawinObjectParamName paramID, const CSimpleVariant &val);
	virtual void OnSetParams();

	void DrawArrows();

public:
	/**
	 *	Specialised drawing function to allows for additional drawing styles.
	 */
	virtual void Draw();

	/**
	 *	Smoothes this CStreamLine.
	 *
	 *	@param nSmoothness Kernel half-size used to smooth this function.
	 *
	 *	@remarks	This function uses a box filter to smooth the vertices of this CStreamLine.
	 *				Thereby, the locations of the first and the last vertex of this CStreamLien are not altered.
	 */
	virtual void Smooth(int nSmoothness = 3);

#ifdef WIN32
	/**
	 *	Overloaded version of CStreamLine.
	 *	Used to output the vertices of this CStreamLine in the SVG format.
	 *
	 *	@see CDrawingObject::FinalizeToString()
	 */
	virtual void FinalizeToString(CString &str, float fLineWidthFactor) const;
#endif

private:
	/**
	 *	THis function iterates over all arrows of this CStreamLine and applies the specified arrow color.
	 */
	void _applyArrowColor();

	friend class CPathLine;
	friend class CSVGConverter;
};

