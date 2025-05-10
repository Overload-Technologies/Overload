/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include <algorithm>

#include <bullet/btBulletCollisionCommon.h>

#include <OvPhysics/Entities/PhysicalSphere.h>

OvPhysics::Entities::PhysicalSphere::PhysicalSphere(float p_radius) : PhysicalObject()
{
	CreateCollisionShape(p_radius);
	Init();
}

OvPhysics::Entities::PhysicalSphere::PhysicalSphere(OvMaths::FTransform & p_transform, float p_radius) : PhysicalObject(p_transform)
{
	CreateCollisionShape(p_radius);
	Init();
}

void OvPhysics::Entities::PhysicalSphere::SetRadius(float p_radius)
{
	if (p_radius != m_radius)
		RecreateCollisionShape(p_radius);
}

float OvPhysics::Entities::PhysicalSphere::GetRadius() const
{
	return m_radius;
}


void OvPhysics::Entities::PhysicalSphere::CreateCollisionShape(float p_radius)
{
	m_shape = std::make_unique<btSphereShape>(p_radius);
	m_radius = p_radius;
}

void OvPhysics::Entities::PhysicalSphere::RecreateCollisionShape(float p_radius)
{
	CreateCollisionShape(p_radius);
	RecreateBody();
}

void OvPhysics::Entities::PhysicalSphere::SetLocalScaling(const OvMaths::FVector3 & p_scaling)
{
	float radiusScale = std::max(std::max(p_scaling.x, p_scaling.y), p_scaling.z);
	m_shape->setLocalScaling({ radiusScale, radiusScale, radiusScale });
}
