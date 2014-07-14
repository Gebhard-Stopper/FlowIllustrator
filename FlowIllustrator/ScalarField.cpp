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
#include "ScalarField.h"
#include "Vector2D.h"

#define _USE_MATH_DEFINES 
#include <math.h>


#define SIGMA 2.0f


CScalarField2D::CScalarField2D(void)
	: m_pField(nullptr),CDataField2D( CRectF(0,0,0,0), 0, 0 )
{
	m_bHasMinMax = false;
}

CScalarField2D::CScalarField2D(const CRectF &rcDomain, int nSamplesX, int nSamplesY)
	: CDataField2D( rcDomain, nSamplesX, nSamplesY )
{
	m_pField = new float [m_nSamplesY * m_nSamplesX];
	m_bHasMinMax = false;
}

CScalarField2D::~CScalarField2D(void)
{
	if (m_pField)
		delete [] m_pField;
}

/*
	Returns the bi-linearly interpolated value at the specified coordinate in domain space.

	Parameters:
	dx:		X-component of the domain coordinate
	dy:		Y-component of the domain coordinate

	posD:	Two dimensional point specifying the domain coordinate
*/
float CScalarField2D::GetValue(float dx, float dy) const
{
	static float x, y;

	_getGridCoordinates(dx, dy, x, y);

	return _getValue(x,y);
}

float CScalarField2D::GetValue(const CPointf &posD) const
{
	static float x, y;

	_getGridCoordinates(posD.x, posD.y, x, y);

	return _getValue(x,y);
}

/*
	Returns the bi-linearly interpolated at the specified grid coordinates
	Ensures the coordinates hvae valid values and lie inside the grid.
*/
float CScalarField2D::_getValue(float x, float y) const
{
	unsigned int px = static_cast<int>(x);
	unsigned int py = static_cast<int>(y);

	if ( static_cast<float>(px)==x && static_cast<float>(py)==y ) {
		return m_pField[py*m_nSamplesX + px];
	}

	int px1((px+1 < m_nSamplesX-1)? px+1 : m_nSamplesX-1);
	int py1((py+1 < m_nSamplesY-1)? py+1 : m_nSamplesY-1);

	register float wx(x - px);
	register float wy(y - py);

	register float f1(m_pField[py  * m_nSamplesX + px]);
	register float f2(m_pField[py1 * m_nSamplesX + px]);
	register float f3( m_pField[py  * m_nSamplesX + px1]);
	register float f4(m_pField[py1 * m_nSamplesX + px1]);

	//Bilinear interpolation in space
	/*float dummy1 = f1 * (1.0f - wy) + f2 * wy;
	float dummy2 = f3 * (1.0f - wy) + f4 * wy;

	return dummy1 * (1.0f - wx) + dummy2 * wx;*/
	return (f1 * (1.0f - wy) + f2 * wy) * (1.0f - wx) + (f3 * (1.0f - wy) + f4 * wy) * wx;
}

/*
	Smoothes the scalar field using a gaussian filter
	This implementation uses the property of a gaussian that it is seperable.
	In order to achieve a better performance, the image is rotated by 90° when the 
	convolution in the first dimension is computed. This allows to take better advantage
	of the serial layout of the main memory and reduces the "jumps" between rows otherwise
*/
void CScalarField2D::Smooth(int nKernelHalfSize)
{
	CScalarField2D dummy(m_rcDomain, m_nSamplesX, m_nSamplesY);	//Receives the intermediate (rotated) result after the first step
	
	float *pKernel = _GetGaussKernel(nKernelHalfSize);

	//Step One, convolve in x
	for (int j = 0; j < m_nSamplesY; j++)
	{
		for (int i = 0; i < m_nSamplesX; i++)
		{
			register float sum(0.0f);

			//Loop over kernel
			for (int k = -nKernelHalfSize; k <= nKernelHalfSize; k++)
			{
				//enforces a black boundary
				if (i+k > -1 &&  i+k < m_nSamplesX) // ( j >= 0 && j < m_nSamplesY) // implicitly given
				{
					sum += pKernel[k+nKernelHalfSize] * m_pField[j*m_nSamplesX + i + k];
				}
			}

			//Automatically rotate
			dummy.m_pField[i*m_nSamplesY + j] = sum;
		}
	}

	//Setp 2, convolve in y
	for (int j = 0; j < m_nSamplesX; j++)
	{
		for (int i = 0; i < m_nSamplesY; i++)
		{
			register float sum(0.0f);

			//Loop over kernel
			for (int k = -nKernelHalfSize; k <= nKernelHalfSize; k++)
			{
				//enforces a black boundary
				if (i+k > -1 &&  i+k < m_nSamplesY) // ( j >= 0 && j < m_nSamplesY) // implicitly given
				{
					sum += pKernel[k+nKernelHalfSize] * dummy.m_pField[j*m_nSamplesY + i + k];
				}
			}

			//Automatically rotate back to original position
			m_pField[i*m_nSamplesX + j] = sum;
		}
	}

	delete pKernel;
}

