/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include <OvMaths/FVector3.h>

namespace OvMaths
{
	/**
	* Mathematic representation of a 4D vector of floats
	*/
	struct FVector4
	{
		static const FVector4 One;
		static const FVector4 Zero;

		float x;
		float y;
		float z;
		float w;

		/**
		* Default constructor
		* @param p_x
		* @param p_y
		* @param p_z
		* @param p_w
		*/
		FVector4(float p_x = 0.0f, float p_y = 0.0f, float p_z = 0.0f, float p_w = 0.0f);

		/**
		* Default constructor
		* @param p_vector
		* @param p_w
		*/
		FVector4(const OvMaths::FVector3& p_vector, float p_w = 0.0f);

		/**
		* Copy constructor
		* @param p_toCopy
		*/
		FVector4(const FVector4& p_toCopy);

		/**
		* Move constructor
		* @param p_toMove
		*/
		FVector4(FVector4&& p_toMove) noexcept = default;

		/**
		* Negation
		*/
		FVector4 operator-() const;

		/**
		* Copy assignment
		* @param p_other
		*/
		FVector4 operator=(const FVector4& p_other);

		/**
		* Calculate the sum of two vectors
		* @param p_other
		*/
		FVector4 operator+(const FVector4& p_other) const;

		/**
		* Add the right vector to the left one
		* @param p_other
		*/
		FVector4& operator+=(const FVector4& p_other);

		/**
		* Calculate the substraction of two vectors
		* @param p_other
		*/
		FVector4 operator-(const FVector4& p_other) const;

		/**
		* Remove the right vector from the left one
		* @param p_other
		*/
		FVector4& operator-=(const FVector4& p_other);

		/**
		* Calcualte the multiplication of a vector with a scalar
		* @param p_scalar
		*/
		FVector4 operator*(float p_scalar) const;

		/**
		* Multiply the vector by a scalar
		* @param p_scalar
		*/
		FVector4& operator*=(float p_scalar);

		/**
		* Return the division of scalar and actual vector
		* @param p_scalar
		*/
		FVector4 operator/(float p_scalar) const;

		/**
		* Divide scalar to the actual vector
		* @param p_scalar
		*/
		FVector4& operator/=(float p_scalar);

		/**
		* Return true if the two vectors are equals
		* @param p_other
		*/
		bool operator==(const FVector4& p_other);

		/**
		* Return true if the two vectors are different
		* @param p_other
		*/
		bool operator!=(const FVector4& p_other);

		/**
		* Calculate the sum of two vectors
		* @param p_left (First vector)
		* @param p_right (Second vector)
		*/
		static FVector4 Add(const FVector4& p_left, const FVector4& p_right);

		/**
		* Calculate the substraction of two vectors
		* @param p_left (First vector)
		* @param p_right (Second vector)
		*/
		static FVector4 Substract(const FVector4& p_left, const FVector4& p_right);

		/**
		* Calculate the multiplication of a vector with a scalar
		* @param p_target
		* @param p_scalar
		*/
		static FVector4 Multiply(const FVector4& p_target, float p_scalar);

		/**
		* Divide scalar to vector left
		* @param p_left
		* @param p_scalar
		*/
		static FVector4 Divide(const FVector4& p_left, float p_scalar);

		/**
		* Return the length of a vector
		* @param p_target
		*/
		static float Length(const FVector4& p_target);

		/**
		* Return the dot product of two vectors
		* @param p_left
		* @param p_right
		*/
		static float Dot(const FVector4& p_left, const FVector4& p_right);

		/**
		* Return the normalize of the given vector
		* @param p_target
		*/
		static FVector4 Normalize(const FVector4& p_target);

		/**
		* Calculate the interpolation between two vectors
		* @param p_start
		* @param p_end
		* @param p_alpha
		*/
		static FVector4 Lerp(const FVector4& p_start, const FVector4& p_end, float p_alpha);
	};
}