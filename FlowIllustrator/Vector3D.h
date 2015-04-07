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
#include "Vector2D.h"

namespace FICore
{
	/**
	 *	Three-dimensional mathematical vector.
	 *	It is designed, such that an array of CVector3D is densly packed into memory
	 *	and can be accessed as an array of float values.
	 */
	class CVector3D :
		public CMathVector
	{
	public:
		float x;	/**< X-component of the vector. */
		float y;	/**< Y-component of the vector. */
		float z;	/**< Z-component of the vector. */

	public:
		/**
		 *	Construct a new CVector3D.
		 *
		 *	@remarks The vector is initialized to (0, 0, 0).
		 */
		__inline CVector3D(void) 
			:x(0.0f), y(0.0f), z(0.0f) {}

		/**
		 *	Construct a new CVector3D and initializes it wit the supplied parameters.
		 *
		 *	@param xVal The X-Component of the vector.
		 *	@param yVal The Y-Component of the vector.
		 *	@param zVal The Z-Component of the vector.
		 */
		__inline CVector3D(float xVal, float yVal, float zVal)
			: x(xVal), y(yVal), z(zVal) {}

		/**
		 *	Construct a new CVector3D and initializes it wit the supplied parameters.
		 *
		 *	@param vec Reference to a CVector2D, used to initialize this vectors X and Y-component.
		 *	@param zVal The Z-Component of the vector.
		 */
		__inline CVector3D(const CVector2D& vec, float zVal)
			: x(vec.x), y(vec.y), z(zVal) {}

		/**
		 *	Construct a new CVector3D and initializes it wit the supplied parameters.
		 *
		 *	@param vec Reference to a CVector3D, whose X, Y and Z-components are copied.
		 */
		__inline CVector3D(const CVector3D &vec)
			: x(vec.x), y(vec.y), z(vec.z) {}

		/**
		 *	Destroy this CVector3D and all of its data.
		 */
		__inline ~CVector3D(void) 
			{}

	public:
		/**
		 *	@brief Assignment operator.
		 *	Assigns the value of another CVector3D to this CVector3D object.
		 *
		 *	@param vec Reference to a CVector3D, whose X, Y and Z-components are assigned to the respective components of this CVector3D.
		 *
		 *	@return A reference to this CVector3D.
		 */
		__inline CVector3D& operator =  (const CVector3D& vec) {
			x = vec.x;
			y = vec.y;
			z = vec.z;
			return *this;
		}

		/**
		 *	@brief Assignment operator.
		 *	Assigns the value of another CVector2D to this CVector3D object.
		 *
		 *	@param vec Reference to a CVector2D, whose X and Y-components are assigned to the respective components of this CVector3D.
		 *
		 *	@return A reference to this CVector3D.
		 *
		 *	@remarks The Z-component of this CVector3D remains unchanged.
		 */
		__inline CVector3D& operator =  (const CVector2D& vec) {
			x = vec.x;
			y = vec.y;
			return *this;
		}

		/**
		 *	@brief Addition assignment operator.
		 *	Adds another CVector3D object component wise to this CVector3D object. 
		 *
		 *	@param vec CVector3D to be added.
		 *
		 *	A reference to this CVector3D.
		 */
		__inline CVector3D& operator += (const CVector3D& vec) {
			x += vec.x;
			y += vec.y;
			z += vec.z;
			return *this;
		}

		/**
		 *	@brief Subtraction assignment operator.
		 *	Subtracts another CVector3D object component wise from this CVector3D object. 
		 *
		 *	@param vec CVector3D to be subtracted.
		 *
		 *	A reference to this CVector3D.
		 */
		__inline CVector3D& operator -= (const CVector3D& vec) {
			x -= vec.x;
			y -= vec.y;
			z -= vec.z;
			return *this;
		}

		/**
		 *	@brief Multiplication assignment operator.
		 *	Multiplies a float value with the components of this CVector3D object. 
		 *
		 *	@param factor Factor as float.
		 *
		 *	A reference to this CVector3D.
		 */
		__inline CVector3D& operator *= (float factor) {
			x *= factor;
			y *= factor;
			z *= factor;
			return *this;
		}

		/**
		 *	@brief Division assignment operator.
		 *	Divides this CVector3D componentwise by a float value. 
		 *
		 *	@param denom The denominator as float.
		 *
		 *	A reference to this CVector3D.
		 */
		__inline CVector3D& operator /= (float denom) {
			x /= denom;
			y /= denom;
			z /= denom;
			return *this;
		}

		/**
		 *	@brief Cross product assignment operator.
		 *	Computes the cross product of this CVector3D and another CVector3D.
		 *
		 *	@param vec A reference to a CVector3D to compute the cross product with.
		 *
		 *	A reference to this CVector3D.
		 */
		__inline CVector3D& operator ^= (const CVector3D& vec) { //cross product
			register float r = y * vec.z - z * vec.y;
			register float s = z * vec.x - x * vec.z;
			z = x * vec.y - y * vec.x;
			x = r;
			y = s;
            
			return *this;
		}

		/**
		 *	@brief Negation operator.
		 *	Computes the negation of this CVector3D
		 *
		 *	@return A new CVector3D, containing the negation of this CVector3D.
		 */
		__inline CVector3D operator - () const {
			return CVector3D(-x, -y, -z);
		}

		/**
		 *	@brief Addition operator.
		 *	Computes the component wise addition of this CVector3D and another CVector3D.
		 *
		 *	@param vec A Refernece to a CVector3D.
		 *
		 *	@return A new CVector3D, containing the addition of this CVector3D and the CVector3D supplied in vec.
		 */
		__inline CVector3D operator + (const CVector3D& vec) const {
			return CVector3D(x + vec.x, y + vec.y, z + vec.z);
		}

		/**
		 *	@brief Subtraction operator.
		 *	Computes the component wise subtraction of this CVector3D and another CVector3D.
		 *
		 *	@param vec A Refernece to a CVector3D.
		 *
		 *	@return A new CVector3D, containing the subtraction of this CVector3D and the CVector3D supplied in vec.
		 */
		__inline CVector3D operator - (const CVector3D& vec) const {
			return CVector3D(x - vec.x, y - vec.y, z - vec.z);
		}

		/**
		 *	@brief Multiplication operator.
		 *	Computes the component wise multiplication of this CVector3D by a float.
		 *
		 *	@param factor The factor as float.
		 *
		 *	@return A new CVector3D, containing the multiplication of this CVector3D and supplied factor.
		 */
		__inline CVector3D operator * (float factor) const {
			return CVector3D(x * factor, y * factor, z * factor);
		}


		/**
		 *	@brief Division operator.
		 *	Computes the component wise division of this CVector3D by a float.
		 *
		 *	@param denom The denominator as float.
		 *
		 *	@return A new CVector3D, containing the division of this CVector3D by the supplied factor.
		 */
		__inline CVector3D operator / (float denom) const {
			return CVector3D(x / denom, y / denom, z / denom);
		}

		/**
		 *	@brief Cross product operator.
		 *	Computes the cross product of this CVector3D and another CVector3D.
		 *
		 *	@param vec A reference to a CVector3D to compute the cross product with.
		 *
		 *	@return A new CVector3D, containing the cross product of this CVector3D and the supplied CVector3D.
		 */
		__inline CVector3D operator ^ (const CVector3D& vec) {
			return CVector3D(
				y * vec.z - z * vec.y,
				z * vec.x - x * vec.z,
				x * vec.y - y * vec.x
			);
		}

		/**
		 *	@brief Dot product operator.
		 *	Computes the dot product of this CVector3D and another CVector3D.
		 *
		 *	@param vec A reference to a CVector3D to compute the dot product with.
		 *
		 *	@return A new CVector3D, containing the dot product of this CVector3D and the supplied CVector3D.
		 */
		__inline float operator * (const CVector3D& vec) {
			return x*vec.x + y*vec.y + z*vec.z;
		}

		/**
		 *	@brief Equality operator.
		 *	Determines the equality of this CVector3D and another CVector3D.
		 *
		 *	@param vec A reference to a CVector3D.
		 *
		 *	@return This operator returns true, if the values of the components of this CVector3D are equal 
		 *			to the respective components of the other CVector3D, otherwise false.
		 */
		__inline bool operator == (const CVector3D& vec) {
			return ( (x == vec.x) && (y == vec.y) && (z == vec.z) );
		}

		/**
		 *	@brief Inequality operator.
		 *	Determines the inequality of this CVector3D and another CVector3D.
		 *
		 *	@param vec A reference to a CVector3D.
		 *
		 *	@return This operator returns true, if at least one of the values of the components of this CVector3D is not equal 
		 *			to the respective component of the other CVector3D, otherwise false.
		 */
		__inline bool operator != (const CVector3D& vec){
			return ( (x != vec.x) || (y != vec.y) || (z != vec.z) );
		}

		/**
		 *	This function computes the magnitide (length) of this CVector3D.
		 *
		 *	@return The magnitude of this CVector3D as float.
		 */
		__inline float abs() const {
			return sqrt(x*x + y*y + z*z);
		}

		/**
		 *	This function normalizes this CVector3D, by dividing its components by its magnitude.
		 */
		__inline void Normalize() {
			(*this) /= abs();
		}

		/**
		 *	Rotate this CVector3D about the X-Axis, by aspecified angle.
		 *
		 *	@param angle The angle to rotate this CVector3D, in radians.
		 *
		 *	@return A refernece to this, now rotated, CVector3D.
		 */
		CVector3D& RotateX(float angle);

		/**
		 *	Rotate this CVector3D about the Y-Axis, by aspecified angle.
		 *
		 *	@param angle The angle to rotate this CVector3D, in radians.
		 *
		 *	@return A refernece to this, now rotated, CVector3D.
		 */
		CVector3D& RotateY(float angle);

		/**
		 *	Rotate this CVector3D about the Z-Axis, by aspecified angle.
		 *
		 *	@param angle The angle to rotate this CVector3D, in radians.
		 *
		 *	@return A refernece to this, now rotated, CVector3D.
		 */
		CVector3D& RotateZ(float angle);

		/**
		 *	Rotate this CVector3D about an arbitrary, by aspecified angle.
		 *
		 *	@param angle	The angle to rotate this CVector3D, in radians.
		 *	@param axis		Reference to a CVector3D, specifying about which axis to rotate.
		 *
		 *	@return A refernece to this, now rotated, CVector3D.
		 */
		CVector3D& RotateAboutAxis(float angle, const CVector3D& axis); //rotate about arbitrary axis
	};
}