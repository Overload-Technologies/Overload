/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include <bullet/LinearMath/btTransform.h>

#include <OvPhysics/Tools/Conversion.h>

btTransform OvPhysics::Tools::Conversion::ToBtTransform(const OvMaths::FTransform& p_transform)
{
	return btTransform(ToBtQuaternion(p_transform.GetWorldRotation()), ToBtVector3(p_transform.GetWorldPosition()));
}

btVector3 OvPhysics::Tools::Conversion::ToBtVector3(const OvMaths::FVector3& p_vector)
{
	return btVector3(p_vector.x, p_vector.y, p_vector.z);
}

btQuaternion OvPhysics::Tools::Conversion::ToBtQuaternion(const OvMaths::FQuaternion& p_quaternion)
{
	return btQuaternion(p_quaternion.x, p_quaternion.y, p_quaternion.z, p_quaternion.w);
}

OvMaths::FTransform OvPhysics::Tools::Conversion::ToOvTransform(const btTransform& p_transform)
{
	OvMaths::FQuaternion q = ToOvQuaternion(p_transform.getRotation());
	OvMaths::FVector3  v = ToOvVector3(p_transform.getOrigin());
	return OvMaths::FTransform(v, q);
}

OvMaths::FVector3 OvPhysics::Tools::Conversion::ToOvVector3(const btVector3& p_vector)
{
	return OvMaths::FVector3(p_vector.getX(), p_vector.getY(), p_vector.getZ());
}

OvMaths::FQuaternion OvPhysics::Tools::Conversion::ToOvQuaternion(const btQuaternion& p_quaternion)
{
	return OvMaths::FQuaternion(p_quaternion.getX(), p_quaternion.getY(), p_quaternion.getZ(), p_quaternion.getW());
}
