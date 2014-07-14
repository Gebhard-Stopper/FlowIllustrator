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

/**
 *	This class is used to represent path lines.
 *	It is derived from CStreamLine and extents the original functionality,
 *	by allowing to specify a time step, which is used as the starting time 
 *	step for path line integration.
 */
class CPathLine :
	public CStreamLine
{
public:
	/**
	 *	Create a new CPathLine object.
	 *
	 *	@param ptOrigin Reference to a CPointf, spcifying the spatial origin of the path line, in domain coordinates.
	 *	@param nNumIntegrationSteps Number of integration steps to perform for path line integration.
	 *	@param nStartFrame Frame (time step), in which path line integration is started.
	 *	@param fArrowLength Lenght of arrows, displayed along the path line.
	 *	@param color Main color of the path line.
	 *	@param fStepLength Step length used for path line integration.
	 */
	CPathLine(const CPointf &ptOrigin, unsigned int nNumIntegrationSteps, int nStartFrame, float fArrowLength, const floatColor& color, float fStepLength = 0.01f);

	/**
	 *	Destroy this CPathLine ofbejct and all of its data.
	 */
	virtual ~CPathLine(void);

protected:
	CPathLine(const CPointf &ptOrigin, unsigned int nNumIntegrationSteps, int nStartFrame, float fArrowLength, const floatColor& color, DrawingObjectType type, float fStepLength = 0.01f);
	CPathLine(const CDrawingObjectParams &params);
	CPathLine(DrawingObjectType nType);

protected:
	/**
	 *	Set one or more parameters to a CPathLine.
	 *
	 *	@param	params A reference to a CDrawingObjectParams object that holds the parameters to be set.
	 *
	 *	@remarks	This function must be implemented in derived classes to ensure that all parameters are
	 *				set correctly.
	 */
	virtual bool setParam(DrawinObjectParamName paramID, const CSimpleVariant &val);

public:
	/**
	 *	Retrieve a deep copy of this CPathLine object.
	 *	
	 *	@return A new CPathLine object, identical to this.
	 *
	 *	@remarks The used must delete this object, if it is no longer needed.
	 */
	virtual CDrawingObject* Duplicate() const;



friend class CSVGConverter;
};

