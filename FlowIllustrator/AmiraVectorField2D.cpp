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

#include "StdAfx.h"
#include "AmiraVectorField2D.h"
#include "amirareader.h"

CAmiraVectorField2D::CAmiraVectorField2D()
{
	m_pFileRead = new CAmiraReader();
}

bool CAmiraVectorField2D::LoadAmiraFile(const char* strFileName)
{
	return reinterpret_cast<CAmiraReader*>(m_pFileRead)->readAmiraFile(strFileName, this);
}


CAmiraVectorField2D::~CAmiraVectorField2D(void)
{
	delete m_pFileRead;
}

//do tri-linear interpolation
//x and y are in domain coordinates: x in range of [xMin, xMax] ; y in range of [yMin, yMax]
CVector2D CAmiraVectorField2D::GetVectorAt(float dx, float dy, float time) const
{
	static float x, y;
	_getGridCoordinates(dx, dy, x, y);

	return _getVectorAt(x, y, time);
}

CVector3D CAmiraVectorField2D::GetVectorAt(float dx, float dy, float z, float time) const
{
	static float x, y;
	_getGridCoordinates(dx, dy, x, y);

	return _getVectorAt(x, y, z, time);
}

CVector2D CAmiraVectorField2D::GetVectorAt(float dx, float dy) const
{
	static float x, y;
	_getGridCoordinates(dx, dy, x, y);

	return _getVectorAt(x, y, static_cast<float>(m_currTimeStep));
}

CVector2D CAmiraVectorField2D::GetVectorAt(const CPointf &point) const
{
	static float x, y;
	_getGridCoordinates(point.x, point.y, x, y);

	return _getVectorAt(x, y, static_cast<float>(m_currTimeStep));
}

void CAmiraVectorField2D::GetJacobian(float dx, float dy, arma::fmat22 *pJacobian) const
{
	//Build jacobian
	/*
		U_x  U_y 
		V_x  V_y 
	*/

	static float x, y;
	const static float delta	= 0.5f;
	const static float div		= 2.0f*delta;

	_getGridCoordinates(dx, dy, x, y);

	//Central differences (spatial components)
	CVector2D p1(_getVectorAt(x + delta, y, static_cast<float>(m_currTimeStep)));
	CVector2D p2(_getVectorAt(x - delta, y, static_cast<float>(m_currTimeStep)));

	CVector2D p3(_getVectorAt(x, y + delta, static_cast<float>(m_currTimeStep)));
	CVector2D p4(_getVectorAt(x, y - delta, static_cast<float>(m_currTimeStep)));

	(*pJacobian)(0,0) = (p1.x - p2.x)/(div);	//U_x
	(*pJacobian)(0,1) = (p3.x - p4.x)/(div);	//U_y

	(*pJacobian)(1,0) = (p1.y - p2.y)/(div);	//V_x
	(*pJacobian)(1,1) = (p3.y - p4.y)/(div);	//V_y
}

__inline void CAmiraVectorField2D::GetJacobian(float dx, float dy, float t, arma::fmat33 *pJacobian) const
{
	static float x, y;
	
	_getGridCoordinates(dx, dy, x, y);

	_getJacobian(x,y,t,pJacobian);
}

__inline void CAmiraVectorField2D::_getJacobian(float x, float y, float t, arma::fmat33 *pJacobian) const
{
	//Build jacobian
	/*
		U_x  U_y  U_t
		V_x  V_y  V_t
		0    0    0
	*/

	const static float delta	= 0.5f;
	const static float div		= 2.0f*delta;

	//Central differences (spatial components)
	CVector2D p1(_getVectorAt(x + delta, y, t));
	CVector2D p2(_getVectorAt(x - delta, y, t));

	CVector2D p3(_getVectorAt(x, y + delta, t));
	CVector2D p4(_getVectorAt(x, y - delta, t));

	//Central differences (temporal components)
	CVector2D w1 (_getVectorAt(x, y, t + delta));
	CVector2D w2 (_getVectorAt(x, y, t - delta));

	(*pJacobian)(0,0) = (p1.x - p2.x)/(div);	//U_x
	(*pJacobian)(0,1) = (p3.x - p4.x)/(div);	//U_y
	(*pJacobian)(0,2) = (w1.x - w2.x)/(div);	//U_t

	(*pJacobian)(1,0) = (p1.y - p2.y)/(div);	//V_x
	(*pJacobian)(1,1) = (p3.y - p4.y)/(div);	//V_y
	(*pJacobian)(1,2) = (w1.y - w2.y)/(div);	//V_t
}

