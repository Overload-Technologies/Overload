/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include <bullet/btBulletCollisionCommon.h>

#include <OvPhysics/Entities/PhysicalBox.h>
#include <OvPhysics/Tools/Conversion.h>

OvPhysics::Entities::PhysicalBox::PhysicalBox(const OvMaths::FVector3& p_size) : PhysicalObject()
{
	CreateCollisionShape(p_size);
	Init();
}

OvPhysics::Entities::PhysicalBox::PhysicalBox(OvMaths::FTransform & p_transform, const OvMaths::FVector3& p_size) : PhysicalObject(p_transform)
{
	CreateCollisionShape(p_size);
	Init();
}

void OvPhysics::Entities::PhysicalBox::SetSize(const OvMaths::FVector3& p_size)
{
	if (m_size != p_size)
		RecreateCollisionShape(p_size);
}

OvMaths::FVector3 OvPhysics::Entities::PhysicalBox::GetSize() const
{
	return m_size;
}

void OvPhysics::Entities::PhysicalBox::CreateCollisionShape(const OvMaths::FVector3& p_size)
{
	m_shape = std::make_unique<btBoxShape>(OvPhysics::Tools::Conversion::ToBtVector3(p_size));
	m_size = p_size;
}

void OvPhysics::Entities::PhysicalBox::RecreateCollisionShape(const OvMaths::FVector3& p_size)
{
	CreateCollisionShape(p_size);
	RecreateBody();
}

void OvPhysics::Entities::PhysicalBox::SetLocalScaling(const OvMaths::FVector3 & p_scaling)
{
	m_shape->setLocalScaling(OvPhysics::Tools::Conversion::ToBtVector3(p_scaling));
}