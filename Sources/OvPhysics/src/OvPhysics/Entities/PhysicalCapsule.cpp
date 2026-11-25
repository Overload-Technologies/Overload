/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include <algorithm>

#include <Jolt/Jolt.h>

#include <Jolt/Physics/Collision/Shape/CapsuleShape.h>
#include <Jolt/Physics/Collision/Shape/SphereShape.h>

#include <OvPhysics/Entities/PhysicalCapsule.h>

OvPhysics::Entities::PhysicalCapsule::PhysicalCapsule(float p_radius, float p_height) : PhysicalObject(), m_radius(p_radius), m_height(p_height)
{
	Init();
}

OvPhysics::Entities::PhysicalCapsule::PhysicalCapsule(OvMaths::FTransform & p_transform, float p_radius, float p_height) : PhysicalObject(p_transform), m_radius(p_radius), m_height(p_height)
{
	Init();
}

void OvPhysics::Entities::PhysicalCapsule::SetRadius(float p_radius)
{
	m_radius = p_radius;
	RecreateBody();
}

void OvPhysics::Entities::PhysicalCapsule::SetHeight(float p_height)
{
	m_height = p_height;
	RecreateBody();
}

float OvPhysics::Entities::PhysicalCapsule::GetRadius() const
{
	return m_radius;
}

float OvPhysics::Entities::PhysicalCapsule::GetHeight() const
{
	return m_height;
}

JPH::Shape* OvPhysics::Entities::PhysicalCapsule::CreateShape(const OvMaths::FVector3& p_scale) const
{
	const float radius = std::max(m_radius * std::max(p_scale.x, p_scale.z), kMinimumShapeRadius);
	const float halfHeight = m_height * p_scale.y * 0.5f;

	if (halfHeight > 0.0f)
	{
		return new JPH::CapsuleShape(halfHeight, radius);
	}

	return new JPH::SphereShape(radius);
}
