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
#include "SimpleVariant.h"
#include "FloatColor.h"
#include <hash_map>
#include <string>

using namespace std;

/**
 *	Enumeration of the available DrawingObject types.
 *	Append new types to the end of the list.
 */
typedef enum DrawingObjectType {
	DO_NONE					= 0x00, 
	DO_VORTEX				= 0x02,
	DO_CHARACTERISTIC_LINE	= 0x01,
	DO_STREAMLINE			= 0x05,
	DO_STREAKLINE			= 0x09,
	DO_PATHLINE				= 0x11,
	DO_TIMELINE				= 0x21,
	DO_RECTANGLE			= 0x40,
	DO_TRIANGLE				= 0x80,
	DO_ELLIPSE				= 0x100, 
	DO_LINE					= 0x200, 
	DO_POINT				= 0x400, 
	DO_BSPLINE				= 0x800, 
	DO_ARROW				= 0x1000,  
	DO_POLYLINE				= 0x2000,
	DO_SPEEDLINE			= 0x4000
};

/**
 *	Enumeration of the available DrawingObject parameters.
 *	Append new parameter types to the end of the list.
 *	
 *	Not all drawing objects use all of the available parameters.
 */
enum DrawinObjectParamName 
{
	DOP_TYPE,
	DOP_COLOR,
	DOP_COLOR_SEL,
	DOP_THICKNESS,
	DOP_ROTATION,
	DOP_ISSOLID,
	DOP_HATCHED,
	DOP_ALPHA,
	DOP_WIDTH,
	DOP_HEIGHT,
	DOP_CENTER,
	DOP_DRAW_STIPPLED,
	DOP_INTEGRATIONSTEPS,
	DOP_STEPLENGTH,
	DOP_ORIGIN,
	DOP_RENDER_AS_PARTICLES,
	DOP_STARTFRAME,
	DOP_USE_STARTFRAME,
	DOP_DRAWASDROPLETS,
	DOP_HALOCOLOR,
	DOP_HAS_HALO,
	DOP_ARROWCOLOR,
	DOP_SMOOTHNESS,
	DOP_VORTEXSTYLE,
	DOP_VORTEXDIR,
	DOP_ARROWSIZE,
	DOP_THRESHOLD,
	DOP_AUTOADJUSTSIZE,
	DOP_SHOWTRAJECTORY,
	DOP_TRANSPARENTTRAJECTORY,
	DOP_REVOLUTIONS,
	DOP_NUMARROWS,
	DOP_TRAJECTORYSTEPS,
	DOP_SHOWARROWS,
	DOP_APPEARANCE,
	DOP_ARROW_ORIGIN,
	DOP_ARROW_DIR,
	DOP_ARROW_LEN,
	DOP_RADIUS1,
	DOP_RADIUS2,
	DOP_PTSTART,
	DOP_PTEND,
	DOP_NUM_SAMPLES,
	DOP_SHOW_SEEDINGLINE,
	DOP_SPEEDLINE_STYLE,
	DOP_ALPHA_MIN,
	DOP_THICKNESS_MIN,
	DOP_NUMDROPLETS,
	DOP_GROW_STEPS,
	DOP_TRANSPARENT_STEPS,
};

/**
 *	This class is used to get and set the parameters of a DrawingObject.
 *	It therefore maps a DrawinObjectParamName to a CSimpleVariant for each parameter/value pair.
 *
 */
class CDrawingObjectParams
{
public:
	/**
	 *	Construct a new CDrawingObjectParams object.
	 *
	 *	@param type Type of drawing object from which the parameters are obtained or to which the parameters are to be set.
	 *
	 *	@remarks Assigning a DrawingObjectType allows to ensure that parameters are set only to the correct object type.
	 *
	 *	@see DrawingObjectType
	 *	@see DrawinObjectParamName
	 *	@see CDrawingObject
	 *	@see CSimpleVariant
	 */
	CDrawingObjectParams(DrawingObjectType type) {
		SetValue(DOP_TYPE, type);
	}

protected:
	CDrawingObjectParams() {};

public:
	hash_map<DrawinObjectParamName, CSimpleVariant>	dataMap;	/**< Mapping of the parameter name to its value. */
	shared_ptr<CDrawingObjectParams> childParams;				/**< Parameters for subordinated drawing objects. */

public:
	/**
	 *	Delete all mapped values.
	 *	This function also releases any subordinated CDrawingObjectParams object and
	 *	frees the according memory.
	 */
	__inline void Clear() {
		dataMap.clear();
		childParams.reset();
	}

