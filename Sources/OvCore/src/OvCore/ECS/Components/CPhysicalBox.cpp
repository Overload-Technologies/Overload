/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include <OvCore/ECS/Actor.h>
#include <OvCore/ECS/Components/CPhysicalBox.h>

#include <OvPhysics/Entities/PhysicalBox.h>

using namespace OvPhysics::Entities;

OvCore::ECS::Components::CPhysicalBox::CPhysicalBox(ECS::Actor & p_owner) :
	CPhysicalObject(p_owner)
{
	m_physicalObject = std::make_unique<OvPhysics::Entities::PhysicalBox>(p_owner.transform.GetFTransform());

	m_physicalObject->SetUserData<std::reference_wrapper<CPhysicalObject>>(*this);

	BindListener();
	Init();
}

std::string OvCore::ECS::Components::CPhysicalBox::GetName()
{
	return "Physical Box";
}

std::string OvCore::ECS::Components::CPhysicalBox::GetTypeName()
{
	return std::string{ComponentTraits<CPhysicalBox>::Name};
}

void OvCore::ECS::Components::CPhysicalBox::SetSize(const OvMaths::FVector3 & p_size)
{
	GetPhysicalObjectAs<PhysicalBox>().SetSize(p_size);
}

OvMaths::FVector3 OvCore::ECS::Components::CPhysicalBox::GetSize() const
{
	return GetPhysicalObjectAs<PhysicalBox>().GetSize();
}

void OvCore::ECS::Components::CPhysicalBox::OnSerialize(tinyxml2::XMLDocument & p_doc, tinyxml2::XMLNode * p_node)
{
	CPhysicalObject::OnSerialize(p_doc, p_node);

	Helpers::Serializer::SerializeVec3(p_doc, p_node, "size", GetSize());
}

void OvCore::ECS::Components::CPhysicalBox::OnDeserialize(tinyxml2::XMLDocument & p_doc, tinyxml2::XMLNode * p_node)
{
	CPhysicalObject::OnDeserialize(p_doc, p_node);

	SetSize(Helpers::Serializer::DeserializeVec3(p_doc, p_node, "size"));
}

void OvCore::ECS::Components::CPhysicalBox::OnInspector(OvUI::Internal::WidgetContainer & p_root)
{
	CPhysicalObject::OnInspector(p_root);

	Helpers::GUIDrawer::DrawVec3(p_root, "Size", std::bind(&CPhysicalBox::GetSize, this), std::bind(&CPhysicalBox::SetSize, this, std::placeholders::_1), 0.1f, 0.f, 100000.f);
}