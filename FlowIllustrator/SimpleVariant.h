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
#include <string>
#include <exception>
#include "Pointf.h"
#include "FloatColor.h"
#include <memory>

using namespace std;

/**
 *	Enumeration indicating the type of data, held by a SIMPLE_VARIANT.
 */
enum VARIANT_TYPE
{
	SVT_NONE		= 0,	/**< SIMPLE_VARIANT holds no, or an undefined value. */
	SVT_INTEGER		= 1,	/**< SIMPLE_VARIANT holds a value of type int. */
	SVT_UINTEGER	= 2,	/**< SIMPLE_VARIANT holds a value of type unsigned int. */
	SVT_FLOAT		= 3,	/**< SIMPLE_VARIANT holds a value of type float. */
	SVT_DOUBLE		= 4,	/**< SIMPLE_VARIANT holds a value of type double. */
	SVT_BOOLEAN		= 5,	/**< SIMPLE_VARIANT holds a value of type bool. */
	SVT_POINT		= 6,	/**< SIMPLE_VARIANT holds a value of type CPointf. */
	SVT_VECTOR2D	= 7,	/**< SIMPLE_VARIANT holds a value of type CVector2D. */
	SVT_FLOATCOL	= 8,	/**< SIMPLE_VARIANT holds a value of type floatColor. */
};

#define __SIMPLE_VARIANT
#define __SIMPLE_VARIANT1

/**
 *	Variant type to hold data of various data types.
 *
 *	@see CPointf
 *	@see CVector2D
 *	@see floatColor_s
 *	@see CSimpleVariant
 *	@see VARIANT_TYPE
 */
struct SIMPLE_VARIANT
{
	SIMPLE_VARIANT() {
		nType = SVT_NONE;
	}

	VARIANT_TYPE	nType;			/**< VARIANT_TYPE indicating thy type of the value held by a SIMPLE_VARIANT. */
	union {
		int				iVal;		/**< Data is accessed as int. */
		unsigned int	uiVal;		/**< Data is accessed as unsigned int. */
		float			fVal;		/**< Data is accessed as float. */
		double			dblVal;		/**< Data is accessed as double. */
		bool			bVal;		/**< Data is accessed as boolean. */
		union {
			struct {
				CPointf		    point;	/**< Data is accessed as CPointf. */
			};
			struct {
				CVector2D	   vec2D;	/**< Data is accessed as CVector2D. */
			};
			struct {
				floatColor     color;	/**< Data is accessed as floatColor. */
			};
		}__SIMPLE_VARIANT1;

	} __SIMPLE_VARIANT;
};


/**
 *	Variant class to hold data of various data types.
 *	This class inherits from SIMPLE_VARIANT and extents its functionality
 *	by providing functions to easily access and cast the data stored in the SIMPLE_VARIANT data structure.
 *
 *	Based on the data type of the assigned data, this calss ensures that the correct VARIANT_TYPE is set.
 *
 *	@see CPointf
 *	@see CVector2D
 *	@see floatColor_s
 *	@see CSimpleVariant
 *	@see VARIANT_TYPE_S
 */
