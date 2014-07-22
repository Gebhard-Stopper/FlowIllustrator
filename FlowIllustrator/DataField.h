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
#include "RectF.h"
#include <limits>

__inline bool isFinite(float f) {
	return (f != std::numeric_limits<float>::infinity() &&
			f != -std::numeric_limits<float>::infinity());
}

/**
 *	CDataField2D is a virtual calss that serves as base data structure for 2D data fields.
 *	It provides basic functionality to query the extents of a field or to perform coordinate transformations.
 *
 *	This virtual class is used as base-class for 2D scalar and vector filds.
*/

class CDataField2D
{
protected:
	unsigned int	m_nSamplesX;		/**< Total number of samples in X-direction. */
	unsigned int	m_nSamplesY;		/**< Total number of samples in Y-direction. */
	unsigned int	m_nMaxIdxX;			/**< Greatest valid index in X-direction. */
	unsigned int	m_nMaxIdxY;			/**< Greatest valid index in Y-direction. */
	CRectF			m_rcDomain;			/**< Rectangle describing the domain. */

public:
	/**
	 *	@param rcDomain Rectangle bounding box of the domain.
	 *	@param nSamplesX Number of samples in X-direction
	 *	@param nSamplesY Number of samples in Y-direction
	 */
	CDataField2D(const CRectF &rcDomain, int nSamplesX, int nSamplesY);

	/**
	 *	Purely virtual destructor.
	 *	Must be implemented by derived classes
	 */
	virtual ~CDataField2D(void) = NULL;

	//Inline functions
public:

	/**
	 *	Helper function to initialise a CDataField2D.
	 *
	 *	@param rcDomain Rectangle bounding box of the domain.
	 *	@param nSamplesX Number of samples in X-direction
	 *	@param nSamplesY Number of samples in Y-direction
	 *
	 */
	__inline void Init(const CRectF &rcDomain, int nSamplesX, int nSampleY) {
		m_nSamplesX = nSamplesX;
		m_nSamplesY = nSampleY;
		m_rcDomain = rcDomain;
	}

	/**
	 *	Returns the number of samples in X-direction.
	 *
	 *	@return The number of samples in X-direction as unsigned int.
	*/
	__inline unsigned int GetExtentX() const { 
		return m_nSamplesX; 
	}

	/**
	 *	Returns the number of samples in Y-direction.
	 *
	 *	@return The number of samples in Y-direction as unsigned int.
	*/
	__inline unsigned int GetExtentY() const { 
		return m_nSamplesY; 
	}

	/**
	 *	Returns the number of samples per domain unit in X-direction.
	 *
	 *	@return The number of samples per unit in X-direction as float.
	 */
	__inline float GetSamplesPerUnitX() const { 
		return (m_nSamplesX) / m_rcDomain.getWidth(); 
	}

	/**
	 *	Returns the number of samples per domain unit in Y-direction.
	 *
	 *	@return The number of samples per unit in Y-direction as float.
	 */
	__inline float GetSamplesPerUnitY() const { 
		return (m_nSamplesY) / m_rcDomain.getHeight(); 
	}

	/**
	 *	Returns the bounding rect of the domain.
	 *
	 *	@return A CRectF representing the bounding rect of the domain.
	 *
	 *	@see CRectF
	 */
	__inline CRectF GetDomainRect() const { 
		return m_rcDomain; 
	}

	/**
	 *	Returns the coordinates in domain space to a pair of grid coordinates.
	 *
	 *	@param gx X-component of the grid coordinate
	 *	@param gy Y-component of the grid coordinate
	 *
	 *	@return The corresponding domain coordinates as CPointf.
	 *
	 *	@see CPointf
	*/
	__inline CPointf GetDomainCoordinates(float gx, float gy) const {
		CPointf retVal;
		_getDomainCoordinates(gx, gy, retVal.x, retVal.y);
		return retVal;
	}

	/**
	 *	Returns the grid coordinates to a pair of domain coordinates.
	 *
	 *	Parameters: 
	 *	@param dx X-component of the domain coordinate
	 *	@param dy Y-component of the domain coordinate
	 *
	 *	@return The corresponding grid coordinates as CPointf.
	 *
	 *	@see CPointf
	*/
	__inline CPointf GetGridCoordinates(float dx, float dy) const {
		CPointf pt;
		_getGridCoordinates(dx, dy, pt.x, pt.y);
		return pt;
	}

	/**
	 *	Rounds the parameter val to the nearest value and returns it.
	 *	If the supplied parameters has a fractional part >= .5 this function returns ceil(val),
	 *	oterwise floor(val)
	 *
	 *	@param val The value to be rounded.
	 *
	 *	@return The rounded value.
	 */
	__inline float round(float val) const
	{
		int dummy = int(val);
		if ( (val - float(dummy)) < 0.5f )
			return floor(val);

		return ceil(val);
	}


	/**
	 *	This function takes a coordinate pair in domain space and returns the
	 *	nearest sample position.
	 *
	 *	@param posD Location in domain space.
	 *
	 *	@return The closest sample in grid coordinates.
	*/
	__inline CPointf GetClosestSamplePos(const CPointf& posD) const
	{
		CPointf dummy;
		_getGridCoordinates(posD.x, posD.y, dummy.x, dummy.y);

		return CPointf(round(dummy.x), round(dummy.y));
	}

protected:
	/**
	 *	Called internally by GetGridCoordinates().
	 *
	 *	@param dx X-component of the input coordinate in domain coordinates, as float.
	 *	@param dy Y-component of the input coordinate in domain coordinates, as float.
	 *	@param gx Reference to a float, which receives the corresponding X-component to dx, in grid coordinates.
	 *	@param gy Reference to a float, which receives the corresponding Y-component to dy, in grid coordinates.
	 */
	__inline void _getGridCoordinates(__in float dx, __in float dy, __out float &gx, __out float &gy) const {
		gx = (dx - m_rcDomain.m_Min.x)/m_rcDomain.getWidth() * (m_nSamplesX-1);
		gy = (dy - m_rcDomain.m_Min.y)/m_rcDomain.getHeight() * (m_nSamplesY-1);
	}

	/**
	 *	Called internally by GetDomainCoordinates().
	 *
	 *	@param gx X-component of the input coordinate in grid coordinates, as float.
	 *	@param gy Y-component of the input coordinate in grid coordinates, as float.
	 *	@param gx Reference to a float, which receives the corresponding X-component to gx, in domain coordinates.
	 *	@param gy Reference to a float, which receives the corresponding Y-component to gy, in domain coordinates.
	 */
	__inline void _getDomainCoordinates(__in float gx, __in float gy, __out float &dx, __out float &dy) const {
		dx = m_rcDomain.m_Min.x + (( gx / (m_nSamplesX-1) ) * m_rcDomain.getWidth()) ;
		dy = m_rcDomain.m_Min.y + (( gy / (m_nSamplesY-1) ) * m_rcDomain.getHeight()) ;
	}
};