float CAmiraVectorField2D::GetVorticity(float dx, float dy, float t) const
{
	//grid coordinates
	static float x, y;
	_getGridCoordinates(dx, dy, x, y);

	return _getVorticity(x,y,t);
}

float CAmiraVectorField2D::_getVorticity(float x, float y, float t) const
{
	const static float delta	= 0.5f;
	const static float div		= 2.0f*delta;
	
	CVector2D p1 = _getVectorAt(x + delta, y, t);
	CVector2D p2 = _getVectorAt(x - delta, y, t);
	CVector2D p3 = _getVectorAt(x, y + delta, t);
	CVector2D p4 = _getVectorAt(x, y - delta, t);

	return ((p1.y - p2.y) - (p3.x - p4.x))/(div);
}

bool CAmiraVectorField2D::_insideGrid(const CVector2D& pos) const
{
	if (pos.x < 0 || pos.y < 0) return false;
	if (pos.x >= m_nSamplesX || pos.y >= m_nSamplesY) return false;

	return true;
}

float CAmiraVectorField2D::GetVorticity(float x, float y) const
{
	return GetVorticity(x, y, static_cast<float>(m_currTimeStep));
}

CVector3D CAmiraVectorField2D::_getVectorAt(float x, float y,  float z, float time) const
{
	int px = static_cast<int>( x );
	if (px < 0) px = 0;
	if (px > static_cast<int>(m_nMaxIdxX)) px = static_cast<int>(m_nMaxIdxX);
	int px1 = (px == static_cast<int>(m_nMaxIdxX) )? px : px +1;

	//translate to origin and multiply with number of samples in y-direction
	int py = static_cast<int>( y );
	if (py < 0) py = 0;
	if (py > static_cast<int>(m_nMaxIdxY)) py = static_cast<int>(m_nMaxIdxY);
	int py1 = (py == static_cast<int>(m_nMaxIdxY))? py : py +1;
	
	//get floor and ceil for the timestep to interpolate
	int tx = static_cast<int>( time );
	if (tx < 0) tx = 0;
	if (tx > static_cast<int>(m_nMaxTimestep)) tx = static_cast<int>(m_nMaxTimestep);
	int tx1 = (tx == static_cast<int>(m_nMaxTimestep))? tx : tx + 1;

	//compute weights
	const register float wx = x - px;
	const register float wy = y - py;
	const register float wt = time - tx;

	CVector2D v1, v2, v3, v4;

	//Get the two timesteps
	const CVector2D* vecField = GetFrame(tx);		//t-1

	if (wt != 0.0f)
	{
		const CVector2D* vecField2 = GetFrame(tx1);		//t+1

		//interpolate in time
		v1 = vecField[py * m_nSamplesX + px] * (1.0f-wt) + vecField2[py * m_nSamplesX + px] * wt;
		v2 = vecField[py1 * m_nSamplesX + px] * (1.0f-wt) + vecField2[py1 * m_nSamplesX + px] * wt;

		v3 = vecField[py * m_nSamplesX + px1] * (1.0f-wt) + vecField2[py * m_nSamplesX + px1] * wt;
		v4 = vecField[py1 * m_nSamplesX + px1] * (1.0f-wt) + vecField2[py1 * m_nSamplesX + px1]* wt;
	}
	else
	{
		v1 = vecField[py * m_nSamplesX + px];
		v2 = vecField[py1 * m_nSamplesX + px];
		v3 = vecField[py * m_nSamplesX + px1];
		v4 = vecField[py1 * m_nSamplesX + px1];
	}

	//Bilinear interpolation in space
	CVector2D dummy1 = v1 * (1.0f - wy) + v2 * wy;
	CVector2D dummy2 = v3 * (1.0f - wy) + v4 * wy;

	return CVector3D( dummy1 * (1.0f - wx) + dummy2 * wx, z);
}

CVector2D CAmiraVectorField2D::_getVectorAt(float x, float y, float time) const
{
	CVector3D dummy = _getVectorAt(x,y,0,time);
	return CVector2D(dummy.x, dummy.y);
}

