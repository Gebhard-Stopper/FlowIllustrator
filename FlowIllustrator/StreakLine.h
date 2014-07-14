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
#include "pathline.h"

/**
 *	This class is used to represent streak lines.
 *	It is derived from CStreamLine and extents the original functionality,
 *	by allowing to specify a time step, which is used as the starting time 
 *	step for streak line integration.
 *	Additionally, it provides the ability to be renderd as streak paricles.
 */
class CStreakLine :
	public CPathLine
{
public:
	/**
	 *	Create a new CStreakLine object.
	 *
	 *	@param ptOrigin Reference to a CPointf, spcifying the spatial origin of the path line, in domain coordinates.
	 *	@param nNumIntegrationSteps Number of integration steps to perform for path line integration.
	 *	@param nStartFrame Frame (time step), in which path line integration is started.
	 *	@param fArrowLength Lenght of arrows, displayed along the path line.
	 *	@param color Main color of the path line.
	 *	@param fStepLength Step length used for path line integration.
	 */
	CStreakLine(const CPointf &ptOrigin, unsigned int nNumIntegrationSteps, int nStartFrame, float fArrowLength, const floatColor& color, float fStepLength = 0.01f);

	/**
	 *	Destroy this CStreakLine ofbejct and all of its data.
	 */
	virtual ~CStreakLine(void);

protected:
	CStreakLine(const CDrawingObjectParams &params);

public:
	/**
	 *	Retrieve, if this CStreakLine is drawn as streak particles.
	 *
	 *	@return This function returns true, if this CStreakLine is draws as streak particles, otherwise flase.
	 */
	__inline bool RenderAsParticles() const { 
		return m_pParams->GetValueBool(DOP_RENDER_AS_PARTICLES); 
	}

	/**
	 *	Set, if this CStreakLine is drawn as streak particles.
	 *
	 *	@param bAsParticles The new value as bool.
	 */
	__inline void RenderAsParticles(bool bAsParticles) { 
		m_pParams->SetValue(DOP_RENDER_AS_PARTICLES, bAsParticles); 
	}

	/**
	 *	Retrieve a deep copy of this CStreakLine object.
	 *	
	 *	@return A new CStreakLine object, identical to this.
	 *
	 *	@remarks The used must delete this object, if it is no longer needed.
	 */
	virtual CDrawingObject* Duplicate() const;

protected:
	/**
	 *	Set one or more parameters to a CStreakLine.
	 *
	 *	@param	params A reference to a CDrawingObjectParams object that holds the parameters to be set.
	 *
	 *	@remarks	This function must be implemented in derived classes to ensure that all parameters are
	 *				set correctly.
	 */
	virtual bool setParam(DrawinObjectParamName paramID, const CSimpleVariant &val);

	/**
	 *	Specialised drawing function to support additional drawing styles.
	 */
	virtual void Draw();

private:
	void _draw();

	friend class CSVGConverter;
};

