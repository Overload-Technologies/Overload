/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include <Jolt/Jolt.h>

#include <Jolt/Math/Quat.h>
#include <Jolt/Math/Vec3.h>

#include <OvPhysics/Tools/Conversion.h>

JPH::Vec3 OvPhysics::Tools::Conversion::ToJoltVector3(const OvMaths::FVector3& p_vector)
{
	return JPH::Vec3(p_vector.x, p_vector.y, p_vector.z);
}

JPH::Quat OvPhysics::Tools::Conversion::ToJoltQuaternion(const OvMaths::FQuaternion& p_quaternion)
{
	return JPH::Quat(p_quaternion.x, p_quaternion.y, p_quaternion.z, p_quaternion.w).Normalized();
}

OvMaths::FVector3 OvPhysics::Tools::Conversion::ToOvVector3(const JPH::Vec3& p_vector)
{
	return OvMaths::FVector3(p_vector.GetX(), p_vector.GetY(), p_vector.GetZ());
}

OvMaths::FQuaternion OvPhysics::Tools::Conversion::ToOvQuaternion(const JPH::Quat& p_quaternion)
{
	return OvMaths::FQuaternion(p_quaternion.GetX(), p_quaternion.GetY(), p_quaternion.GetZ(), p_quaternion.GetW());
}
