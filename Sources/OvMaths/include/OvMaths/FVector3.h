/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once


namespace OvMaths
{
	/**
	* Mathematic representation of a 3D vector of floats
	*/
	struct FVector3
	{
		static const FVector3 One;
		static const FVector3 Zero;
		static const FVector3 Forward;
		static const FVector3 Right;
		static const FVector3 Up;

		float x;
		float y;
		float z;

		/**
		* Default constructor
		* @param p_x
		* @param p_y
		* @param p_z
		*/
		FVector3(float p_x = 0.0f, float p_y = 0.0f, float p_z = 0.0f);

		/**
		* Copy constructor
		* @param p_toCopy
		*/
		FVector3(const FVector3& p_toCopy);

		/**
		* Move constructor
		* @param p_toMove
		*/
		FVector3(FVector3&& p_toMove) noexcept = default;

		/**
		* Negation
		*/
		FVector3 operator-() const;

		/**
		* Copy assignment
		* @param p_other
		*/
		FVector3 operator=(const FVector3& p_other);

		/**
		* Calculate the sum of two vectors
		* @param p_other
		*/
		FVector3 operator+(const FVector3& p_other) const;

		/**
		* Add the right vector to the left one
		* @param p_other
		*/
		FVector3& operator+=(const FVector3& p_other);

		/**
		* Calculate the substraction of two vectors
		* @param p_other
		*/
		FVector3 operator-(const FVector3& p_other) const;

		/**
		* Remove the right vector from the left one
		* @param p_other
		*/
		FVector3& operator-=(const FVector3& p_other);

		/**
		* Multiple two vector together component-wise
		* @param p_other
		*/
		FVector3 operator*(const OvMaths::FVector3& p_other) const;

		/**
		* Calcualte the multiplication of a vector with a scalar
		* @param p_scalar
		*/
		FVector3 operator*(float p_scalar) const;

		/**
		* Multiply the vector by a scalar
		* @param p_scalar
		*/
		FVector3& operator*=(float p_scalar);

		/**
		* Multiple two vector together component-wise
		* @param p_other
		*/
		FVector3& operator*=(const OvMaths::FVector3& p_other);

		/**
		* Return the division of scalar and actual vector
		* @param p_scalar
		*/
		FVector3 operator/(float p_scalar) const;

		/**
		* Divide scalar to the actual vector
		* @param p_scalar
		*/
		FVector3& operator/=(float p_scalar);

		/**
		* Return true if the two vectors are equals
		* @param p_other
		*/
		bool operator==(const FVector3& p_other);

		/**
		* Return true if the two vectors are different
		* @param p_other
		*/
		bool operator!=(const FVector3& p_other);

		/**
		* Calculate the sum of two vectors
		* @param p_left (First vector)
		* @param p_right (Second vector)
		*/
		static FVector3 Add(const FVector3& p_left, const FVector3& p_right);

		/**
		* Calculate the substraction of two vectors
		* @param p_left (First vector)
		* @param p_right (Second vector)
		*/
		static FVector3 Substract(const FVector3& p_left, const FVector3& p_right);

		/**
		* Calculate the multiplication of a vector with a scalar
		* @param p_target
		* @param p_scalar
		*/
		static FVector3 Multiply(const FVector3& p_target, float p_scalar);

		/**
		* Multiple two vectors component-wise
		* @param p_left
		* @param p_right
		*/
		static FVector3 Multiply(const FVector3& p_left, const FVector3& p_right);

		/**
		* Divide scalar to vector left
		* @param p_left
		* @param p_scalar
		*/
		static FVector3 Divide(const FVector3& p_left, float p_scalar);

		/**
		* Return the length of a vector
		* @param p_target
		*/
		static float Length(const FVector3& p_target);

		/**
		* Return the dot product of two vectors
		* @param p_left
		* @param p_right
		*/
		static float Dot(const FVector3& p_left, const FVector3& p_right);

		/**
		* Return the distance between two vectors
		* @param p_left
		* @param p_right
		*/
		static float Distance(const FVector3& p_left, const FVector3& p_right);

		/**
		* Return the cross product of two vectors
		* @param p_left
		* @param p_right
		*/
		static FVector3 Cross(const FVector3& p_left, const FVector3& p_right);

		/**
		* Return the normalize of the given vector
		* @param p_target
		*/
		static FVector3 Normalize(const FVector3& p_target);

		/**
		* Calculate the interpolation between two vectors
		* @param p_start
		* @param p_end
		* @param p_alpha
		*/
		static FVector3 Lerp(const FVector3& p_start, const FVector3& p_end, float p_alpha);

		/**
		* Calculate the angle between two vectors
		* @param p_from
		* @param p_to
		*/
		static float AngleBetween(const FVector3& p_from, const FVector3& p_to);
	};
}