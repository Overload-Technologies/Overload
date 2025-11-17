/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/


#include <OvCore/ECS/Actor.h>
#include <OvCore/ECS/Components/CPhysicalSphere.h>

#include <OvPhysics/Entities/PhysicalSphere.h>
#include <OvUI/Widgets/Drags/DragFloat.h>

using namespace OvPhysics::Entities;

OvCore::ECS::Components::CPhysicalSphere::CPhysicalSphere(ECS::Actor & p_owner) :
	CPhysicalObject(p_owner)
{
	m_physicalObject = std::make_unique<PhysicalSphere>(p_owner.transform.GetFTransform());

	m_physicalObject->SetUserData<std::reference_wrapper<CPhysicalObject>>(*this);

	BindListener();
	Init();
}

std::string OvCore::ECS::Components::CPhysicalSphere::GetName()
{
	return "Physical Sphere";
}

std::string OvCore::ECS::Components::CPhysicalSphere::GetTypeName()
{
	return std::string{ComponentTraits<CPhysicalSphere>::Name};
}

void OvCore::ECS::Components::CPhysicalSphere::SetRadius(float p_radius)
{
	GetPhysicalObjectAs<PhysicalSphere>().SetRadius(p_radius);
}

float OvCore::ECS::Components::CPhysicalSphere::GetRadius() const
{
	return GetPhysicalObjectAs<PhysicalSphere>().GetRadius();
}

void OvCore::ECS::Components::CPhysicalSphere::OnSerialize(tinyxml2::XMLDocument & p_doc, tinyxml2::XMLNode * p_node)
{
	CPhysicalObject::OnSerialize(p_doc, p_node);

	Helpers::Serializer::SerializeFloat(p_doc, p_node, "radius", GetRadius());
}

void OvCore::ECS::Components::CPhysicalSphere::OnDeserialize(tinyxml2::XMLDocument & p_doc, tinyxml2::XMLNode * p_node)
{
	CPhysicalObject::OnDeserialize(p_doc, p_node);

	SetRadius(Helpers::Serializer::DeserializeFloat(p_doc, p_node, "radius"));
}

void OvCore::ECS::Components::CPhysicalSphere::OnInspector(OvUI::Internal::WidgetContainer & p_root)
{
	CPhysicalObject::OnInspector(p_root);

	Helpers::GUIDrawer::DrawScalar<float>(p_root, "Radius", std::bind(&CPhysicalSphere::GetRadius, this), std::bind(&CPhysicalSphere::SetRadius, this, std::placeholders::_1), 0.1f, 0.f, 100000.f);
}