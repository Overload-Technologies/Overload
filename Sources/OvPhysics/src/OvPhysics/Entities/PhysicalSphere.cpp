/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include <algorithm>

#include <Jolt/Jolt.h>

#include <Jolt/Physics/Collision/Shape/SphereShape.h>

#include <OvPhysics/Entities/PhysicalSphere.h>

OvPhysics::Entities::PhysicalSphere::PhysicalSphere(float p_radius) : PhysicalObject(), m_radius(p_radius)
{
	Init();
}

OvPhysics::Entities::PhysicalSphere::PhysicalSphere(OvMaths::FTransform & p_transform, float p_radius) : PhysicalObject(p_transform), m_radius(p_radius)
{
	Init();
}

void OvPhysics::Entities::PhysicalSphere::SetRadius(float p_radius)
{
	if (p_radius != m_radius)
	{
		m_radius = p_radius;
		RecreateBody();
	}
}

float OvPhysics::Entities::PhysicalSphere::GetRadius() const
{
	return m_radius;
}

JPH::Shape* OvPhysics::Entities::PhysicalSphere::CreateShape(const OvMaths::FVector3& p_scale) const
{
	const float radiusScale = std::max(std::max(p_scale.x, p_scale.y), p_scale.z);
	return new JPH::SphereShape(std::max(m_radius * radiusScale, kMinimumShapeRadius));
}
