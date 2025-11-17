/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include <OvCore/ECS/Actor.h>
#include <OvCore/ECS/Components/CLight.h>

#include <OvUI/Widgets/Texts/Text.h>
#include <OvUI/Widgets/Drags/DragFloat.h>
#include <OvUI/Widgets/Selection/ColorEdit.h>
#include <OvUI/Widgets/Buttons/Button.h>
#include <OvUI/Widgets/Layout/Group.h>

OvCore::ECS::Components::CLight::CLight(ECS::Actor & p_owner) :
	AComponent(p_owner),
	m_data{ p_owner.transform.GetFTransform() }
{
}

OvRendering::Entities::Light& OvCore::ECS::Components::CLight::GetData()
{
	return m_data;
}

const OvMaths::FVector3& OvCore::ECS::Components::CLight::GetColor() const
{
	return m_data.color;
}

float OvCore::ECS::Components::CLight::GetIntensity() const
{
	return m_data.intensity;
}

void OvCore::ECS::Components::CLight::SetColor(const OvMaths::FVector3& p_color)
{
	m_data.color = p_color;
}

void OvCore::ECS::Components::CLight::SetIntensity(float p_intensity)
{
	m_data.intensity = p_intensity;
}

void OvCore::ECS::Components::CLight::OnSerialize(tinyxml2::XMLDocument & p_doc, tinyxml2::XMLNode * p_node)
{
	using namespace OvCore::Helpers;

	Serializer::SerializeVec3(p_doc, p_node, "color", m_data.color);
	Serializer::SerializeFloat(p_doc, p_node, "intensity", m_data.intensity);
}

void OvCore::ECS::Components::CLight::OnDeserialize(tinyxml2::XMLDocument & p_doc, tinyxml2::XMLNode * p_node)
{
	using namespace OvCore::Helpers;

	Serializer::DeserializeVec3(p_doc, p_node, "color", m_data.color);
	Serializer::DeserializeFloat(p_doc, p_node, "intensity", m_data.intensity);
}

void OvCore::ECS::Components::CLight::OnInspector(OvUI::Internal::WidgetContainer& p_root)
{
	using namespace OvCore::Helpers;

	GUIDrawer::DrawColor(p_root, "Color", reinterpret_cast<OvUI::Types::Color&>(m_data.color));
	GUIDrawer::DrawScalar<float>(p_root, "Intensity", m_data.intensity, 0.005f, GUIDrawer::_MIN_FLOAT, GUIDrawer::_MAX_FLOAT);
}
