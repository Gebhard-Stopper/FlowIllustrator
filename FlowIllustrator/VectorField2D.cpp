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
#include "VectorField2D.h"
#include <complex>
#include "Vector3D.h"
#include <iostream>

CVectorField2D::CVectorField2D()
	: CDataField2D( CRectF(0,0,0,0), 0, 0)
{
	m_pData		= nullptr;
	m_bIsMM		= false;
	m_nSamplesX = 0;
	m_nSamplesY = 0;
}

CVectorField2D::CVectorField2D(const CRectF &rcDomain, int nSamplesX, int nSamplesY)
	: CDataField2D( rcDomain, nSamplesX, nSamplesY)
{
	m_pData = new CVector2D[nSamplesX * nSamplesY];
	m_bIsMM = false;
}

CVectorField2D::~CVectorField2D(void)
{
	if (!m_bIsMM)
	{
		if (m_pData)
			delete [] m_pData;
	}
}

//ToDo maybe use SSE to speedup

//do bilinear interpolation
//x and y are in domain coordinates: x in range of [xMin, xMax] ; y in range of [yMin, yMax]
CVector2D CVectorField2D::GetVectorAt(float dx, float dy) const
{
	float x, y;

	_getGridCoordinates(dx, dy, x, y);

	return _getVectorAt(x, y);
}

void CVectorField2D::integrateRK4(float dx, float dy, int numSteps, float stepLen, CPointf *pOutBuff) const
{
	CVector2D domainVec;
	CVector2D currPos;

	pOutBuff[0] = CPointf(dx, dy);

	_getGridCoordinates(dx, dy, currPos.x, currPos.y);

	for (int i = 1; i < numSteps; i++)
	{
		CVector2D v1 (_getVectorAt(currPos.x, currPos.y));
		
		CVector2D v2 (currPos + (v1*(stepLen/2.0f)));
		v2 = _getVectorAt(v2.x, v2.y);

		CVector2D v3(currPos + (v2*(stepLen/2.0f)));
		v3 = _getVectorAt(v3.x, v3.y);

		CVector2D v4(currPos + (v3*stepLen));
		v4 = _getVectorAt(v4.x, v4.y);

		currPos += (v1/6.0f + v2/3.0f + v3/3.0f + v4/6.0f)*stepLen;

		_getDomainCoordinates(currPos.x, currPos.y, domainVec.x, domainVec.y);

		pOutBuff[i] = domainVec;
	}
}

CRITICAL_POINT_TYPE CVectorField2D::GetCriticalPointType(const CPointf& point) const
{
	arma::fmat22 J;
	GetJacobian(point.x, point.y, &J);

	std::complex<double> c( static_cast<double>( pow((J(1,1)-J(0,0))/2.0, 2) + J(0,1)*J(1,0) - J(0,0)*J(1,1)) );
	c = std::sqrt(c);

	std::complex<double> eig1, eig2;

	eig1 = (J(0,0) + J(1,1))/2.0 + c;
	eig2 = (J(0,0) + J(1,1))/2.0 - c;

	//we have an imaginary number
	if ( fabs(c.imag()) > EPSILON) // Do we have a complex result?
	{
		double fReal = eig1.real();

		if ( fReal > EPSILON  ) //null
			return REPELLING_FOCUS;
		if( fReal < (-EPSILON) )
			return ATTRACTING_FOCUS;

		return CENTER;
	}

	if (eig1.real() < (-EPSILON) && eig2.real() < (-EPSILON))
		return ATTRACTING_SADDLE;

	if (eig1.real() > EPSILON && eig2.real() > EPSILON)
		return REPELLING_SADDLE;

	if ( (eig1.real() > EPSILON && eig2.real() < (-EPSILON)) || (eig1.real() < (-EPSILON) && eig2.real() > EPSILON))
		return CENTER;

	return NONE; //could not be classified
}

/*
	ToDo: Imaginary numbers!
*/

//Wenn sample distanz in x und y richtung unterschiedlich ist:
//Beim laden darauch achten, wenn dx, dy unterschiedlich sind, das kleinere = 1, das größere entsprechend größer als e
//das selbe gilt für dt (vermutlich)

//Wenn wir exportieren, dann muss dx und dy wieder in die richtige bounding box eingepasst werden

//

void CVectorField2D::GetJacobian(float dx,float dy, arma::fmat22 *pJacobian) const
{
	//Build jacobian
	/*
		U_x  U_y 
		V_x  V_y 
	*/

	static float x, y;

	const static float delta = 0.5f;

	_getGridCoordinates(dx, dy, x, y);

	//Central differences (spatial components)
	CVector2D p1(_getVectorAt(x + delta, y));
	CVector2D p2(_getVectorAt(x - delta, y));

	CVector2D p3(_getVectorAt(x, y + delta));
	CVector2D p4(_getVectorAt(x, y - delta));

	(*pJacobian)(0,0) = (p1.x - p2.x)/(2.0f*delta);	//U_x
	(*pJacobian)(0,1) = (p3.x - p4.x)/(2.0f*delta);	//U_y

	(*pJacobian)(1,0) = (p1.y - p2.y)/(2.0f*delta);	//V_x
	(*pJacobian)(1,1) = (p3.y - p4.y)/(2.0f*delta);	//V_y
}
	