	__inline CDrawingObjectParams& operator = (const CDrawingObjectParams &src) {
		Clear();
		dataMap.insert(src.dataMap.begin(), src.dataMap.end());
		return *this;
	}

	/**
	 *	Check if a DrawinObjectParamName/value pair is present in the map.
	 *
	 *	@param key The DrawinObjectParamName whose presence is to be checked
	 *
	 *	@return true, if present, false otherwise
	 *
	 *	@see CSimpleVariant
	 */
	__inline bool HasValue(DrawinObjectParamName key) const {
		return (dataMap.find(key) != dataMap.end());
	}

	/**
	 *	Set a DrawinObjectParamName/value pair.
	 *
	 *	@param key The DrawinObjectParamName
	 *	@param val The value associated to he key
	 *
	 *	@remarks If the supplied key is already present in the map, its value will be overwritten with the value specified in val.
	 *
	 *	@see CSimpleVariant
	 */
	__inline void SetValue(DrawinObjectParamName key, const CSimpleVariant &val) {
		auto pos = dataMap.find(key);
		if ( pos != dataMap.end() ) {
			dataMap.erase(pos);
		}
		dataMap.insert( pair<DrawinObjectParamName, CSimpleVariant> (key, val) );
	}

	/**
	 *	Retrieve the value associated to a DrawinObjectParamName.
	 *
	 *	@param key The DrawinObjectParamName whose value is to be obtained.
	 *	@param val Reference to a CSimpleVariant to receive the stored value.
	 *
	 *	@return If the supplied key is not present in the map, this function returns false and the value in val is unspecified.
				Otherwise, the return value is true and val contains the associated value.
	 *
	 *	@see CSimpleVariant
	 */
	__inline bool GetValue(DrawinObjectParamName key, CSimpleVariant &val) const {
		auto pos = dataMap.find(key);
		if ( pos != dataMap.end() ) {
			val = (*pos).second;
			return true;
		}

		return false;
	}

	/**
	 *	Retrieve the value associated to a DrawinObjectParamName.
	 *
	 *	@param key The DrawinObjectParamName whose value is to be obtained.
	 *
	 *	@return The iVal of the associated CSimpleVariant.
	 *
	 *	@see CSimpleVariant
	 */
	__inline int GetValueInt(DrawinObjectParamName key) const {
		static CSimpleVariant buffer;
		if (GetValue(key, buffer)) {
			return buffer.iVal;
		}
		return 0;
	}

	__inline unsigned int GetValueUInt(DrawinObjectParamName key) const {
		static CSimpleVariant buffer;
		if (GetValue(key, buffer)) {
			return buffer.uiVal;
		}
		return 0;
	}

	/**
	 *	Retrieve the value associated to a DrawinObjectParamName.
	 *
	 *	@param key The DrawinObjectParamName whose value is to be obtained.
	 *
	 *	@return The iVal of the associated CSimpleVariant.
	 *
	 *	@remarks If the value of the associated CSimpleVariat is not of type int, the returned value is undefined.
	 *
	 *	@see CSimpleVariant
	 */
	__inline float GetValueFloat(DrawinObjectParamName key) const {
		static CSimpleVariant buffer;
		if (GetValue(key, buffer)) {
			return buffer.fVal;
		}
		return 0.0f;
	}

	/**
	 *	Retrieve the value associated to a DrawinObjectParamName.
	 *
	 *	@param key The DrawinObjectParamName whose value is to be obtained.
	 *
	 *	@return The iVal of the associated CSimpleVariant.
	 *
	 *	@remarks If the value of the associated CSimpleVariat is not of type bool, the returned value is undefined.
	 *
	 *	@see CSimpleVariant
	 */
	__inline bool GetValueBool(DrawinObjectParamName key) const {
		static CSimpleVariant buffer;
		if (GetValue(key, buffer)) {
			return buffer.bVal;
		}
		return 0;
	}

