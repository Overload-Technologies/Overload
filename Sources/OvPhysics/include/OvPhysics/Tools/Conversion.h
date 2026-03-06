/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include <OvMaths/FTransform.h>
#include <OvMaths/FVector3.h>
#include <OvMaths/FQuaternion.h>

class btTransform;
class btVector3;
class btQuaternion;

namespace OvPhysics::Tools
{
	/**
	* Conversion helper to convert bullet maths to OvMaths
	*/
	class Conversion
	{
	public:
		Conversion() = delete;

		/**
		* Convert a FTransform to btTransform
		* @param p_transform
		*/
		static btTransform ToBtTransform(const OvMaths::FTransform& p_transform);

		/**
		* Convert a FVector3 to btVector3
		* @param p_vector
		*/
		static btVector3 ToBtVector3(const OvMaths::FVector3& p_vector);

		/**
		* Convert a FQuaternion to btQuaternion
		* @param p_quaternion
		*/
		static btQuaternion ToBtQuaternion(const OvMaths::FQuaternion& p_quaternion);

		/**
		* Convert a btTransform to FTransform
		* @param p_transform
		*/
		static OvMaths::FTransform ToOvTransform(const btTransform& p_transform);

		/**
		* Convert a btVector3 to FVector3
		* @param p_vector
		*/
		static OvMaths::FVector3 ToOvVector3(const btVector3& p_vector);

		/**
		* Convert a btQuaternion to FQuaternion
		* @param p_quaternion
		*/
		static OvMaths::FQuaternion ToOvQuaternion(const btQuaternion& p_quaternion);
	};
}