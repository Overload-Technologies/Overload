/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include <stdint.h>

#include "OvMaths/FVector3.h"
#include "OvMaths/FVector2.h"

namespace OvMaths
{
	/**
	* Mathematic representation of a 3x3 Matrix of floats
	*/
	struct FMatrix3
	{
		float data[9];
		static const FMatrix3 Identity;

		/**
		* Default constructor
		*/
		FMatrix3();

		/**
		* Set all elements to value
		* @param p_value
		*/
		FMatrix3(float p_value);

		/**
		* Constructor
		* @param p_element1
		* @param p_element2
		* @param p_element3
		* @param p_element4
		* @param p_element5
		* @param p_element6
		* @param p_element7
		* @param p_element9
		* @param p_element8
		*/
		FMatrix3(float p_element1, float p_element2, float p_element3,
				 float p_element4, float p_element5, float p_element6, 
				 float p_element7, float p_element8, float p_element9);

		/**
		* Copy constructor
		* @param p_other
		*/
		FMatrix3(const FMatrix3& p_other);

		/**
		* Copy assignment
		* @param p_other
		*/
		FMatrix3& operator=(const FMatrix3& p_other);

		/**
		* Check if elements are equals
		* @param p_other
		*/
		bool operator==(const FMatrix3& p_other);

		/**
		* Element-wise addition
		* @param p_other
		*/
		FMatrix3 operator+(const FMatrix3& p_other) const;

		/**
		* Element-wise addition
		* @param p_other
		*/
		FMatrix3& operator+=(const FMatrix3& p_other);

		/**
		* Element-wise subtraction
		* @param p_other
		*/
		FMatrix3 operator-(const FMatrix3& p_other) const;

		/**
		* Element-wise subtraction
		* @param p_other
		*/
		FMatrix3& operator-=(const FMatrix3& p_other);

		/**
		* Scalar Product
		* @param p_scalar
		*/
		FMatrix3 operator*(float p_scalar) const;

		/**
		* Scalar Product
		* @param p_scalar
		*/
		FMatrix3& operator*=(float p_scalar);

		/**
		* Vector Product
		* @param p_vector
		*/
		FVector3 operator*(const FVector3& p_vector) const;

		/**
		* Matrix Product
		* @param p_other
		*/
		FMatrix3 operator*(const FMatrix3& p_other) const;

		/**
		* Matrix Product
		* @param p_other
		*/
		FMatrix3& operator*=(const FMatrix3& p_other);

		/**
		* Scalar Division
		* @param p_scalar
		*/
		FMatrix3 operator/(float p_scalar) const;

		/**
		* Scalar Division
		* @param p_scalar
		*/
		FMatrix3& operator/=(float p_scalar);

		/**
		* Matrix Division
		* @param p_other
		*/
		FMatrix3 operator/(const FMatrix3& p_other) const;

		/**
		* Matrix Division
		* @param p_other
		*/
		FMatrix3& operator/=(const FMatrix3& p_other);

		/**
		* Get element at index (row,column)
		* @param p_row
		* @param p_column
		*/
		float& operator()(uint8_t p_row, uint8_t p_column);
		
		/**
		* Check if elements are equals
		* @param p_left
		* @param p_right
		*/
		static bool AreEquals(const FMatrix3& p_left, const FMatrix3& p_right);

		/**
		* Element-wise addition
		* @param p_left
		* @param p_scalar
		*/
		static FMatrix3 Add(const FMatrix3& p_left, float p_scalar);

		/**
		* Element-wise addition
		* @param p_left
		* @param p_right
		*/
		static FMatrix3 Add(const FMatrix3& p_left, const FMatrix3& p_right);

		/**
		* Element-wise subtraction
		* @param p_left
		* @param p_scalar
		*/
		static FMatrix3 Subtract(const FMatrix3& p_left, float p_scalar);

		/**
		* Element-wise subtractions
		* @param p_left
		* @param p_right
		*/
		static FMatrix3 Subtract(const FMatrix3& p_left, const FMatrix3& p_right);

		/**
		* Scalar Product
		* @param p_left
		* @param p_scalar
		*/
		static FMatrix3 Multiply(const FMatrix3& p_left, float p_scalar);

		/**
		* Vector Product
		* @param p_matrix
		* @param p_vector
		*/
		static FVector3 Multiply(const FMatrix3& p_matrix, const FVector3& p_vector);

		/**
		* Matrix Product
		* @param p_left
		* @param p_right
		*/
		static FMatrix3 Multiply(const FMatrix3& p_left, const FMatrix3& p_right);

		/**
		* Scalar Division
		* @param p_left
		* @param p_scalar
		*/
		static FMatrix3 Divide(const FMatrix3& p_left, float p_scalar);

		/**
		* Matrix Division
		* @param p_left
		* @param p_right
		*/
		static FMatrix3 Divide(const FMatrix3& p_left, const FMatrix3& p_right);

		/**
		* Compare to Identity matrix
		* @param p_matrix
		*/
		static bool IsIdentity(const FMatrix3& p_matrix);

		/**
		* Compute matrix determinant
		* @param p_matrix
		*/
		static float Determinant(const FMatrix3& p_matrix);

		/**
		* Return transposed matrix
		* @param p_matrix
		*/
		static FMatrix3 Transpose(const FMatrix3& p_matrix);

		/**
		* Return Cofactor matrix
		* @param p_matrix
		*/
		static FMatrix3 Cofactor(const FMatrix3& p_matrix);

		/**
		* Return Minor matrix
		* @param p_matrix
		*/
		static FMatrix3 Minor(const FMatrix3& p_matrix);

		/**
		* Return Adjoint matrix
		* @param p_other
		*/
		static FMatrix3 Adjoint(const FMatrix3& p_other);

		/**
		* Return inverse matrix
		* @param p_matrix
		*/
		static FMatrix3 Inverse(const FMatrix3& p_matrix);

		/**
		* Return 2D translation matrix
		* @param p_translation
		*/
		static FMatrix3 Translation(const FVector2& p_translation);

		/**
		* Translate matrix in 2D
		* @param p_matrix
		* @param p_translation
		*/
		static FMatrix3 Translate(const FMatrix3& p_matrix, const FVector2& p_translation);

		/**
		* Return 2D rotation matrix
		* @param p_rotation angle in radians
		*/
		static FMatrix3 Rotation(float p_rotation);

		/**
		* Rotate matrix in 2D
		* @param p_matrix
		* @param p_rotation angle in radians
		*/
		static FMatrix3 Rotate(const FMatrix3& p_matrix, float p_rotation);

		/**
		* Return 2D scaling matrix
		* @param p_scale
		*/
		static FMatrix3 Scaling(const FVector2& p_scale);

		/**
		* Scale matrix in 2D
		* @param p_matrix
		* @param p_scale
		*/
		static FMatrix3 Scale(const FMatrix3& p_matrix, const FVector2& p_scale);
	
		/**
		* Get row
		* @param p_row
		*/
		static FVector3 GetRow(const FMatrix3& p_matrix, uint8_t p_row);

		/**
		* Get Column
		* @param p_column
		*/
		static FVector3 GetColumn(const FMatrix3& p_matrix, uint8_t p_column);
	};
}