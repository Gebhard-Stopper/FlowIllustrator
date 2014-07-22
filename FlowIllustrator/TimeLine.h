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
#include "streamline.h"
#include "SpeedLine.h"

/**
 *	This class represents a characteristic time line curve.
 *	A time line connects particles, injected into a vector filed at the same time step,
 *	by typically at different locations.
 *	In this implementation, these particles are initially generated as evenly spaced particles along
 *	a seeding line, defined by two points.
 *
 *	For each particle (sample) on the seeding line, a path line integration is performed,
 *	using the specified number of integration steps and step length. Thus, time line integration can be solw.
 */
class CTimeLine :
	public CStreamLine
{
protected:
	shared_ptr<CDrawingObjectParams>	m_SpeedLineParams;

public:
	/**
	 *	Create a new CTimeLin e object.
	 *	
	 *	@param ptSeedLineStart	Reference to a CPointf, defining the beginning of the seeding line.
	 *	@param ptSeedLineEnd	Reference to a CPointf, defining the end of the seeding line.
	 *	@param numSamples		Number of samples along the seeding line.
	 *	@param numIntegrationSteps	Number of integration steps per sample.
	 *	@param fStepLen	Integration step length per sample.
	 *	@param fThickness	Thickness of this CTimeLine object.
	 *	@param pColor	Pointer to a floatColor, specifying the main color of this CTimeLIne.
	 */
	CTimeLine(const CPointf &ptSeedLineStart, const CPointf &ptSeedLineEnd, int numSamples, int numIntegrationSteps, float fStepLen, float fThickness, const floatColor& color);
	~CTimeLine(void);

protected:
	CTimeLine(const CDrawingObjectParams &params);

private:
	void _init();

protected:
	/**
	 *	Retrive all parameters of a CDrawingObject.
	 *	
	 *	@param params A refernece to a CDrawingObjectParams object to receive the parameters.
	 *
	 *	@remarks	Derived classes must implemented this function, and ensure that it appends all parameters
	 *				to params.
	 **/
	virtual void GetParams(CDrawingObjectParams &params) const; 

	/**
	 *	Override that ensures that the parameters are stored in m_SpeedLineParams as default parameters for new speed lines.
	 */
	virtual bool SetChildObjParams(const CDrawingObjectParams &params);

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
	 *	CTimeLine speciffic override.
	 */
	virtual void OnSetParams();

public:
	/**
	 *	Set the orifgin of the seeding line.
	 *
	 *	@param point Reference to a CPointf, specifying the origin of the seeding line.
	 */
	virtual __inline void SetOrigin(const CPointf& point) { 
		CStreamLine::SetOrigin(point);
		_init();
	}

	/** 
	 *	Set the end point of the seeding line.
	 *
	 *	@param ptSeedLineEnd Reference to a CPointf, specifying the end point of the seeding line.
	 */
	__inline void SetSeedLineEnd(const CPointf &ptSeedLineEnd) { 
		m_pParams->SetValue(DOP_PTEND, ptSeedLineEnd); 
		_init();
	}

	/**
	 *	Retrieve the end point of the seeding line.
	 *
	 *	@return The end point of the seeding line as CPointf.
	 */
	__inline CPointf GetSeedLineEnd() const { 
		return m_pParams->GetValuePointf(DOP_PTEND); 
	}

	/**
	 *	Retrieve the number of samples used to generate this CTimeLine.
	 *
	 *	@return The number of samples along the seeding line as int.
	 */
	__inline int GetNumSamples() const { 
		return m_pParams->GetValueInt(DOP_NUM_SAMPLES);  
	}

	/**
	 *	Set the number of samples along the seeding line.
	 *
	 *	@param numSamples The new number of samples along the seeding line.
	 */
	__inline void SetNumSamples(int numSamples) { 
		m_pParams->SetValue(DOP_NUM_SAMPLES, numSamples); 
	}

	/**
	 *	Translate this CTimeLine.
	 *
	 *	@param tx Translation in X-direction
	 *	@param ty Translation in Y-direction
	 */
	virtual __inline void Translate(float tx, float ty) { 
		CDrawingObject::Translate(tx, ty);
		CPointf pt(tx, ty);

		SetOrigin(GetOrigin() + pt);
		SetSeedLineEnd(GetSeedLineEnd() + pt);
		_init();
	}

	/**
	 *	Show / hide the trajectory of this CTimeLine.
	 *
	 *	@param bShow If true, the trajectory is displayed.
	 */
	__inline void ShowTrajectory(bool bShow) {
		m_pParams->SetValue(DOP_SHOWTRAJECTORY, bShow);
	}

	/**
	 *	Retrieve, if the trajectory of this CTimeLine object is displayed.
	 *
	 *	@return If true, the trajectory is displayed, false otherwise.
	 */
	__inline bool ShowTrajectory() const {
		return m_pParams->GetValueBool(DOP_SHOWTRAJECTORY);
	}

	/**
	 *	Set if the seeding line of this CTimeLine is to be drawn.
	 *
	 * @param bDraw Set this to true to draw the seeding line.
	 */
	__inline void DrawSeedingLine(bool bDraw) {
		m_pParams->SetValue(DOP_SHOW_SEEDINGLINE, bDraw);
	}

	/**
	 *	Retrieve if the seeding line of this CTimeLine is to be drawn.
	 *
	 * @return Returns true, if the seeding line is drawn, otherwise false.
	 */
	__inline bool DrawSeedingLine() const {
		return m_pParams->GetValueBool(DOP_SHOW_SEEDINGLINE);
	}

	/**
	 *	Get an exact copy of this CTimeLine object.
	 *
	 *	@return A pointer the the new CTimeLine object.
	 */
	virtual CDrawingObject* Duplicate() const;

	/** 
	 *	Add a new CSpeedLine as trajectory.
	 *
	 *	@param pTrajectory A pointer to a CSpeedLine object.
	 *
	 *	@remarks	This CTimeLine object takes control of the supplied CSpeedLine and deletes it if no longer needed.
	 *				Do not delete the Supplied CSpeedLine yourself.
	 */
	void AddTrajectory(CSpeedLine *pTrajectory);

	/**
	 *	Draw this CTimeLine.
	 */
	virtual void Draw();

	friend class CSVGConverter;
};

