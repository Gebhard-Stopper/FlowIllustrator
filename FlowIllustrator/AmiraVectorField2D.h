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

	/**
	 *	Helper structure to integrate through 2D, time-dependent vector fields.
	 */
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
	/**
	 *	Load an amira mesh file (*.am).
	 *
	 *	@param strFileName The file name as char array.
	 *
	 *	@remarks This function internally uses the loading function of a class derived from CBasicFileReader, and pointet to by m_pFileRead. 
	 */
	bool LoadAmiraFile(const char* strFileName);

protected:
	/**
	 *	Initializes the parameters of the vector field, represented by this class.
	 *
	 *	@param rcDomain	The two dimensional rectangle defining the domain of the vector field.
	 *	@param fExtentZ	The extent of the domain in z-direction. In a 2D, time-dependent vector field, this is the time span in secornds.
	 *	@param nSamplesX Number of samples in x-direction.
	 *	@param nSamplesY Number of samples in y-direction
	 *	@param nSamplesZ Number of time steps.
	 *	@param pData Pointer to the actual data.	
	 */
	void Init(const CRectF &rcDomain, float fExtentZ,  int nSamplesX, int nSamplesY, int nSamplesZ, CAmiraVectorField2D* pData);

	/**
	 *	Performs a Runge-Kutta integration on the current time step of the vector field.
	 *
	 *	@param xOrg	X-coordinate of the starting point of the integration in domain coordinates.
	 *	@param yOrg Y-coordinate of the starting point of the integration in domain coordinates.
	 *	@param numSteps Number of integration steps to be performed.
	 *	@param stepLen The step lenght used by the integrator.
	 *	@param pOutBuff A pointer to an array of numSteps CPointf elements to receive the vertices of the resulting stream line.
	 */
	virtual void integrateRK4(float xOrg, float yOrg, int numSteps, float stepLen, CPointf *pOutBuff) const;

public:
	/**
	 *	Starts Runge-Kutta integration at the specified location in domain space at the current time step.
	 *
	 *	@param pos Starting position of integration in domain space
	 *	@param nNumSteps Maximum number of integration steps. 
	 *	@param stepLen Step length for RK integrator in grid space
	 *	@param bForward If true, forward integration is performed, otherwise backward integration
	 *	@param pOutBuff Pointer to a std::vector to hold the vertices of the resulting stream line in domain space
	 *	@param rcIntegrationDomain Rectangular region in which the integration is performed. can be <= to the domain rectangle of the vector field.
	 */
	void integrateRK4(const CVector3D &pos, int nNumSteps, float stepLen, bool bForward, vector<CPointf> *pOutBuff, const CRectF &rcIntegrationDomain) const;

	/**
	 * Starts streak line integration, based on the RK4 integrator.
	 * 
	 * @param pos Starting position of integration in domain space
	 * @param nNumSteps Maximum number of integration steps. 
	 * @param stepLen Step length for RK integrator in grid space
	 * @param bForward If true, forward integration is performed, otherwise backward integration
	 * @param pOutBuff Pointer to a std::vector to hold the vertices of the resulting stream line in domain space
	 * @param rcIntegrationDomain Rectangular region in which the integration is performed. can be <= to the domain rectangle of the vector field.
	 */
	void integrateStreakLine(const CVector3D &pos, int nNumSteps, float stepLen, bool bForward, vector<CPointf> *pOutBuff, const CRectF &rcIntegrationDomain) const;

	/**
	 * Starts time line integration, based on the RK4 integrator.
	 * 
	 * @param ptSeedLineStart Starting point of the seeding line.
	 * @param ptSeedLineEnd End point of the seeding line.
	 * @param nNumSamples Number of samples along the seeding line, defined by ptSeedLineStart and ptSeedLineEnd.
	 * @param nNumSteps Maximum number of integration steps. 
	 * @param stepLen Step length for RK integrator in grid space
	 * @param bForward If true, forward integration is performed, otherwise backward integration
	 * @param pOutBuff Pointer to a std::vector to hold the vertices of the resulting time line in domain space
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

	/**
	 *	Retrieve the extent in z-direction (temporal).
	 *
	 *	@return The extent in z-direction as float.
	 */
	__inline float GetZMax() const {
		return m_fExtentZ;
	}