void CAmiraVectorField2D::Init(const CRectF &rcDomain, float fExtentZ, int nSamplesX, int nSamplesY, int nSamplesZ, CAmiraVectorField2D* pData)
{
	CDataField2D::Init(rcDomain, nSamplesX, nSamplesY);

	m_nMaxIdxX		= nSamplesX-1;
	m_nMaxIdxY		= nSamplesY-1;
	m_fExtentZ		= fExtentZ;
	m_bIsMM			= true;
	m_numTimeSteps	= nSamplesZ;
	m_nMaxTimestep	= m_numTimeSteps-1;
	m_currTimeStep	= 0;
	m_pData			= reinterpret_cast<CMathVector*>(pData);
}

CVectorField2D* CAmiraVectorField2D::_getCurrentVectorFieldPtr() const
{
	return _getVectorFieldPtr(m_currTimeStep);
}

CVectorField2D* CAmiraVectorField2D::_getVectorFieldPtr(int time) const
{
	CVectorField2D *pDummy = new CVectorField2D();
	pDummy->m_nSamplesX = m_nSamplesX;
	pDummy->m_nSamplesY = m_nSamplesY;

	pDummy->m_rcDomain = m_rcDomain;

	pDummy->m_pData = &m_pData[m_nSamplesX*m_nSamplesY*time * sizeof(CVector2D)];

	return pDummy;
}

CScalarField2D* CAmiraVectorField2D::GetVorticityField(bool bGetMagnitude, float time) const
{
	CVectorField2D *pDummy;

	if (time < 0)
		pDummy = _getCurrentVectorFieldPtr();
	else
		pDummy = _getVectorFieldPtr(static_cast<int>(time));

	CScalarField2D *pRetVal = pDummy->GetVorticityField(bGetMagnitude);
	
	pDummy->m_pData = nullptr;
	delete pDummy;

	return pRetVal;
}

CScalarField2D* CAmiraVectorField2D::GetVorticityField(CRectF rect, bool bGetMagnitude, float time) const
{
	CVectorField2D *pDummy;

	if (time < 0)
		pDummy = _getCurrentVectorFieldPtr();
	else
		pDummy = _getVectorFieldPtr(static_cast<int>(time));

	CScalarField2D *pRetVal = pDummy->GetVorticityField(rect, bGetMagnitude);
	
	pDummy->m_pData = nullptr;
	delete pDummy;

	return pRetVal;
}

void CAmiraVectorField2D::integrateRK4(float xOrg, float yOrg, int numSteps, float stepLen, CPointf *pOutBuff) const
{
	CVectorField2D *pDummy = _getCurrentVectorFieldPtr();
	pDummy->integrateRK4(xOrg, yOrg, numSteps, stepLen, pOutBuff);

	pDummy->m_pData = nullptr;
	delete pDummy;
}

void CAmiraVectorField2D::integrateRK4(const CVector3D &pos, int nNumSteps, float stepLen, bool bForward, vector<CPointf> *pOutBuff, const CRectF &rcIntegrationDomain) const
{
	CVector3D currPos;
	CPointf trace;

	CRectF rcDomain(rcIntegrationDomain);

	//Ensure the integration domain does not exceed the real domain
	if (rcDomain.m_Min.x < m_rcDomain.m_Min.x) rcDomain.m_Min.x = m_rcDomain.m_Min.x;
	if (rcDomain.m_Min.y < m_rcDomain.m_Min.y) rcDomain.m_Min.y = m_rcDomain.m_Min.y;
	if (rcDomain.m_Max.x > m_rcDomain.m_Max.x) rcDomain.m_Max.x = m_rcDomain.m_Max.x;
	if (rcDomain.m_Max.y > m_rcDomain.m_Max.y) rcDomain.m_Max.y = m_rcDomain.m_Max.y;

	pOutBuff->reserve(nNumSteps);

	_getGridCoordinates(pos.x, pos.y, currPos.x, currPos.y);
	
	pOutBuff->push_back( CPointf(pos.x, pos.y) );
	float fTimeStep		= static_cast<float>(m_currTimeStep);
	bool bError			= false;
	float dir			= (bForward)? 1.0f : -1.0f;
	const float deltaTime = _getDeltaT(pos.z, stepLen);

	for (int i =1; i < nNumSteps; i++)
	{
		currPos.z	= pos.z;
		currPos		= _RK4(currPos, fTimeStep, stepLen, dir, bError);

		_getDomainCoordinates(currPos.x, currPos.y, trace.x, trace.y);

		if (bError || !rcDomain.PtInRect(trace.x, trace.y) ||  fTimeStep >= m_numTimeSteps) break;

		if (bForward) {
			pOutBuff->push_back( trace );
		} else {
			pOutBuff->insert(pOutBuff->end()-pOutBuff->size(), trace );
		}

		fTimeStep += deltaTime;
	}

	pOutBuff->shrink_to_fit();
}