/*
	Computes the kernel for gaussian convolution and returns a pointer to it
*/
float* CScalarField2D::_GetGaussKernel(int nKernelHalfSize)
{
	int kSize = 2 * nKernelHalfSize + 1;
	float *pKernel = new float [ kSize ];

	//fill with values
	float sum = 0.0f;

	for (int k = -nKernelHalfSize; k <= nKernelHalfSize; k++) 
	{
		pKernel[k+nKernelHalfSize] = static_cast<float>((((1.0/(sqrt(2.0*M_PI)*SIGMA))*(exp(-(float)(k*k)/(2.0*SIGMA*SIGMA))))));
		sum += pKernel[k+nKernelHalfSize];
	}

	//Normalize
	for (int i = 0; i < kSize; i++) 
	{
		pKernel[i] /= sum;
	}

	return pKernel;
}

/*
	Performs gradient ascent starting from the specified position at ptStart.
	The result, i.e. the location of the local maximum is returned in ptEnd.
	If the function succeeds the return value is true, otherwise false.

	Parameters:
	ptStart:	Starting point of gradient ascent
	ptEnd:		Location of the local maximum

*/
bool CScalarField2D::GradientAscent(const CPointf& ptStart, CPointf& ptEnd) const
{
	CVector2D pos;
	_getGridCoordinates(ptStart.x, ptStart.y, pos.x, pos.y);

	CVector2D gradient = _getDerivative(pos.x, pos.y);

	static const float step = .1f;

	while ( gradient.abs() > 1e-3)
	{
		if (!_insideGrid(pos)) {
			ptEnd.x = -1;
			ptEnd.y = -1;
			return false;
		}

		pos = pos + (gradient*step);
		gradient = _getDerivative(pos.x, pos.y);
		
	}
	_getDomainCoordinates(pos.x, pos.y, ptEnd.x, ptEnd.y);

	return true;
}

/*
	Returns the derivative at the specified grid location.
	By default this function returns central differences.

	Due to the interpolation this function uses it implicitly returs 
	forward/backward differences at the respective grid boundaries.
*/
CVector2D CScalarField2D::_getDerivative(float x, float y) const
{
	static float step = .5f;
	static float div = 2.0f*step;

	/*float f1(_getValue(x-step, y));
	float f2(_getValue(x+step, y));
	float f3(_getValue(x, y-step));
	float f4(_getValue(x, y+step));

	return CVector2D( (f2-f1)/div, (f4-f3)/div );*/

	return CVector2D( (_getValue(x+step, y)-_getValue(x-step, y))/div, (_getValue(x, y+step)-_getValue(x, y-step))/div );
}

/*
	Returns the gradient at the specified domain coordinates

	Parameters:
	dx:	X-component of the domain coordinate
	dy:	Y-component of the domain coordinate
*/
CVector2D CScalarField2D::GetGradient(float dx, float dy)
{
	static float x, y;
	_getGridCoordinates(dx, dy, x, y);
	return _getDerivative(x,y);
}

/*
	Returns an absolute copy of the scalar field
*/
CScalarField2D* CScalarField2D::Abs() const
{
	int numElements = m_nSamplesX*m_nSamplesY;

	CScalarField2D* retVal = new CScalarField2D(m_rcDomain, m_nSamplesX, m_nSamplesY);

	float min =m_pField[0], max =0.0f;
	

	for (int i =0; i < numElements; i++) {
		float curr (fabs(m_pField[i]));
		if (curr > max) max = curr;
		else if(curr < min) min = curr;
		(*retVal)[i] = curr;
	}

	retVal->SetMinMax(min, max);

	return retVal;
}

/*
	Returns true if the specified position in grid coordinates is valid, othervise false.
*/
bool CScalarField2D::_insideGrid(const CVector2D& pos) const
{
	if (pos.x < 0 || pos.y < 0) return false;
	if (pos.x >= m_nSamplesX || pos.y >= m_nSamplesY) return false;

	return true;
}

/*
	Returns true if the specified position in domain coordinates is valid, othervise false.
*/
bool CScalarField2D::InsideDomain(const CVector2D& posD) const
{
	if (posD.x < m_rcDomain.m_Min.x || posD.y < m_rcDomain.m_Min.y) return false;
	if (posD.x > m_rcDomain.m_Max.x || posD.y > m_rcDomain.m_Max.y) return false;

	return true;
}

/*
	Fills the scalar field with zeros.
*/
void CScalarField2D::Zero()
{
	memset(m_pField, 0, m_nSamplesX*m_nSamplesY*sizeof(*m_pField));
}