protected:
	/**
	 *	Retrieve the bi-linearly interpolated vextor at the specified location.
	 *
	 *	@param x X-component of the vector location in grid coordinates.
	 *	@param y Y-component of the vector location in grid coordinates.
	 *	@param z Z-component of the vector location in grid coordinates. This value is pushed through. Its main use is for stream line and path line integration.
	 *	@param time The time step, at which the vector is to be retrieved.
	 *
	 *	@return A CVector3D with the components ( u(x,y,t), v(x,y,t), z)
	 */
	CVector3D _getVectorAt(float x, float y, float z, float time) const;//Amira

	/**
	 *	Retrieve the bi-linearly interpolated vextor at the specified location.
	 *
	 *	@param x X-component of the vector location in grid coordinates.
	 *	@param y Y-component of the vector location in grid coordinates.
	 *	@param time The time step, at which the vector is to be retrieved.
	 *	
	 *	@return A CVector2D with the components ( u(x,y,t), v(x,y,t) )
	 */
	CVector2D _getVectorAt(float x, float y, float time) const;//Amira

	/**
	 *	@see GetJacobian(float dx, float dy, float t, arma::fmat33 *pJacobian)
	 */
	void _getJacobian(float x, float y, float t, arma::fmat33 *pJacobian) const;

	/**
	 *	@see GetVorticity(float dx, float dy, float time)
	 */
	float _getVorticity(float x, float y, float time) const;

	/**
	 *	Perform one iteration of the streak line computation.
	 *
	 *	@param fStepLen	The step lenght used by the RK4 integrator.
	 *	@param deltaTime The time difference between two integration steps.
	 *	@param fTimeStep The current time step.
	 *	@param bError Indicates if an error occured during RK4 integration.
	 *	@param vertexBuff Pointer to the vertices (particles) of the streak line to be integrated.
	 *	@param nIterations Number of iterations to be performed withput inserting a new particle.
	 *
	 *	@return Returns true, if no errors occured, or false, if e.g. a particle has left the sample grid.
	 *
	 *	@remarks	New particles are inserted into the streak before calling this function into the std::vector referenced by vertexBuff.
	 *				vertexBuff also contains the result of the integration, when this function returns.
	 */
	bool _integrateStreakLine(float fStepLen, float deltaTime, float &fTimeStep, bool &bError, vector<particle> &vertexBuff, int nIterations) const;

	/**
	 *	Retrieves a pointer to the time slice at the current time step.
	 *
	 *	@return a Pointer to a new CVectorField2D, representing the specified time step.
	 *
	 *	@remarks	The returned CVectorField2D holds a pointer into the original data held by this CAmiraVectorField and must not be deleted by the user!
	 *				The m_pData member of the returned CVectorField2D must be set to nullptr, before the CVectorField2D is deleted.
	 */
	CVectorField2D* _getCurrentVectorFieldPtr() const;

	/**
	 *	Retrieves a pointer to the time slice at the specified time step.
	 *
	 *	@param	The time step at which the vector field is to be retrieved.
	 *
	 *	@return a Pointer to a new CVectorField2D, representing the specified time step.
	 *
	 *	@remarks	This function does not validate the specified time step.
	 *				If a negative or too large value is given the behaviour of this function is undefined.
	 *				<BR>
	 *				The returned CVectorField2D holds a pointer into the original data held by this CAmiraVectorField and must not be deleted by the user!
	 *				The m_pData member of the returned CVectorField2D must be set to nullptr, before the CVectorField2D is deleted.
	 */
	CVectorField2D* _getVectorFieldPtr(int time) const;

	/**
	 *	Validates, if a gives position in grid coordinates is inside the sample grid boundaries.
	 *
	 *	@param pos The location to be validated as reference to a CVector2D.
	 *
	 *	@return Returns true, if the specified position is a valid position on the grid of sample locations, therwise false.
	 *
	 *	@remarks Although the position to be validated is specified as a CVector2D, it is interpreted as a point location and not a vector.
	 */
	bool _insideGrid(const CVector2D& pos) const;

	/**
	 *	Computes and returns the temporal difference between two RK4 integration steps through the spatio-temporal domain.
	 *
	 *	@param z Typically set to 1 or 0, in order to get the time delta for a path line or a stream line computation.
	 *	@param stepLen The step length used by the RK4 integrator.
	 */
	float _getDeltaT(float z, float stepLen) const;

private:
	/**
	 *	Performs an actual RK4 integration step.
	 *	
	 *	@param pos The position from which the RK4 integration is started.
	 *	@param fTimeStep The time step from which the RK4 integration is started.
	 *	@param stepLen The step lenght for the RK4 integrator.
	 *	@param fDir The integration direction, typically 1 for forward integration and -1 for backward integration.
	 *	@param bError If true, an error occured during integration, e.g. one of the intermediate results is the zero vector, or one of the components of hte result is infinite.
	 *	@param bNormalize Set this to true, if the result is to be normalized.
	 *
	 *	@return The integration result as CVector3D.
	 */
	CVector3D _RK4(const CVector3D &pos, float fTimeStep, float stepLen, float fDir, bool &bError, bool bNormalize=false) const;

	friend class CAmiraReader;
};

