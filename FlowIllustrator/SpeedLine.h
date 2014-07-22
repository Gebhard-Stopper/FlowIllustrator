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
#include "polyline.h"

/**
 *	This class represents speed lines, used to indicate motion of e.g. vortices or time lines.
 *
 *	@see CVortexObj
 *	@see CTimeLine
 */
class CSpeedLine :
	public CPolyLine
{
public:
	/**
	 *	Create a new CSpeedLine object.
	 *
	 *	@param color The main Color of the new CSpeedLine.
	 *	@param fThickness The thickness of the new CSpeedLine. Also used as the maximum thickness, if m_dwStyle uses SL_THICKNESS_INCREASE or SL_THICKNESS_DECREASE.
	 *	@param fAlpha Maximum alpah value, used if m_dwStyle uses SL_TRANSPARENCY_INCREASE or SL_TRANSPARENCY_DECREASE.  m_fAlphaMin is set to fAlpha/100.
	 */
	CSpeedLine(const floatColor& color, float fThickness, float fAlpha);

	CSpeedLine(const CDrawingObjectParams &params);

	/**
	 *	Destroy this CSpeedLine and all of its data.
	 */
	~CSpeedLine(void);

	/**
	 *	Retrieve a deep copy of this CSpeedLine object.
	 *	
	 *	@return A new CSpeedLine object, identical to this.
	 *
	 *	@remarks The used must delete this object, if it is no longer needed.
	 */
	virtual CDrawingObject* Duplicate() const;
};