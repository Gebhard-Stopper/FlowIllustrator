/*
 *	Copyright (C) 2014, Max Planck Institut für Informatik, Saarbrücken.
 *	Implementation: 2014, Gebhard Stopper [ gebhard.stopper@gmail.com ]
 *	
 *	If you perform any changes on this file, please append your name to 
 *	the List of people who worked on this file.
 *
 *	If you add or modify functions or variables, please do not forget to
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
#include "mathvector.h"
#include "math.h"

namespace FICore
{
	/**
	 *	Two-dimensional mathematical vector.
	 *	It is designed, such that an array of CVector2D is densly packed into memory
	 *	and can be accessed as an array of float values.
	*/
	class CVector2D :
		public CMathVector
	{
	public:
		float x;	/**< X-component of the vector. */
		float y;	/**< Y-component of the vector. */

	
	public:	
		/**
		 *	Construct a new CVector2D.
		 *
		 *	@remarks The vector is initialized to (0 , 0).
		 */
		__inline CVector2D(void) 
			: x(0.0f), y (0.0f) {}

		/**
		 *	Construct a new CVector2D and initializes it wit the supplied parameters.
		 *
		 *	@param xVal The X-Component of the vector.
		 *	@param yVal The Y-Component of the vector.
		 */
		__inline CVector2D(float xVal, float yVal) 
			: x(xVal), y (yVal) {}

		/**
		 *	Construct a new CVector2D and initializes it wit the supplied parameter
		 *
		 *	@param vec Reference to a CVector2D object whose parameters are to be copied.
		 */
		__inline CVector2D(const CVector2D &vec) 
			: x(vec.x), y(vec.y) {}

		/**
		 *	Destroys the CVector2D obejct and its data.
		 */
		__inline ~CVector2D(void) 
			{}

	public: //Operators
		/**
		 *	@brief Assignment operator.
		 *
		 *	@param vec CVector2D to be assigned.
		 */
		__inline CVector2D& operator = (const CVector2D& vec) {
			x = vec.x;
			y = vec.y;
			return *this;
		}

		/**
		 *	@brief Addition assignment operator.
		 *	Adds the value of another CVector2D object to this CVector2D object. 
		 *
		 *	@param vec CVector2D to be added.
		 *
		 *	@return A reference to this CVector2D.
		 */
		__inline CVector2D& operator += (const CVector2D& vec){
			x += vec.x;
			y += vec.y;
			return *this;
		}

		/**
		 *	@brief Subtraction assignment operator.
		 *	Subtracts the value of a float to the from the components this CVector2D object. 
		 *
		 *	@param fVal Float to be subtracted.
		 */
		__inline CVector2D& operator -= (float fVal) {
			x -= fVal;
			y -= fVal;
			return *this;
		}

		/**
		 *	@brief Subtraction assignment operator.
		 *	Subtracts the another CVector2D object from this CVector2D object. 
		 *
		 *	@param vec CVector2D to be subtracted.
		 */
		__inline CVector2D& operator -= (const CVector2D& vec){
			x -= vec.x;
			y -= vec.y;
			return *this;
		}

		/**
		 *	@brief Multiplication assignment operator.
		 *	Multiplies a float value with this CVector2D object. 
		 *
		 *	@param val The factor as float.
		 */
		__inline CVector2D& operator *= (float val){
			x *= val;
			y *= val;
			return *this;
		}

		/**
		 *	@brief Division assignment operator.
		 *	Divides this CVector2D object by a float value. 
		 *
		 *	@param val The factor as float.
		 */
		__inline CVector2D& operator /= (float val){
			x /= val;
			y /= val;
			return *this;
		}

		/**
		 *	@brief Negation operator.
		 *	Returns a negated copy of this CVector2D object. 
		 *
		 *	@return The negated CVector2D object.
		 */
		__inline CVector2D operator -() const {
			return CVector2D(-x, -y);
		}

		/**
		 *	@brief Addition operator.
		 *	Adds another CVector2D object component-wise to a copy of this object. 
		 *
		 *	@param vec The CVector2D to be added.
		 *
		 *	@return	The result of the addition.
		 */
		__inline CVector2D operator +(const CVector2D& vec) const{
			return CVector2D(x + vec.x, y + vec.y);
		}

		/**
		 *	@brief Subtraction operator.
		 *	Subtracts another CVector2D object component-wise from a copy of this object. 
		 *
		 *	@param vec The CVector2D to be subtracted.
		 *
		 *	@return	The result of the Subtraction.
		 */
		__inline CVector2D operator -(const CVector2D& vec) const{
			return CVector2D(x - vec.x, y - vec.y);
		}

		/**
		 *	@brief Multiplication operator.
		 *	Multiplies a float value component-wise with a copy of this object. 
		 *
		 *	@param val The float factor.
		 *
		 *	@return	The result of the multiplication.
		 */
		__inline CVector2D operator *(float val) const{
			return CVector2D(x * val, y * val);
		}

		/**
		 *	@brief Division operator.
		 *	Divides a copy of this object component-wise by a float value. 
		 *
		 *	@param val The float denominator .
		 *
		 *	@return	The result of the division.
		 */
		__inline CVector2D operator /(float val) const{
			return CVector2D(x / val, y / val);
		}

		/**
		 *	@brief Cross product operator.
		 *	Calculates the cross product of this CVector2D and the supplied argument, vec. 
		 *
		 *	@param vec The second vector for the cross product.
		 *
		 *	@return	The cross product as float.
		 *
		 *	@remarks	The cross product in 2D is calculated as follows: <BR>
		 *				cp = vex1 x vec2 = vec1.x*vec2.y - vex1.y-vec2.x
		 */
		__inline float operator ^ (const CVector2D& vec){
			return x*vec.y - y*vec.x;
		} 

		/**
		 *	@brief Dot product operator.
		 *	Calculates the dot product (inner product) of this CVector2D and the supplied argument, vec. 
		 *
		 *	@param vec The second vector for the dot product.
		 *
		 *	@return	The dot product as float.
		 *
		 *	@remarks	The dot product in 2D is calculated as follows: <BR>
		 *				dp = vex1 * vec2 = vec1.x*vec2.x + vex1.y-vec2.y
		 */
		__inline float operator * (const CVector2D& vec){
			return x * vec.x + y * vec.y;
		} 

		/**
		 *	@brief Equal operator.
		 *	Determines if two CVector2D objects are equal, i.e. their respective X and y-components are equal.
		 *
		 *	@param vec The CVector2D, this vecor is compared against.
		 *
		 *	@return	True, if the two vectors are equal, otherwise false.
		 */
		__inline bool operator ==(const CVector2D& vec) const{
			return ((x == vec.x) && (y == vec.y));
		}

		/**
		 *	@brief Not equal operator.
		 *	Determines if two CVector2D objects are not equal, i.e. at least one pair of their respective X and y-components are not equal.
		 *
		 *	@param vec The CVector2D, this vecor is compared against.
		 *
		 *	@return	True, if the two vectors are not equal, otherwise false.
		 */
		__inline bool operator !=(const CVector2D& vec) const{
			return ((x != vec.x) || (y != vec.y));
		}

		/**
		 *	@brief Index operator.
		 *	Allows to access the values of the vector in an index based manner.
		 *
		 *	@param indx Zero-based index of the component whose value is to be accessed.
		 *
		 *	@return	A copy of the value at the specified index.
		 *
		 *	@remarks Valid values for an index are only {0, 1}. Other values will cause undefined behaviour.
		 */
		__inline float operator [] (int indx) const {
			return *(reinterpret_cast<const float*>(this)+indx);
		}

		/**
		 *	@brief Index operator.
		 *	Allows to access the values of the vector in an index based manner.
		 *
		 *	@param indx Zero-based index of the component whose value is to be accessed.
		 *
		 *	@return	A refernece to the value at the specified index.
		 *
		 *	@remarks Valid values for an index are only {0, 1}. Other values will cause undefined behaviour.
		 */
		__inline float& operator [] (int indx) {
			return *(reinterpret_cast<float*>(this)+indx);
		}

	public:

		/**
		 *	Normalizes this CVector2D object.
		 *
		 *	@return	A reference to this, normalized CVector2D object.
		 */
		__inline CVector2D& Normalize(){
			return *this /= abs();
		}


		/**
		 *	Rotate the vector in a counter clockwise fashon.
		 *
		 *	@param angle The relative angle to rotate the vector, in radians.
		 *
		 *	@return	A reference to this, rotated CVector2D object.
		 */
		__inline CVector2D& Rotate(const float angle){
			const float s = sinf(angle);
			const float c = cosf(angle);
    
			const float nx = c * x - s * y;
			const float ny = s * x + c * y;
    
			x = nx;
			y = ny;
    
			return *this;
		}

		/**
		 *	Retrieve the vector magnitude (length) of this CVector2D obejct.
		 *
		 *	@return	The vector magnitude as float.
		 */
		__inline float abs() const{
			return sqrt( x*x + y*y );
		}

	#ifdef WIN32
		__inline CString ToString() const {
			CString str;
			str.Format(_T("%f,%f"), x, y);
			return str;		
		}
	#else
		__inline string ToString() const {
			//TODO
		}
	#endif
	};
}