	/**
	 *	Retrieve the value associated to a DrawinObjectParamName.
	 *
	 *	@param key The DrawinObjectParamName whose value is to be obtained.
	 *
	 *	@return The iVal of the associated CSimpleVariant.
	 *
	 *	@remarks If the value of the associated CSimpleVariat is not of type double, the returned value is undefined.
	 *
	 *	@see CSimpleVariant
	 */
	__inline double GetValueDouble(DrawinObjectParamName key) const {
		static CSimpleVariant buffer;
		if (GetValue(key, buffer)) {
			return buffer.dblVal;
		}
		return 0;
	}

	/**
	 *	Retrieve the value associated to a DrawinObjectParamName.
	 *
	 *	@param key The DrawinObjectParamName whose value is to be obtained.
	 *
	 *	@return The iVal of the associated CSimpleVariant.
	 *
	 *	@remarks If the value of the associated CSimpleVariat is not of type floatColor, the returned value is undefined.
	 *
	 *	@see CSimpleVariant
	 */
	__inline floatColor GetValueFloatColor(DrawinObjectParamName key) const {
		static CSimpleVariant buffer;
		if (GetValue(key, buffer)) {
			return buffer.color;
		}
		return floatColor(0,0,0);
	}

	__inline const floatColor& GetValueFloatColor_r(DrawinObjectParamName key) const {
		static const floatColor error_col(0,0,0);

		auto pos = dataMap.find(key);
		if ( pos != dataMap.end() ) {
			return pos->second.color;
		}

		return error_col;
	}

	/**
	 *	Retrieve the value associated to a DrawinObjectParamName.
	 *
	 *	@param key The DrawinObjectParamName whose value is to be obtained.
	 *
	 *	@return The iVal of the associated CSimpleVariant.
	 *
	 *	@remarks If the value of the associated CSimpleVariat is not of type CPointf, the returned value is undefined.
	 *
	 *	@see CSimpleVariant
	 */
	__inline CPointf GetValuePointf(DrawinObjectParamName key) const {
		static CSimpleVariant buffer;
		if (GetValue(key, buffer)) {
			return buffer.point;
		}
		return CPointf(0,0);
	}

	__inline const CPointf& GetValuePointf_r(DrawinObjectParamName key) const {
		static const CPointf error_pt(0,0);

		auto pos = dataMap.find(key);
		if ( pos != dataMap.end() ) {
			return pos->second.point;
		}

		return error_pt;
	}

	/**
	 *	Retrieve the value associated to a DrawinObjectParamName.
	 *
	 *	@param key The DrawinObjectParamName whose value is to be obtained.
	 *
	 *	@return The iVal of the associated CSimpleVariant.
	 *
	 *	@remarks If the value of the associated CSimpleVariat is not of type CVector2D, the returned value is undefined.
	 *
	 *	@see CSimpleVariant
	 */
	_inline CVector2D GetValueVector2D(DrawinObjectParamName key) const {
		static CSimpleVariant buffer;
		if (GetValue(key, buffer)) {
			return buffer.vec2D;
		}
		return CVector2D(0,0);
	}

	/**
	 *	Set a CDrawingObjectParams object containing parameter-value pairs for 
	 *	a CDrawingObject subordinated to the CDrawingObject this CDrawingObjectParams 
	 *	contains parameters to.
	 *
	 *	@param pParams Pointer to CDrawingObjectParams object.
	 *
	 *	@remarks	The CDrawingObjectParams passed to this function is managed by this 
	 *				CDrawingObjectParams. The memory of the subordinated object is automatically released
	 *				when this CDrawingObjectParams object is destroyed.
	 */
	void SetChildParams(shared_ptr<CDrawingObjectParams> pParams) {
		childParams = pParams;
	}

	/**
	 *	Retrieve a pointer to a CDrawingObjectParams object, containing data
	 *	of a subordinated CDrawingObject.
	 *
	 *	@return Pointer to a CDrawingObjectParams object.
	 */
	CDrawingObjectParams* GetChildParams() const {
		return childParams.get();
	}

	friend class CDrawingObject;
};

/**
 *
 *	Helper class that stores a mapping of int-to-string, as well as the reverse mapping of string-to-int.
 *
 */
class CNameMapping
{
protected:
	hash_map<int, string>	map;		/**< Original mapping from int to string. */
	hash_map<string, int>	revMap;		/**< Reverse mapping from string to int. */

public:
	/**
	 *	Delete all mapped pairs.
	 **/
	__inline void Clear() {
		map.clear();
		revMap.clear();
	}

