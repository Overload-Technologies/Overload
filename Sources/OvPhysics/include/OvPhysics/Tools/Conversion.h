/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include <OvMaths/FVector3.h>
#include <OvMaths/FQuaternion.h>

namespace JPH
{
	class Quat;
	class Vec3;
}

namespace OvPhysics::Tools
{
	/**
	* Conversion helper to convert Jolt maths to OvMaths
	*/
	class Conversion
	{
	public:
		Conversion() = delete;

		/**
		* Convert a FVector3 to JPH::Vec3
		* @param p_vector
		*/
		static JPH::Vec3 ToJoltVector3(const OvMaths::FVector3& p_vector);

		/**
		* Convert a FQuaternion to a normalized JPH::Quat
		* @param p_quaternion
		*/
		static JPH::Quat ToJoltQuaternion(const OvMaths::FQuaternion& p_quaternion);

		/**
		* Convert a JPH::Vec3 to FVector3
		* @param p_vector
		*/
		static OvMaths::FVector3 ToOvVector3(const JPH::Vec3& p_vector);

		/**
		* Convert a JPH::Quat to FQuaternion
		* @param p_quaternion
		*/
		static OvMaths::FQuaternion ToOvQuaternion(const JPH::Quat& p_quaternion);
	};
}