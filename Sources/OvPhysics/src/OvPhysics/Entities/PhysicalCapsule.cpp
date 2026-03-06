/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include <algorithm>

#include <bullet/btBulletCollisionCommon.h>

#include <OvPhysics/Entities/PhysicalCapsule.h>
#include <OvPhysics/Tools/Conversion.h>

OvPhysics::Entities::PhysicalCapsule::PhysicalCapsule(float p_radius, float p_height) : PhysicalObject()
{
	CreateCollisionShape(p_radius, p_height);
	Init();
}

OvPhysics::Entities::PhysicalCapsule::PhysicalCapsule(OvMaths::FTransform & p_transform, float p_radius, float p_height) : PhysicalObject(p_transform)
{
	CreateCollisionShape(p_radius, p_height);
	Init();
}

void OvPhysics::Entities::PhysicalCapsule::SetRadius(float p_radius)
{
	RecreateCollisionShape(p_radius, m_height);
}

void OvPhysics::Entities::PhysicalCapsule::SetHeight(float p_height)
{
	RecreateCollisionShape(m_radius, p_height);
}

float OvPhysics::Entities::PhysicalCapsule::GetRadius() const
{
	return m_radius;
}

float OvPhysics::Entities::PhysicalCapsule::GetHeight() const
{
	return m_height;
}

void OvPhysics::Entities::PhysicalCapsule::CreateCollisionShape(float p_radius, float p_height)
{
	m_shape = std::make_unique<btCapsuleShape>(p_radius, p_height);
	m_radius = p_radius;
	m_height = p_height;
}

void OvPhysics::Entities::PhysicalCapsule::RecreateCollisionShape(float p_radius, float p_height)
{
	CreateCollisionShape(p_radius, p_height);
	RecreateBody();
}

void OvPhysics::Entities::PhysicalCapsule::SetLocalScaling(const OvMaths::FVector3 & p_scaling)
{
	m_shape->setLocalScaling({ std::max(p_scaling.x, p_scaling.z), p_scaling.y, 1.0f });
}