	/**
	 *	Add a new mapping.
	 *
	 *	@param iVal The int value
	 *	@param sVal The string value
	 *
	 *	@remarks This function creates both, the entry in the original, as well as in the reverse map.
	 **/
	__inline void Add(int iVal, string sVal) {
		map.insert( pair<int, string>( iVal, sVal) );
		revMap.insert( pair<string, int> (sVal, iVal) );
	}

#ifdef WIN32
	/**
	 *	Retrieve the string associated to an int key.
	 *
	 *	@param iVal They key as integer to the associated string.
	 *
	 *	@return The associated string value. If the specified key is not found, this function returns an empty string.
	 */
	__inline CString Get(int iVal) const {
		auto iter = map.find(iVal);
		if (iter != map.end())
			return CString( (*iter).second.c_str() );

		return CString();
	}

	/**
	 *	Retrieve the int associated to a string key.
	 *
	 *	@param sVal They key as string to the associated int.
	 *
	 *	@return The associated int value. If the specified key is not found, this function returns 0.
	 */
	__inline int Get(CString sVal) const {
		CT2CA pszConvertedAnsiString (sVal);
		string str (pszConvertedAnsiString);

		auto iter = revMap.find(str);
		if (iter != revMap.end())
			return (*iter).second;

		return 0;
	}
#else
	/**
	 *	Retrieve the string associated to an int key.
	 *
	 *	@param iVal They key as integer to the associated string.
	 *
	 *	@return The associated string value. If the specified key is not found, this function returns an empty string.
	 */
	__inline string Get(int iVal) const {
		auto iter = map.find(iVal);
		if (iter != map.end())
			return (*iter).second;

		return "";
	}

	/**
	 *	Retrieve the int associated to a string key.
	 *
	 *	@param sVal They key as string to the associated int.
	 *
	 *	@return The associated int value. If the specified key is not found, this function returns 0.
	 */
	__inline int Get(string sVal) const {
		auto iter = revMap.find(sVal);
		if (iter != revMap.end())
			return (*iter).second;

		return 0;
	}
#endif 
};


/**
 *	This class provides two two-way mappings from a string-name to a DrawingObjectType
 *	and vise versa, as well as string-name to DrawinObjectParamName.
 */
class CDONames
{
protected:
	CNameMapping	m_TypeNames;	//*< Mapping from string to DrawingObjectType, and wise versa. */
	CNameMapping	m_ParamNames;	//*< Mapping from string to DrawinObjectParamName, and wise versa. */

public:
	/**
	 *	This function initialises the two mapping.
	 *	All type-name mappings have to be included in this function.
	 */
	void Init();	

#ifdef WIN32
	/**
	 *	Retrieve the CString name, of a DrawingObjectType.
	 *
	 *	@param nType A DrawingObjectType, to which the name is to be retrieved.
	 *
	 *	@return The name of the DrawingObjectType as CString.
	 */
	CString GetTypeName(DrawingObjectType nType) const;

	/**
	 *	Retrieve the DrawingObjectType, base on the CString name of a drawing object.
	 *
	 *	@param strName A reference to a CString, to which the DrawingObjectType is to be retrieved.
	 *
	 *	@return The DrawingObjectType belonging the the supplied name.
	 */
	DrawingObjectType GetType(const CString& strName) const;

	/**
	 *	Retrieve the CString name, of a DrawinObjectParamName.
	 *
	 *	@param nParam A DrawinObjectParamName, to which the name is to be retrieved.
	 *
	 *	@return The name of the DrawinObjectParamName as CString.
	 */
	CString GetParamName(DrawinObjectParamName nParam) const;

	/**
	 *	Retrieve the DrawinObjectParamName, base on the CString name of a drawing object.
	 *
	 *	@param strName A reference to a CString, to which the DrawinObjectParamName is to be retrieved.
	 *
	 *	@return The DrawinObjectParamName belonging the the supplied name.
	 */
	DrawinObjectParamName GetParam(const CString &strName) const;
#else
	string GetTypeName(DrawingObjectType nType) const;
	DrawingObjectType GetType(const string& strName) const;

	string GetParamName(DrawinObjectParamName nParam) const;
	DrawinObjectParamName GetParam(const string &strName) const;
#endif
};

extern CDONames DrawingObjNames;