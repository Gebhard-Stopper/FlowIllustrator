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
#include "DataField.h"
#include "RectF.h"
#include "Vector2D.h"

/**
 *	This class derived from CDataField2D represents a non-timedependent, two-dimensional scalar field.
 *	It allows to to manipulate and retrieve values within its domain.
 *
 *	A CDataField2D is defined by a rectangular domain, which contains X*Y floating point samples.
 */
class CScalarField2D : public CDataField2D
{
protected:
	float	*m_pField;		/**< Pointer to the scalar data */
	bool	 m_bHasMinMax;	/**< Flag, which indicates if the minimum and maximum value contained in this CDataField2D are known */
	float	 m_fMin;		/**< The minimum value present in this CDataField2D. Only contais a valid value, if m_bHasMinMax is true */
	float	 m_fMax;		/**< The maximum value present in this CDataField2D. Only contais a valid value, if m_bHasMinMax is true */

public:
	/**
	 *	Construct a new, empty CScalarField2D.
	 */
	CScalarField2D(void);

	/**
	 *	Construct a new CScalarField2D.
	 *	@param rcDomain	Rectangular domain of the scalar field.
	 *	@param nSamplesX Number of samples in X-direction.
	 *	@param nSamplesY Number of samples in Y-direction.
	 */
	CScalarField2D(const CRectF &rcDomain, int nSamplesX, int nSamplesY);

	/**
	 *	Destroy the CDataField2D with all ots data.
	 */
	~CScalarField2D(void);

public:
	/**
	 *	Set a scalar value at the specified sample position.
	 *
	 *	@param x The X-component of the sample location, in grid coordinates.
	 *	@param y The Y-Component of the sample location, in grid coordinates.
	 *	@param val The new value.
	 *
	 *	@remarks	This function does not verify the specified location.
	 *				Specifying alocation outside the grid may cause undefined behaviour.
	 */
	__inline void SetAt(int x, int y, float val) {
		m_pField[y*m_nSamplesX + x] = val;
	}

	/**
	 *	Retrieve a scalar value at the specified sample position.
	 *
	 *	@param x The X-component of the sample location, in grid coordinates.
	 *	@param y The Y-Component of the sample location, in grid coordinates.
	 *
	 *	@return The float value at the specified location.
	 *
	 *	@remarks	This function does not verify the specified location.
	 *				Specifying alocation outside the grid may cause undefined behaviour.
	 */
	__inline float GetAt(int x, int y) const {
		return m_pField[y*m_nSamplesX + x];
	}

	/**
	 *	Retrieve a pointer to the beginning of the scalar field.
	 *
	 *	@return A float pointer to the beginning of the scalar field.
	 */
	__inline float* GetData() const {
		return m_pField;
	}

	/**
	 *	Retrieve the scalar value at the specified location.
	 *
	 *	@param dx X-component of the value to be retrieved, in domain coordinates.
	 *	@param dy Y-component of the value to be retrieved, in domain coordinates.
	 *
	 *	@return The floating point value at the specified location.
	 *
	 *	@remarks	If the specified location is an exact sample location, the value at the respective sample poitn is returned.
	 *				Otherwise, the returned value is obtained via bi-linear interpolation.
	 */
	float GetValue(float dx, float dy) const;

	/**
	 *	Retrieve the scalar value at the specified location.
	 *
	 *	@param posD The location of the value to be retrieved as a reference to a CPointf, in domain coordinates.
	 *
	 *	@return The floating point value at the specified location.
	 *
	 *	@remarks	If the specified location is an exact sample location, the value at the respective sample poitn is returned.
					Otherwise, the returned value is obtained via bi-linear interpolation.
	 */
	float GetValue(const CPointf &posD) const;

	/**
	 *	Retrieve the minimum value present in this CScalarField2D.
	 *
	 *	@return The minimum value as float.
	 *
	 *	@remarks	The returned value is only valid, if m_bHasMinMax is true.
	 */
	float GetMinValue() const { return m_fMin; }

		/**
	 *	Retrieve the maximum value present in this CScalarField2D.
	 *
	 *	@return The maximum value as float.
	 *
	 *	@remarks	The returned value is only valid, if m_bHasMinMax is true.
	 */
	float GetMaxValue() const { return m_fMax; }

	/**
	 *	Applies a Gaussian smoothing operation this entire CScalarField2D.
	 *	
	 *	@param nKernelHalfSize The kernel halfsize of the filterkernel, used to smooth this CScalarField2D.
	 *
	 *	@remarks	The size of the actual filter kernel is 2*nKernelHalfSize + 1.
	 *				Greater filter kernels cause a stronger smoothing/bluring of the CScalarField2D.
	 *				However, this can also cause a considerable drop in performance.
	 */
	void Smooth(int nKernelHalfSize);

	/**
	 *	Perform gradient ascent to find a local maximum.
	 *
	 *	@param ptStart Reference to CPointf to a location in domain space, from where the gradient ascent is started.
	 *	@param ptEnd Reference to a CPointf to receive the location of the local maximum in domain coordinates.
	 *
	 *	@return This function returns true, if a local maximum was found, otherwise false.
	 *
	 *	@remarks	This function returns true if the length of the local gradient vector is less than 1e-3.
	 *				If the search for a local maximum crosses grid/domain boundaries, the search is aborted, and false is returned.
	 */
	bool GradientAscent(const CPointf& ptStart, CPointf& ptEnd) const;

	/**
	 *	Retrieve the gradient vector at the specified location.
	 *
	 *	@param dx X-component of the location of the gradient vector.
	 *	@param dy Y-component of the location of the gradient vector.
	 *
	 *	@return The gradient vctor as CVector2D.
	 */
	CVector2D GetGradient(float dx, float dy);

	/**
	 *	Specify the minimum and maximum value for this CScalarField2D.
	 *
	 *	@param min The new minimum value.
	 *	@parammax The new maximum value.
	 */
	void SetMinMax(float min, float max) { m_fMin = min; m_fMax = max; m_bHasMinMax = true; }

	/**
	 *	Retrieve a copy of this CScalarField2D with absolute values.
	 *
	 *	@return The new CScalarField2D.
	 *
	 *	@remarks The user has to delete the returned CScalarField2D, if no longer needed.
	 */
	CScalarField2D* Abs() const; 

	/**
	 *	Verify a location is inside the domain of this CScalarField2D.
	 *
	 *	@param posD The location to verify in domain coordinates.
	 *
	 *	@return This function returns true, if the specified location is a valid location inside the domain of this CScalarField2D, otherwise false.
	 */
	bool InsideDomain(const CVector2D& posD) const;

	/**
	 *	Set all sample values of this CScalarField2D to 0.
	 */
	void Zero();

	__inline float& operator [] (int nIndex) {
		return m_pField[nIndex];
	}

private:
	float* _GetGaussKernel(int nKernelHalfSize);
	float _getValue(float x, float y) const;
	CVector2D _getDerivative(float x, float y) const;
	bool _insideGrid(const CVector2D& pos) const;
};

