/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include <OvCore/ECS/Actor.h>
#include <OvCore/ECS/Components/CPhysicalCapsule.h>

#include <OvPhysics/Entities/PhysicalCapsule.h>
#include <OvUI/Widgets/Drags/DragFloat.h>

using namespace OvPhysics::Entities;

OvCore::ECS::Components::CPhysicalCapsule::CPhysicalCapsule(ECS::Actor & p_owner) :
	CPhysicalObject(p_owner)
{
	m_physicalObject = std::make_unique<PhysicalCapsule>(p_owner.transform.GetFTransform());

	m_physicalObject->SetUserData<std::reference_wrapper<CPhysicalObject>>(*this);

	BindListener();
	Init();
}

std::string OvCore::ECS::Components::CPhysicalCapsule::GetName()
{
	return "Physical Capsule";
}

void OvCore::ECS::Components::CPhysicalCapsule::SetRadius(float p_radius)
{
	GetPhysicalObjectAs<PhysicalCapsule>().SetRadius(p_radius);
}

void OvCore::ECS::Components::CPhysicalCapsule::SetHeight(float p_height)
{
	GetPhysicalObjectAs<PhysicalCapsule>().SetHeight(p_height);
}

float OvCore::ECS::Components::CPhysicalCapsule::GetRadius() const
{
	return GetPhysicalObjectAs<PhysicalCapsule>().GetRadius();
}

float OvCore::ECS::Components::CPhysicalCapsule::GetHeight() const
{
	return GetPhysicalObjectAs<PhysicalCapsule>().GetHeight();
}

void OvCore::ECS::Components::CPhysicalCapsule::OnSerialize(tinyxml2::XMLDocument & p_doc, tinyxml2::XMLNode * p_node)
{
	CPhysicalObject::OnSerialize(p_doc, p_node);

	Helpers::Serializer::SerializeFloat(p_doc, p_node, "radius", GetRadius());
	Helpers::Serializer::SerializeFloat(p_doc, p_node, "height", GetHeight());
}

void OvCore::ECS::Components::CPhysicalCapsule::OnDeserialize(tinyxml2::XMLDocument & p_doc, tinyxml2::XMLNode * p_node)
{
	CPhysicalObject::OnDeserialize(p_doc, p_node);

	SetRadius(Helpers::Serializer::DeserializeFloat(p_doc, p_node, "radius"));
	SetHeight(Helpers::Serializer::DeserializeFloat(p_doc, p_node, "height"));
}

void OvCore::ECS::Components::CPhysicalCapsule::OnInspector(OvUI::Internal::WidgetContainer & p_root)
{
	CPhysicalObject::OnInspector(p_root);

	Helpers::GUIDrawer::DrawScalar<float>(p_root, "Radius", std::bind(&CPhysicalCapsule::GetRadius, this), std::bind(&CPhysicalCapsule::SetRadius, this, std::placeholders::_1), 0.1f, 0.f, 100000.f);
	Helpers::GUIDrawer::DrawScalar<float>(p_root, "Height", std::bind(&CPhysicalCapsule::GetHeight, this), std::bind(&CPhysicalCapsule::SetHeight, this, std::placeholders::_1), 0.1f, 0.f, 100000.f);
}
