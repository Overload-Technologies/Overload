/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include <OvUI/Widgets/Texts/Text.h>
#include <OvUI/Widgets/Drags/DragFloat.h>
#include <OvUI/Widgets/Selection/ColorEdit.h>
#include <OvUI/Widgets/Buttons/Button.h>

#include "OvCore/ECS/Actor.h"

#include "OvCore/ECS/Components/CAmbientBoxLight.h"

OvCore::ECS::Components::CAmbientBoxLight::CAmbientBoxLight(ECS::Actor & p_owner) :
	CLight(p_owner)
{
	m_data.type = OvRendering::Settings::ELightType::AMBIENT_BOX;

	m_data.intensity = 0.03f;
	m_data.constant = 1.0f;
	m_data.linear = 1.0f;
	m_data.quadratic = 1.0f;
}

std::string OvCore::ECS::Components::CAmbientBoxLight::GetName()
{
	return "Ambient Box Light";
}

OvMaths::FVector3 OvCore::ECS::Components::CAmbientBoxLight::GetSize() const
{
	return { m_data.constant, m_data.linear, m_data.quadratic };
}

void OvCore::ECS::Components::CAmbientBoxLight::SetSize(const OvMaths::FVector3& p_size)
{
	m_data.constant = p_size.x;
	m_data.linear = p_size.y;
	m_data.quadratic = p_size.z;
}

void OvCore::ECS::Components::CAmbientBoxLight::OnSerialize(tinyxml2::XMLDocument & p_doc, tinyxml2::XMLNode * p_node)
{
	using namespace OvCore::Helpers;

	CLight::OnSerialize(p_doc, p_node);

	Serializer::SerializeVec3(p_doc, p_node, "size", { m_data.constant, m_data.linear, m_data.quadratic });
}

void OvCore::ECS::Components::CAmbientBoxLight::OnDeserialize(tinyxml2::XMLDocument & p_doc, tinyxml2::XMLNode * p_node)
{
	using namespace OvCore::Helpers;

	CLight::OnDeserialize(p_doc, p_node);

	OvMaths::FVector3 size = Serializer::DeserializeVec3(p_doc, p_node, "size");
	m_data.constant = size.x;
	m_data.linear = size.y;
	m_data.quadratic = size.z;
}

void OvCore::ECS::Components::CAmbientBoxLight::OnInspector(OvUI::Internal::WidgetContainer& p_root)
{
	using namespace OvCore::Helpers;

	CLight::OnInspector(p_root);

	auto sizeGatherer = [this]() -> OvMaths::FVector3 { return { m_data.constant, m_data.linear, m_data.quadratic }; };
	auto sizeProvider = [this](const OvMaths::FVector3& p_data) { m_data.constant = p_data.x; m_data.linear = p_data.y, m_data.quadratic = p_data.z; };

	GUIDrawer::DrawVec3(p_root, "Size", sizeGatherer, sizeProvider, 0.1f, 0.f);
}
