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
#include "vectorfield.h"
#include "Vector2D.h"
#include "Vector3D.h"
#include "armadillo"
#include "RectF.h"
#include "ScalarField.h"

static const float EPSILON = 1e-3f;
/**
 *	Enumeration of critical point types according to Helmann and Hesselink
 */
enum CRITICAL_POINT_TYPE 
{ 
	NONE,				/**< Not defined / invalid */
	SADDLE,				/**< Saddle point */
	REPELLING_SADDLE,	/**< Repelling saddle */
	REPELLING_FOCUS,	/**< Repelling focus */
	CENTER,				/**< Center vortex */
	ATTRACTING_FOCUS,	/**< Vortex with attracting focus */
	ATTRACTING_SADDLE	/**< Vortex with repelling focus */
};

/**
 *	This class resembles a two-dimensional vector field.
 *	The vector field consists of N x M samples on a uniform grid.
 *	This uniform grid is embedded into a rectangual domain of size X x Y.
 *
 *	CVectorField2D abstracts from the sample grid and provides an interface to access
 *	any point inside the domain by specifying appropriate coordinates in domain space.
 *	Thereby, CVectorField2D takes care of interpolating values inbetween samples and
 *	automatically applies von Neumann boundary conditions, where necessary.
 */
class CVectorField2D :
	public CDataField2D
{
protected:
	CMathVector		*m_pData;	/**< Pointer to the actual vector data. For convenient access, it points to a class derived from CMathVector, instead of raw float values */ 
	bool			 m_bIsMM;	/**< Flag that indicates, if the data in m_pData is retrieved via a memory mapped file */

protected:
	CVectorField2D();

public:
	/** 
	 *	Construct a new CVectorField2D
	 *
	 *	@param rcDomain	Reference to CRectF defining the domain of the vector field.
	 *	@param nSamplesX Number of samples in X-Direction
	 *	@param nSamplesY Number of samples in Y-Direction
	 */
	CVectorField2D(const CRectF &rcDomain, int nSamplesX, int nSamplesY);
	virtual ~CVectorField2D(void);

/****************************************************************************
*	Public functions that receive parameters in domain coordinates and		*
*	automatically project them onto their corresponding grid locations.		*
*****************************************************************************/
public:
	/**
	 *	Returns the CVector2D at the specified location.
	 *	If the specified location is not an exact sample location, the returned value
	 *	is obtained by bi-linear interpolation.
	 *
	 *	@param dx X-component of the coordinate in domain space
	 *	@param dy Y-component of the coordinate in domain space
	 *
	 *	@return CVector2D at the specified location.
	 */
	virtual CVector2D GetVectorAt(float dx, float dy) const;

	/**
	 *	Returns the unnormalised vorticity at the specified location.
	 *
	 *	@param dx X-component of the coordinate in domain space
	 *	@param dy Y-component of the coordinate in domain space
	 *
	 *	@return The vorticity as float.
	 *
	 *	@remarks A positive vorticity value indicates a counter clockwise, a negative value a clockwise rotation direction.
	 */
	virtual float GetVorticity(float dx, float dy) const;

	/**
	 *	Starts Runge-Kutta integration at the specified location.
	 *
	 *	@param dx X-component of the coordinate in domain space
	 *	@param dy Y-component of the coordinate in domain space
	 *	@param numSteps Maximum number of integration steps. 
	 *	@param stepLen Step length for RK integrator in grid space
	 *	@param pOutBuff Vertices of the resulting stream line in domain space
	 */
	virtual void integrateRK4(float dx, float dy, int numSteps, float stepLen, CPointf *pOutBuff) const;

	/**
	 *	Returns the type of the critical point, based on the classification scheme
	 *	by Helman and Hesselink for at the specified location.
	 *
	 *	@param point Location of the critical point to be queries in domain coordinates.
	 */
	virtual CRITICAL_POINT_TYPE GetCriticalPointType(const CPointf& point) const;


	virtual CScalarField2D* GetMagnitudeField() const;
	/**
	 *	Returns the corresponding vorticity field to the vector field.
	 *
	 *	@param bGetMagnitude If true, the vorticity magnitude field is returned
	 *
	 *	@remarks The returned scalar field has the same domain recangle and number of sample points as this CVectorField2D.
	 *	The returned scalar field has to be deleted by the user if not needed anymore.
	 */
	virtual CScalarField2D* GetVorticityField(bool bGetMagnitude = false) const;

	/**
	 *	Returns a portion of the corresponding vorticity field to the vector field.
	 *
	 *	@param bGetMagnitude If true, the vorticity magnitude field is returned
	 *	@param rect Rectangular region for which the vorticity field is to be obtained.
	 *
	 *	@remarks The returned scalar field has the same dimain recangle and number of sample points as the 
	 *	corresponding vector field.
	 *	The returned scalar field has to be deleted by the user if not needed anymore.
	 *
	 *	If the specified region overlaps the domain boundaries it is clamped s.t. it lies
	 *	inside the valid domain.
	 */
	virtual CScalarField2D* CVectorField2D::GetVorticityField(CRectF rect, bool bGetMagnitude) const;

	/**
	 *	Retrieves the jacobian matrix for the specified location and stores the result in pJacobian.
	 *
	 *	@param dx X-component of the coordinate in domain space
	 *	@param dy Y-component of the coordinate in domain space
	 *	@param pJacobian Pointer to a valid arma::fmat22 that is filled with the 
	 *					resulting directional derivatives as follows:<BR>
	 *
	 *					( U_x  U_y ) <BR>
	 *					( V_x  V_y )
	 *
	 */
	void GetJacobian(float dx, float dy, arma::fmat22 *pJacobian) const;


protected:
	/*
		Internal functions, called by the above, public functions.
		Coordinate parameters are expected to be in grid-space
	*/

	/**
	 *	This function is called by internally to retrieve an actual CVector2D.
	 *	It operates directly on the unifrm grid.
	 *
	 *	@param x X-component of the CVector2D to be retrieved in grid coordinates.
	 *	@param y Y-component of the CVector2D to be retrieved in grid coordinates.
	 *
	 *	@return The CVector2D at the specified location.
	 */
	__inline CVector2D _getVectorAt(float x, float y) const;//Amira

	/**
	 *	This function is called by internally to retrieve an actual vorticity value.
	 *	It operates directly on the unifrm grid.
	 *
	 *	@param x X-component of the vorticity value to be retrieved in grid coordinates.
	 *	@param y Y-component of the vorticity value to be retrieved in grid coordinates.
	 *
	 *	@return The vorticity value at the specified location.
	 */
	float _getVorticity(float x, float y) const;

	/**
	 *	This function is called by internally to retrieve an actual absolute vorticity value.
	 *	It operates directly on the unifrm grid.
	 *
	 *	@param x X-component of the vorticity value to be retrieved in grid coordinates.
	 *	@param y Y-component of the vorticity value to be retrieved in grid coordinates.
	 *
	 *	@return The absolute vorticity value at the specified location.
	 */
	float _getVorticityAbs(float x, float y) const;

	friend class CAmiraVectorField2D;
};