void CAmiraVectorField2D::integrateTimeLine(const CPointf &ptSeedLineStart, const CPointf &ptSeedLineEnd, int nNumSamples, int nNumSteps, float stepLen, bool bForward, vector<CPointf> *pOutBuff, const CRectF &rcIntegrationDomain) const
{
	CVector3D currPos;
	CRectF rcDomain(rcIntegrationDomain);

	//Ensure the integration domain does not exceed the real domain
	if (rcDomain.m_Min.x < m_rcDomain.m_Min.x) rcDomain.m_Min.x = m_rcDomain.m_Min.x;
	if (rcDomain.m_Min.y < m_rcDomain.m_Min.y) rcDomain.m_Min.y = m_rcDomain.m_Min.y;
	if (rcDomain.m_Max.x > m_rcDomain.m_Max.x) rcDomain.m_Max.x = m_rcDomain.m_Max.x;
	if (rcDomain.m_Max.y > m_rcDomain.m_Max.y) rcDomain.m_Max.y = m_rcDomain.m_Max.y;

	for (int i=0; i < nNumSamples; i++)
	{
		CPointf &currPt ( (*pOutBuff)[i] );
		CVector3D pos ( CPointf::toVector2D( currPt ), 1.0f );

		_getGridCoordinates(pos.x, pos.y, currPos.x, currPos.y);
	
		float fTimeStep		= static_cast<float>(m_currTimeStep);
		bool bError			= false;
		float dir			= (bForward)? 1.0f : -1.0f;
		const float deltaTime = _getDeltaT(pos.z, stepLen);

		for (int i =0; i < nNumSteps; i++)
		{
			currPos.z	= pos.z;
			currPos		= _RK4(currPos, fTimeStep, stepLen, dir, bError);

			fTimeStep += deltaTime;
		}

		_getDomainCoordinates(currPos.x, currPos.y, currPt.x, currPt.y);

		if ( currPt.x > rcDomain.m_Max.x ) {
			currPt.x = rcDomain.m_Max.x;
		} else if ( currPt.x < rcDomain.m_Min.x ) {
			currPt.x = rcDomain.m_Min.x;
		}

		if ( currPt.y > rcDomain.m_Max.y ) {
			currPt.y = rcDomain.m_Max.y;
		} else if ( currPt.y < rcDomain.m_Min.y ) {
			currPt.y = rcDomain.m_Min.y;
		}

		if (bError ||  fTimeStep >= m_numTimeSteps) break;
	}

}

float CAmiraVectorField2D::_getDeltaT(float z, float stepLen) const
{
	float f2 = z+(z * (stepLen/2.0f));
	float f4 = z+(z * stepLen);

	float factor = GetSamplesPerSecond()/GetSamplesPerUnitX();

	return (z + (f2 + f2)*2.0f + f4)/6.0f*stepLen * factor;
}