class CSimpleVariant : 
	public SIMPLE_VARIANT
{
public:
	/**
	 *	Construct a new CSimpleVariant.
	 *	
	 *	@remarks nType is set to SVT_NONE.
	 */
	__inline CSimpleVariant() {
		nType = SVT_NONE;
	}

	/**
	 *	Construct a new CSimpleVariant.
	 *	
	 *	@param intVal Value of type integer.
	 */
	__inline CSimpleVariant(int intVal) {
		*this = intVal;
	}

	/**
	 *	Construct a new CSimpleVariant.
	 *	
	 *	@param uintVal Value of type unsigned integer.
	 */
	__inline CSimpleVariant(unsigned int uintVal) {
		*this = uintVal;
	}

	/**
	 *	Construct a new CSimpleVariant.
	 *	
	 *	@param floatVal Value of type float.
	 */
	__inline CSimpleVariant(float floatVal) {
		*this = floatVal;
	}

	/**
	 *	Construct a new CSimpleVariant.
	 *	
	 *	@param doubleVal Value of type double.
	 */
	__inline CSimpleVariant(double doubleVal) {
		*this = doubleVal;
	}

	/**
	 *	Construct a new CSimpleVariant.
	 *	
	 *	@param boolVal Value of type bool.
	 */
	__inline CSimpleVariant(bool boolVal) {
		*this = boolVal;
	}

	/**
	 *	Construct a new CSimpleVariant.
	 *	
	 *	@param point Value of type CPointf.
	 */
	__inline CSimpleVariant(const CPointf &point) {
		*this = point;
	}

	/**
	 *	Construct a new CSimpleVariant.
	 *	
	 *	@param vec Value of type CVector2D.
	 */
	__inline CSimpleVariant(const CVector2D &vec) {
		*this = vec;
	}

	/**
	 *	Construct a new CSimpleVariant.
	 *	
	 *	@param color Value of type floatColor.
	 */
	__inline CSimpleVariant(const floatColor &color) {
		*this = color;
	}

	/**
	 *	Destroys the CSimpleVariant object and its associated value.
	 */
	__inline ~CSimpleVariant(void) {};

public:
	/**
	 *	Assigns a value of type CSimpleVariant and sets nType to the corresponding value.
	 *	
	 *	@param varVal Value of type CSimpleVariant.
	 */
	__inline const CSimpleVariant& CSimpleVariant::operator = (const CSimpleVariant& varVal) {
		memcpy(this, &varVal, sizeof(SIMPLE_VARIANT));
		return *this;
	}

	/**
	 *	Assigns a value of type int and sets nType to the corresponding value.
	 *	
	 *	@param val Value of type int.
	 */
	__inline const CSimpleVariant& CSimpleVariant::operator = (int val) {
		nType = SVT_INTEGER; 
		iVal = val;
		return *this;
	}

	/**
	 *	Assigns a value of type int and sets nType to the corresponding value.
	 *	
	 *	@param val Value of type int.
	 */
	__inline const CSimpleVariant& CSimpleVariant::operator = (unsigned int val) {
		nType = SVT_UINTEGER; 
		uiVal = val;
		return *this;
	}

		/**
	 *	Assigns a value of type float and sets nType to the corresponding value.
	 *	
	 *	@param val Value of type float.
	 */
	__inline const CSimpleVariant& CSimpleVariant::operator = (float val) {
		nType = SVT_FLOAT;
		fVal = val;
		return *this;
	}

	/**
	 *	Assigns a value of type double and sets nType to the corresponding value.
	 *	
	 *	@param val Value of type double.
	 */
	__inline const CSimpleVariant& CSimpleVariant::operator = (double val) {
		nType = SVT_DOUBLE;
		dblVal = val;
		return *this;
	}

	/**
	 *	Assigns a value of type bool and sets nType to the corresponding value.
	 *	
	 *	@param val Value of type bool.
	 */
	__inline const CSimpleVariant& CSimpleVariant::operator = (bool val) {
		nType = SVT_BOOLEAN;
		bVal = val;
		return *this;
	}

	/**
	 *	Assigns a value of type CPointf and sets nType to the corresponding value.
	 *	
	 *	@param val Value of type CPointf.
	 *
	 *	@see CPointf
	 */
	__inline const CSimpleVariant& CSimpleVariant::operator = (const CPointf &val) {
		nType = SVT_POINT;
		point = val;
		return *this;
	}

	/**
	 *	Assigns a value of type CVector2D and sets nType to the corresponding value.
	 *	
	 *	@param val Value of type CVector2D.
	 *
	 *	@see CVector2D
	 */
	__inline const CSimpleVariant& CSimpleVariant::operator = (const CVector2D &val) {
		nType = SVT_VECTOR2D;
		vec2D = val;
		return *this;
	}

	/**
	 *	Assigns a value of type floatColor and sets nType to the corresponding value.
	 *	
	 *	@param val Value of type floatColor.
	 *
	 *	@see floatColor_s
	 */
	__inline const CSimpleVariant& CSimpleVariant::operator = (const floatColor &val) {
		nType = SVT_FLOATCOL;
		this->color = val;
		return *this;
	}

	int GetIntVal() const;
	unsigned int GetUIntVal() const;
	float GetFloatVal() const;
	double GetDblVal() const;
	bool GetBoolVal() const;
	const floatColor& GetColorVal() const;
	const CPointf& GetPointfVal() const;
	const CVector2D& GetVector2DVal() const;

public:
#ifdef WIN32
	/**
	 *	Returns a string representation of a CSimpleVariant.
	 *	
	 *	@param bHex If true, this function returns a string, representing numerical values in hexadecimal form.
	 *
	 *	@return String representation of the value stored in a CSimpleVariant.
	 */
	CString GetString(bool bHex = false) const;
#endif
};
