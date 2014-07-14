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

/**
 *	Represents a RGBA color. 
 *
 *	Each component has a range of valid values from [0..1].
 *
 *	A pointer floatColor value can be directly passed to glColor4fv() to set the current OpenGL drawing color.
 */
typedef struct floatColor_s {
	float red;		/**< Red component. */
	float green;	/**< Green component. */
	float blue;		/**< Blue component. */
	float alpha;	/**< Alpha component for transparency. */

public:
	/**
	 *	Construct a new floatColor object
	 *
	 *	@param r The red component. Valid values are in range [0..1].
	 *	@param g The green component. Valid values are in range [0..1].
	 *	@param b The blue component. Valid values are in range [0..1].
	 *	@param a The alpha component. Valid values are in range [0..1], where 1 is fully opaque and 0 is fully transparent.
	 */
	floatColor_s(float r, float g, float b, float a=1.0f) 
		: red(r), green(g), blue(b), alpha(a) {}

	/**
	 *	Construct a new floatColor object
	 *
	 *	@remarks The color is initially black (r=g=b=0, a=1)
	 */
	floatColor_s() {}

	/**
	 *	Construct a new floatColor object
	 *
	 *	@param color Reference to a floatColor objects, whose values are copied.
	 */
	floatColor_s(const floatColor_s &color) 
		: red (color.red), green(color.green), blue(color.blue), alpha(color.alpha)
	{}

public:
#ifdef WIN32 
	/**
	 *	Retrieve the color values as an RGB hex string.
	 *	
	 *	@return String of the format #RRGGBB
	 *
	 *	@remarks color	The original values in range [0..1] are transformed into [0..255] and represented as hexadecimal integers. 
						The alpha component is thereby ignored.
	 */
	__inline CString GetHexString() const {
		CString retVal;
		retVal.Format(_T("#%02x%02x%02x"), static_cast<int>(red*255), static_cast<int>(green*255), static_cast<int>(blue*255) );
		return retVal;
	}
#else
		/**
	 *	Retrieve the color values as an RGB hex string.
	 *	
	 *	@return String of the format #RRGGBB
	 *
	 *	@remarks color	The original values in range [0..1] are transformed into [0..255] and represented as hexadecimal integers. 
						The alpha component is thereby ignored.
	 */
	__inline string GetHexString() const {
		char buffer[8];
		sprintf(buffer, "#%02x%02x%02x", static_cast<int>(red*255), static_cast<int>(green*255), static_cast<int>(blue*255) );
		return string(buffer);		
	}
#endif

} floatColor, *LPFLOATCOLOR;