void CAmiraVectorField2D::integrateStreakLine(const CVector3D &pos, int nNumSteps, float stepLen, bool bForward, vector<CPointf> *pOutBuff, const CRectF &rcIntegrationDomain) const
{
	CVector3D currPos, origin;
	CPointf trace;
	CRectF rcDomain(rcIntegrationDomain);

	//Ensure the integration domain does not exceed the real domain
	if (rcDomain.m_Min.x < m_rcDomain.m_Min.x) rcDomain.m_Min.x = m_rcDomain.m_Min.x;
	if (rcDomain.m_Min.y < m_rcDomain.m_Min.y) rcDomain.m_Min.y = m_rcDomain.m_Min.y;
	if (rcDomain.m_Max.x > m_rcDomain.m_Max.x) rcDomain.m_Max.x = m_rcDomain.m_Max.x;
	if (rcDomain.m_Max.y > m_rcDomain.m_Max.y) rcDomain.m_Max.y = m_rcDomain.m_Max.y;

	vector<particle> vertexBuff;

	vertexBuff.reserve(nNumSteps+1);
	pOutBuff->reserve(nNumSteps+1);

	_getGridCoordinates(pos.x, pos.y, origin.x, origin.y);
	origin.z	= pos.z;

	float fTimeStep		= static_cast<float>(m_currTimeStep);
	bool bError			= false;
	float dir			= (bForward)? 1.0f : -1.0f;
	bool bContinue		= true;

	float deltaTime = _getDeltaT(origin.z, stepLen);

	vector <int> refineIdx;

	for (int i =0; i < nNumSteps; i++)
	{
		vertexBuff.push_back(particle(origin, fTimeStep));
		bContinue = _integrateStreakLine(stepLen, deltaTime, fTimeStep, bError, vertexBuff, 1);
		if ( fTimeStep >= (m_numTimeSteps-1) || !bContinue) break;
	}

	vertexBuff.push_back( particle(origin, 0) ); //Insert origin

	for (auto iter = vertexBuff.rbegin(); iter != vertexBuff.rend(); ++iter)
	{
		_getDomainCoordinates((*iter).pos.x, (*iter).pos.y, trace.x, trace.y);
		pOutBuff->push_back(trace);
	}

	pOutBuff->shrink_to_fit();
}


bool CAmiraVectorField2D::_integrateStreakLine(float fStepLen, float deltaTime, float &fTimeStep, bool &bError, vector<particle> &vertexBuff, int nIterations) const
{
	bool bresult = true;
	for (int i=0; i < nIterations && bresult; i++)
	{
		for (auto iter = vertexBuff.begin(); iter != vertexBuff.end(); ++iter)
		{
			iter->pos = _RK4(iter->pos, fTimeStep, fStepLen, 1, bError);

			if (iter->pos.x <0) iter->pos.x = 0.0f;
			else if ( iter->pos.x > m_nMaxIdxX) iter->pos.x = static_cast<float>(m_nMaxIdxX);

			if (iter->pos.y < 0) iter->pos.y = 0.0f;
			else if (iter->pos.y > m_nMaxIdxY) iter->pos.y = static_cast<float>(m_nMaxIdxY);
			if(bError) 
			{
				bresult = false;
				break;
			}
		}

		fTimeStep += deltaTime;
	}

	return bresult;
}

CVector3D CAmiraVectorField2D::_RK4(const CVector3D &pos, float fTimeStep, float stepLen, float fDir, bool &bError, bool bNormalize) const
{
	CVector3D retVal;

	CVector3D v1 = _getVectorAt(pos.x, pos.y, pos.z, fTimeStep) * fDir ;

	if (v1.abs() > 1e-15 && isFinite(v1.x) && isFinite(v1.y))
	{
		if (bNormalize) v1.Normalize();

		CVector3D v2 (pos + (v1*(stepLen/2.0f)));
		v2 = _getVectorAt(v2.x, v2.y, v2.z, fTimeStep) * fDir;
		if (bNormalize) v2.Normalize();

		CVector3D v3(pos + (v2*(stepLen/2.0f)));
		v3 = _getVectorAt(v3.x, v3.y, v3.z, fTimeStep) * fDir;
		if (bNormalize) v3.Normalize();

		CVector3D v4(pos + (v3*stepLen));
		v4 = _getVectorAt(v4.x, v4.y, v4.z, fTimeStep) * fDir;
		if (bNormalize) v4.Normalize();

		retVal = pos + ((v1 + (v2 + v3)*2.0f + v4)/6.0f)*stepLen;

		static CVector3D ZeroVector (0.0f, 0.0f, 0.0f);
		bError = (v1 == ZeroVector || v2 == ZeroVector || v3 == ZeroVector || v4 == ZeroVector
			  || !isFinite(retVal.x) || !isFinite(retVal.y) );

		return retVal;
	}

	bError = true;
	return pos;
}

CRITICAL_POINT_TYPE CAmiraVectorField2D::GetCriticalPointType(const CPointf& point) const
{
	CVectorField2D *pDummy = _getCurrentVectorFieldPtr();
	CRITICAL_POINT_TYPE nType = pDummy->GetCriticalPointType(point);

	pDummy->m_pData = nullptr;
	delete pDummy;

	return nType;
}