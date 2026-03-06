/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once


namespace OvMaths
{
	/**
	* Mathematic representation of a 2D vector of floats
	*/
	struct FVector2
	{
		static const FVector2 One;
		static const FVector2 Zero;

		float x;
		float y;

		/**
		* Default constructor
		* @param p_x
		* @param p_y
		*/
		FVector2(float p_x = 0.0f, float p_y = 0.0f);

		/**
		* Copy constructor
		* @param p_toCopy
		*/
		FVector2(const FVector2& p_toCopy);

		/**
		* Move constructor
		* @param p_toMove
		*/
		FVector2(FVector2&& p_toMove) noexcept = default;

		/**
		* Negation
		*/
		FVector2 operator-() const;

		/**
		* Copy assignment
		* @param p_other
		*/
		FVector2 operator=(const FVector2& p_other);

		/**
		* Calculate the sum of two vectors
		* @param p_other
		*/
		FVector2 operator+(const FVector2& p_other) const;

		/**
		* Add the right vector to the left one
		* @param p_other
		*/
		FVector2& operator+=(const FVector2& p_other);

		/**
		* Calculate the substraction of two vectors
		* @param p_other
		*/
		FVector2 operator-(const FVector2& p_other) const;

		/**
		* Remove the right vector from the left one
		* @param p_other
		*/
		FVector2& operator-=(const FVector2& p_other);

		/**
		* Calcualte the multiplication of a vector with a scalar
		* @param p_scalar
		*/
		FVector2 operator*(float p_scalar) const;

		/**
		* Multiply the vector by a scalar
		* @param p_scalar
		*/
		FVector2& operator*=(float p_scalar);

		/**
		* Return the division of scalar and actual vector
		* @param p_scalar
		*/
		FVector2 operator/(float p_scalar) const;

		/**
		* Divide scalar to the actual vector
		* @param p_scalar
		*/
		FVector2& operator/=(float p_scalar);

		/**
		* Return true if the two vectors are equals
		* @param p_other
		*/
		bool operator==(const FVector2& p_other);

		/**
		* Return true if the two vectors are different
		* @param p_other
		*/
		bool operator!=(const FVector2& p_other);

		/**
		* Calculate the sum of two vectors
		* @param p_left (First vector)
		* @param p_right (Second vector)
		*/
		static FVector2 Add(const FVector2& p_left, const FVector2& p_right);

		/**
		* Calculate the substraction of two vectors
		* @param p_left (First vector)
		* @param p_right (Second vector)
		*/
		static FVector2 Substract(const FVector2& p_left, const FVector2& p_right);

		/**
		* Calculate the multiplication of a vector with a scalar
		* @param p_target
		* @param p_scalar
		*/
		static FVector2 Multiply(const FVector2& p_target, float p_scalar);

		/**
		* Divide scalar to vector left
		* @param p_left
		* @param p_scalar
		*/
		static FVector2 Divide(const FVector2& p_left, float p_scalar);

		/**
		* Return the length of a vector
		* @param p_target
		*/
		static float Length(const FVector2& p_target);

		/**
		* Return the dot product of two vectors
		* @param p_left
		* @param p_right
		*/
		static float Dot(const FVector2& p_left, const FVector2& p_right);

		/**
		* Return the normalize of the given vector
		* @param p_target
		*/
		static FVector2 Normalize(const FVector2& p_target);

		/**
		* Calculate the interpolation between two vectors
		* @param p_start
		* @param p_end
		* @param p_alpha
		*/
		static FVector2 Lerp(const FVector2& p_start, const FVector2& p_end, float p_alpha);

		/**
		* Calculate the angle between two vectors
		* @param p_from
		* @param p_to
		*/
		static float AngleBetween(const FVector2& p_from, const FVector2& p_to);
	};
}