float CVectorField2D::GetVorticity(float dx, float dy) const
{
	//grid coordinates
	static float x, y;

	_getGridCoordinates(dx, dy, x, y);

	return _getVorticity(x,y);
}

float CVectorField2D::_getVorticity(float x, float y) const
{
	const static float delta	= 0.5f;
	const static float div		= 2.0f*delta;

	CVector2D p1(_getVectorAt(x + delta, y));
	CVector2D p2(_getVectorAt(x - delta, y));
	CVector2D p3(_getVectorAt(x, y + delta));
	CVector2D p4(_getVectorAt(x, y - delta));

	return ((p1.y - p2.y) - (p3.x - p4.x))/(div);
}

float CVectorField2D::_getVorticityAbs(float x, float y) const
{
	return fabs(_getVorticity(x,y));
}

CVector2D CVectorField2D::_getVectorAt(float x, float y) const
{
	int px = int( x );
	int py = int( y );

	if (px < 0) px = 0;
	if (px > static_cast<int>(m_nSamplesX-1)) px = static_cast<int>( m_nSamplesX - 1 );
	int px1 = (px == (m_nSamplesX - 1) )? px : px +1;

	//translate to origin and multiply with number of samples in y-direction
	if (py < 0) py = 0;
	if (py > static_cast<int>(m_nSamplesY-1)) py = static_cast<int>( m_nSamplesY - 1 );
	int py1 = (py == (m_nSamplesY -1))? py : py +1;

	//compute weights
	register float wx = px1 - x;
	register float wy = py1 - y;

	register CVector2D *pData(reinterpret_cast<CVector2D *>(m_pData));

	register CVector2D &v1 = pData[py * m_nSamplesX + px];
	register CVector2D &v2 = pData[py1 * m_nSamplesX + px];
	register CVector2D &v3 = pData[py * m_nSamplesX + px1];
	register CVector2D &v4 = pData[py1 * m_nSamplesX + px1];

	//Bilinear interpolation in space
	/*CVector2D dummy1 = v1 * wy + v2 * (1.0f - wy);
	CVector2D dummy2 = v3 * wy + v4 * (1.0f - wy);

	return dummy1 * wx + dummy2 * (1.0f - wx);*/

	return (v1 * wy + v2 * (1.0f - wy)) * wx + (v3 * wy + v4 * (1.0f - wy)) * (1.0f - wx);
}

CScalarField2D* CVectorField2D::GetVorticityField(bool bGetMagnitude) const
{
	CScalarField2D *pVortField = new CScalarField2D(m_rcDomain, m_nSamplesX, m_nSamplesY);

	//Get function pointer to desired vorticity function
	float (CVectorField2D::*pVorticityFunc)(float x, float y) const = (bGetMagnitude)? &CVectorField2D::_getVorticityAbs : &CVectorField2D::_getVorticity;

	static float min, max;

	min = max = (this->*pVorticityFunc)(0,0);
	int nIdx(0);
	for (unsigned int j = 0; j < m_nSamplesY; j++)
	{
		for (unsigned int i = 0; i < m_nSamplesX; i++)
		{
			float curr = (this->*pVorticityFunc)( static_cast<float>(i), static_cast<float>(j) );
			if (curr > max) max = curr;
			else if (curr < min) min = curr;
			(*pVortField)[nIdx++] = curr;
		}
	}
	
	pVortField->SetMinMax(min, max);

	return pVortField;
}

CScalarField2D* CVectorField2D::GetVorticityField(CRectF rect, bool bGetMagnitude) const
{
	//Get function pointer to desired vorticity function
	float (CVectorField2D::*pVorticityFunc)(float x, float y) const = (bGetMagnitude)? &CVectorField2D::_getVorticityAbs : &CVectorField2D::_getVorticity;

	//Min and max grid cells that are contained in the returned vorticity field
	CPointf gridMin = GetClosestSamplePos( CPointf::fromVector2D(rect.m_Min) );
	CPointf gridMax = GetClosestSamplePos( CPointf::fromVector2D(rect.m_Max) );

	//Get the actual domain rect
	CRectF rcActualDomain;
	_getDomainCoordinates(gridMin.x, gridMin.y, rcActualDomain.m_Min.x, rcActualDomain.m_Min.y);
	_getDomainCoordinates(gridMax.x, gridMax.y, rcActualDomain.m_Max.x, rcActualDomain.m_Max.y);

	CScalarField2D *pVortField = new CScalarField2D(	rcActualDomain, 
														static_cast<int>(gridMax.x - gridMin.x), 
														static_cast<int>(gridMax.y - gridMin.y) );

	float min, max;
	min = max = (this->*pVorticityFunc)(0,0);

	for (int j = static_cast<int>(gridMin.y), y=0; j < gridMax.y; j++, y++)
	{
		for (int i = static_cast<int>(gridMin.x), x=0; i < gridMax.x; i++, x++)
		{
			float curr((this->*pVorticityFunc)( static_cast<float>(i), static_cast<float>(j) ));
			if (curr > max) max = curr;
			else if (curr < min) min = curr;

			pVortField->SetAt(x,y, curr);
		}
	}
	
	pVortField->SetMinMax(min, max);

	return pVortField;
}