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

#define _USE_MATH_DEFINES
#include <math.h>

#ifndef _PIXEL_PER_UNIT_
#define _PIXEL_PER_UNIT_
extern float gPixelPerUnit;
#endif

#ifndef _ZOOM_FACTOR_
#define _ZOOM_FACTOR_
extern float gZoomFactor;
#endif


__inline float DegToRad(float deg) {
		return  static_cast<float>(deg * (M_PI/180.0f));
	}

__inline float RadToDeg(float rad) {
	return static_cast<float>(rad * (180.0f/M_PI));
}

void HSVtoRGB( float *r, float *g, float *b, float h, float s, float v );