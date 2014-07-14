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
#include "Vector3D.h"
#include "math.h"

CVector3D& CVector3D::RotateX(float angle)
{
	register float s = sinf(angle);
    register float c = cosf(angle);
    
    register float ny = c * y - s * z;
    register float nz = c * z + s * y;
    
    y = ny;
    z = nz;
    
    return *this;
}

CVector3D& CVector3D::RotateY(float angle)
{
	register float s = sinf(angle);
    register float c = cosf(angle);
    
    register float nx = c * x + s * z;
    register float nz = c * z - s * x;
    
    x = nx;
    z = nz;
    
    return *this;
}

CVector3D& CVector3D::RotateZ(float angle)
{
	register float s = sinf(angle);
    register float c = cosf(angle);
    
    register float nx = c * x - s * y;
    register float ny = c * y + s * x;
    
    x = nx;
    y = ny;
    
    return *this;
}

CVector3D& CVector3D::RotateAboutAxis(float angle, const CVector3D& axis)
{
	register float s = sinf(angle);
    register float c = cosf(angle);
    register float k = 1.0F - c;
    
    register float nx = x * (c + k * axis.x * axis.x) + y * (k * axis.x * axis.y - s * axis.z)
            + z * (k * axis.x * axis.z + s * axis.y);
    register float ny = x * (k * axis.x * axis.y + s * axis.z) + y * (c + k * axis.y * axis.y)
            + z * (k * axis.y * axis.z - s * axis.x);
    register float nz = x * (k * axis.x * axis.z - s * axis.y) + y * (k * axis.y * axis.z + s * axis.x)
            + z * (c + k * axis.z * axis.z);
    
    x = nx;
    y = ny;
    z = nz;
    
    return *this;
}