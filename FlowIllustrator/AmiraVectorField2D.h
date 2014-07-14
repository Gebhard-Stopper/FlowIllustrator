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
#include "vectorfield2d.h"
#include "BasicFileReader.h"
#include <vector>

using namespace std;

/**
 * Represents a 2-dimensional time-dependent vectorfield on a uniform grid.
 */
class CAmiraVectorField2D :
	public CVectorField2D
{
protected:
	//Navigation through timesteps
	unsigned int m_numTimeSteps;	/**< The number of time steps of this CAmiraVectorField2D. */
	unsigned int m_nMaxTimestep;	/**< The greates valid time step index of this CAmiraVectorField2D. */
	unsigned int m_currTimeStep;	/**< The current time step of this CAmiraVectorField2D, which can be accessed of. */
	float		 m_fExtentZ;		/**< Extent of this CAmiraVectorField2D in temporal domain in seconds. */	//
	CBasicFileReader *m_pFileRead;	/**< Pointer to a class, derived from CBasicFileReader, that handles reading vector fields from disk. */

	struct particle {
		CVector3D	pos;
		double		time;
		particle() {}
		particle (CVector3D &p, float t) 
			:pos(p), time(t) {}
	};

public:
	CAmiraVectorField2D();	//path to amira file as parameter
	~CAmiraVectorField2D(void);

public:
	bool LoadAmiraFile(const char* strFileName);

protected:
	void Init(const CRectF &rcDomain, float fExtentZ,  int nSamplesX, int nSamplesY, int nSamplesZ, CAmiraVectorField2D* pData);
	virtual void integrateRK4(float xOrg, float yOrg, int numSteps, float stepLen, CPointf *pOutBuff) const;

public:
	/**
	 *	Starts Runge-Kutta integration at the specified location in domain space at the current time step.
	 *
	 *	@param pos Starting position of integration in domain space
	 *	@param numSteps Maximum number of integration steps. 
	 *	@param stepLen Step length for RK integrator in grid space
	 *	@param bForward If true, forward integration is performed, otherwise backward integration
	 *	@param pOutData Pointer to a std::vector to hold the vertices of the resulting stream line in domain space
	 *	@param rcIntegrationDomain Rectangular region in which the integration is performed. can be <= to the domain rectangle of the vector field.
	 */
	void integrateRK4(const CVector3D &pos, int nNumSteps, float stepLen, bool bForward, vector<CPointf> *pOutBuff, const CRectF &rcIntegrationDomain) const;

	/**
	 * Starts streak line integration, based on the RK4 integrator.
	 * 
	 * @param 
	 * @param pos Starting position of integration in domain space
	 * @param numSteps Maximum number of integration steps. 
	 * @param stepLen Step length for RK integrator in grid space
	 * @param bForward If true, forward integration is performed, otherwise backward integration
	 * @param pOutData Pointer to a std::vector to hold the vertices of the resulting stream line in domain space
	 * @param rcIntegrationDomain Rectangular region in which the integration is performed. can be <= to the domain rectangle of the vector field.
	 */
	void integrateStreakLine(const CVector3D &pos, int nNumSteps, float stepLen, bool bForward, vector<CPointf> *pOutBuff, const CRectF &rcIntegrationDomain) const;

	/**
	 * Starts time line integration, based on the RK4 integrator.
	 * 
	 * @param 
	 * @param ptSeedLineStart Starting point of the seeding line.
	 * @param ptSeedLineEnd End point of the seeding line.
	 * @param nNumSamples Number of samples along the seeding line, defined by ptSeedLineStart and ptSeedLineEnd.
	 * @param numSteps Maximum number of integration steps. 
	 * @param stepLen Step length for RK integrator in grid space
	 * @param bForward If true, forward integration is performed, otherwise backward integration
	 * @param pOutData Pointer to a std::vector to hold the vertices of the resulting time line in domain space
	 * @param rcIntegrationDomain Rectangular region in which the integration is performed. can be <= to the domain rectangle of the vector field.
	 */
	void integrateTimeLine(const CPointf &ptSeedLineStart, const CPointf &ptSeedLineEnd, int nNumSamples, int nNumSteps, float stepLen, bool bForward, vector<CPointf> *pOutBuff, const CRectF &rcIntegrationDomain) const;

	/**
	 *	Returns the type of vortex (center, sink source), based on the classification scheme
	 *	by Helman and Hesselink for the critical point at the specified location.
	 *
	 *	@param point Location of the critical point to be queries in domain coordinates.
	 *
	 *	@return CRITICAL_POINT_TYPE, specifying the type of critical point.
	 */
	virtual CRITICAL_POINT_TYPE GetCriticalPointType(const CPointf& point) const;


//Inline functions for time step navigation
public:
	/**
	 *	Retrieve the total number of time stpes of the vector field.
	 *	
	 *	@return Number of time steps as unsigned integer.
	 */
	__inline unsigned int GetNumTimeSteps() const { 
		return m_numTimeSteps; 
	}

	/**
	 * Retrieve the current time step number.
	 * For the first time step in a vector field, this function will return 0,
	 * whereas for the last time step it will return (n-1), where n is the total number of time steps.
	 *
	 * @return Current time step as unsigned integer.
	 */
	__inline unsigned int GetCurrentTimeStep() const { 
		return m_currTimeStep; 
	}

	/**
	 * Retrieve a pointer to the current time step.
	 * The pointer points to the first element of the current time step.
	 * The retrieved frame contains m_nSamplesX * m_nSamplesY elemnts.
	 *
	 * @see GetExtentX()
	 * @see GetExtentY()
	 *
	 * @return Pointer to the first element of the current time step as CVector2D*
	 */
	__inline CVector2D* GetCurrentFrame() const { 
		return GetFrame(m_currTimeStep); 
	}


	/**
	 * Retrieve a pointer to the specified time step.
	 * The pointer points to the first element of the specified time step.
	 * The retrieved frame contains m_nSamplesX * m_nSamplesY elemnts.
	 *
	 * @return Pointer to the first element of the specified time step as CVector2D*
	 *
	 * @see GetExtentX()
	 * @see GetExtentY()
	 */
	__inline CVector2D* GetFrame(int time) const { 
		return &(reinterpret_cast<CVector2D*>(m_pData)[m_nSamplesX * m_nSamplesY * time]); 
	}

	/**
	 * Got to the next time step.
	 * The current time step is increased by one.
	 * If the current time step is the last time step in the vector field,
	 * the time step remains unchanged.
	 *
	 * @return The current time step number
	 */
	__inline unsigned int NextTimestep() { 
		GotoTimeStep(m_currTimeStep+1);
		return m_currTimeStep;
	}

	/**
	 * Got to the previous time step.
	 * The current time step is decreased by one.
	 * If the current time step is the first step in the vector field,
	 * the time step remains unchanged.
	 *
	 * @return The current time step number
	 */
	__inline unsigned int PrevTimestep() {
		GotoTimeStep(m_currTimeStep-1);
		return m_currTimeStep; 
	}

	/**
	 * Got to the first time step.
	 * If the current time step is the first time step in the vector field,
	 * the time step remains unchanged.
	 *
	 * @return The current time step number
	 */
	__inline unsigned int FirstTimestep() {
		GotoTimeStep(0);
		return m_currTimeStep; 
	}

	/**
	 * Got to the last time step.
	 * If the current time step is the last time step in the vector field,
	 * the time step remains unchanged.
	 *
	 * @return The current time step number
	 */
	__inline unsigned int LastTimestep() {
		GotoTimeStep(m_numTimeSteps-1);
		return m_currTimeStep;
	}

	/**
	 * Got to the specified time step.
	 * The current time step is set to timeStep.
	 * The current time step is only changed, if the specified time step 
	 * is greater 0 and less than the maximum number of time steps.
	 *
	 * @return The current time step number
	 */
	__inline void GotoTimeStep(unsigned int timeStep) {
		if (timeStep >= 0 && timeStep < m_numTimeSteps)
			m_currTimeStep = timeStep;
	}

		//Probing functions
public:
	/**
	 *	Retrieve the interpolated vector at the specified location and time.
	 *
	 *	@param dx X-Component of the location in domain space
	 *	@param dy Y-Component of the location in domain space
	 *	@param z desired value for the z-component of the return value
	 *	@param time Timestep at which the vector is to be interpolatet
	 *
	 *	@return The interpolated vector as CVector3D.
	 *
	 *	@remarks:	The z parameter of the function is used as the desired value of the z-component
	 *				of the return value.
	 *				The resulting value is obtained via tri-linear interpolation in the spatio-temporal domain.
	*/
	CVector3D GetVectorAt(float dx, float dy, float z, float time) const;
	
	/**
	 *	Retrieve the interpolated vector at the specified location and time.
	 *
	 *	@param dx X-Component of the location in domain space
	 *	@param dy Y-Component of the location in domain space
	 *	@param time Timestep at which the vector is to be interpolatet
	 *
	 *	@return The interpolated vector as CVector2D.
	 *
	 *	@remarks The returned value is obtained via tri-linear interpolation in the spatio-temporal domain.
	*/
	CVector2D GetVectorAt(float dx, float dy, float time) const; 
	
	/**
	 *	Retrieve the interpolated vector at the specified location and time.
	 *
	 *	@param dx X-Component of the location in domain space.
	 *	@param dy Y-Component of the location in domain space.
	 *
	 *	@return The interpolated vector as CVector2D.
	 *
	 *	@remarks The returned value is a bi-linear interpolation in the spatial domain at the
				 current time step.
	*/
	virtual CVector2D GetVectorAt(float dx, float dy) const;

	/**
	 *	Retrieve the interpolated vector at the specified location at the current time step.
	 *
	 *	@param point Location to retrieve the vector from, in domain coordinates
	 *
	 *	@return The interpolated vector as CVector2D.
	 *
	 *	@remarks The returned value is a bi-linear interpolation in the spatial domain at the
				 current time step.
	*/
	virtual CVector2D GetVectorAt(const CPointf &point) const;

	/**
	 *	Returns the corresponding vector of the feature flow field at the specified position.
	 *
	 *	@param dx X-Component of the location in domain space.
	 *	@param dy Y-Component of the location in domain space.
	 *	@param time Timestep at which the vector is to be interpolatet.
	 *
	 *	@return The vector of the corresponding feature flow field.
	 */
	CVector3D GetFeatureFlowField(float dx, float dy, float time) const;	//Amira

	/**
	 *	Returns the vorticity value at the specified location at the specified time
	 *
	 *	@param dx X-Component of the location in domain space
	 *	@param dy Y-Component of the location in domain space
	 *	@param time Timestep at which the vorticity is to be interpolatet
	 *
	 *	@return The scalar vorticity value.
	 *
	 *	@remarks A negative value indicates a clockwise, and a positive value a counter clockwise rotation direction.
	 */
	float GetVorticity(float dx, float dy, float time) const;	

	/**
	 *	Returns the vorticity value at the specified location at the current time
	 *
	 *	@param dx X-Component of the location in domain space
	 *	@param dy Y-Component of the location in domain space
	 *
	 *	@return The scalar vorticity value.
	 *
	 *	@remarks A negative value indicates a clockwise, and a positive value a counter clockwise rotation direction.
	 */
	virtual float GetVorticity(float dx, float dy) const;

	/**
	 *	Returns the corresponding vorticity field to the vector field.
	 *
	 *	@param bGetMagnitude If true, the vorticity magnitude field is returned
	 *	@param time Time step at which the vorticity field is to be obtained
	 *
	 *	@return A pointer to a scalar field containing the vorticity values for the specified region
	 *
	 *	@remarks
	 *	The returned scalar field has to be deleted by the user if not needed anymore.
	 *	If time is < 0 the vorticity is obtained from the current time step
	*/
	virtual CScalarField2D* GetVorticityField(bool bGetMagnitude = false, float time = -1) const;
	
	/**
	 *	Returns the corresponding vorticity field to the vector field.
	 *
	 *	@param bGetMagnitude If true, the vorticity magnitude field is returned
	 *	@param rect Rectangular region for which the vorticity field is to be obtained.
	 *	@param time Time step at which the vorticity field is to be obtained
	 *
	 *	@return A pointer to a scalar field containing the vorticity values for the specified region
	 *
	 *	@remarks The returned scalar field has the same domain recangle and number of sample points as the 
	 *	corresponding vector field.
	 *	The returned scalar field has to be deleted by the user if not needed anymore.
	 *	If time is < 0 the vorticity is obtained from the current time step
	*/
	virtual CScalarField2D* GetVorticityField(CRectF rect, bool bGetMagnitude = false, float time = -1) const;

	/**
	 *	Retrieves the Jacobian at the specified location and time.
	 *	Directional derivatives are in X, Y and temporal direction.
	 *
	 *	@param dx X-component of the coordinate in domain space
	 *	@param dy Y-component of the coordinate in domain space
	 *	@param t Time at which the jacobian is to be obtained
	 *	@param pJacobian Pointer to a valid arma::fmat33 that is filled with the 
	 *					 resulting directional derivatives as follows:
	 *
	 *	@remark The jacobian has the following format:<BR>
	 *	( U_x  U_y  U_t )<BR>
	 *	( V_x  V_y  V_t )<BR>
	 *	( 0    0    0   )
	*/
	void  GetJacobian(float dx, float dy, float t, arma::fmat33 *pJacobian) const;

	/**
	 *	Retrieves the Jacobian at the specified location.
	 *	Directional derivatives are in X, Y direction.
	 *
	 *	@param dx X-component of the coordinate in domain space
	 *	@param dy Y-component of the coordinate in domain space
	 *	@param pJacobian Pointer to a valid arma::fmat22 that is filled with the 
	 *					 resulting directional derivatives as follows:
	 *
	 *	@remark The jacobian has the following format:<BR>
	 *	( U_x  U_y )<BR>
	 *	( V_x  V_y )
	*/
	virtual void GetJacobian(float dx, float dy, arma::fmat22 *pJacobian) const;

	/**
	 *	Retrieve the number of samples (time steps) in z-direction per second
	 *
	 *	@return Samples per second.
	 */
	__inline float GetSamplesPerSecond() const {
		return m_numTimeSteps/m_fExtentZ;
	}


	__inline float GetZMax() const {
		return m_fExtentZ;
	}

	//Vortex detection functions
public:
	bool isVortex_lambda2(float x, float y, float time) const;			//Jeong and Hussain
	bool isVortex_Eigenvector(float x, float y, float time) const;		//Sujudi and Haimes
	bool isVortex_ParallelVectors(float x, float y, float time) const;	//Roth and Peickert

protected:
	CVector3D _getVectorAt(float x, float y, float z, float time) const;//Amira
	CVector2D _getVectorAt(float x, float y, float time) const;//Amira
	CVector3D _getFeatureFlowField(float gx, float gy, float t) const;
	void _getJacobian(float x, float y, float t, arma::fmat33 *pJacobian) const;
	float _getVorticity(float x, float y, float time) const;
	bool _integrateStreakLine(float fStepLen, float deltaTime, float &fTimeStep, bool &bError, vector<particle> &result, int nIterations) const;

	/*
		These functions return a pointer to a new CVectorField2D.
		The data pointer of the CVectorField2D however points to data
		int the CAmiraVectorField2D this function was called from!

		The m_pData pointer MUST BE SET TO nullptr before the CVectorField2D is deleted!
	*/
	CVectorField2D* _getCurrentVectorFieldPtr() const;
	CVectorField2D* _getVectorFieldPtr(int time) const;

	bool _insideGrid(const CVector2D& pos) const;

	float _getDeltaT(float z, float stepLen) const;

private:
	CVector3D _RK4(const CVector3D &pos, float fTimeStep, float stepLen, float fDir, bool &bError, bool bNormalize=false) const;

	friend class CAmiraReader;
};

