/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include <OvCore/ECS/Actor.h>
#include <OvCore/ECS/Components/CDirectionalLight.h>

#include <OvUI/Widgets/Texts/Text.h>
#include <OvUI/Widgets/Drags/DragFloat.h>
#include <OvUI/Widgets/Selection/ColorEdit.h>
#include <OvUI/Widgets/Selection/ComboBox.h>

OvCore::ECS::Components::CDirectionalLight::CDirectionalLight(ECS::Actor & p_owner) :
	CLight(p_owner)
{
	m_data.type = OvRendering::Settings::ELightType::DIRECTIONAL;
	m_data.castShadows = true;
}

std::string OvCore::ECS::Components::CDirectionalLight::GetName()
{
	return "Directional Light";
}

void OvCore::ECS::Components::CDirectionalLight::SetCastShadows(bool p_enabled)
{
	m_data.castShadows = p_enabled;
}

bool OvCore::ECS::Components::CDirectionalLight::GetCastShadows() const
{
	return m_data.castShadows;
}

void OvCore::ECS::Components::CDirectionalLight::SetShadowAreaSize(float p_shadowAreaSize)
{
	m_data.shadowAreaSize = p_shadowAreaSize;
}

float OvCore::ECS::Components::CDirectionalLight::GetShadowAreaSize() const
{
	return m_data.shadowAreaSize;
}

void OvCore::ECS::Components::CDirectionalLight::SetShadowFollowCamera(bool p_enabled)
{
	m_data.shadowFollowCamera = p_enabled;
}

bool OvCore::ECS::Components::CDirectionalLight::GetShadowFollowCamera() const
{
	return m_data.shadowFollowCamera;
}

void OvCore::ECS::Components::CDirectionalLight::SetShadowMapResolution(uint32_t p_resolution)
{
	m_data.shadowMapResolution = p_resolution;
}

uint32_t OvCore::ECS::Components::CDirectionalLight::GetShadowMapResolution() const
{
	return m_data.shadowMapResolution;
}

void OvCore::ECS::Components::CDirectionalLight::OnSerialize(tinyxml2::XMLDocument & p_doc, tinyxml2::XMLNode * p_node)
{
	CLight::OnSerialize(p_doc, p_node);
	OvCore::Helpers::Serializer::SerializeBoolean(p_doc, p_node, "cast_shadows", m_data.castShadows);
	OvCore::Helpers::Serializer::SerializeFloat(p_doc, p_node, "shadow_area_size", m_data.shadowAreaSize);
	OvCore::Helpers::Serializer::SerializeBoolean(p_doc, p_node, "shadow_follow_camera", m_data.shadowFollowCamera);
	OvCore::Helpers::Serializer::SerializeInt(p_doc, p_node, "shadow_map_resolution", m_data.shadowMapResolution);
}

void OvCore::ECS::Components::CDirectionalLight::OnDeserialize(tinyxml2::XMLDocument & p_doc, tinyxml2::XMLNode * p_node)
{
	CLight::OnDeserialize(p_doc, p_node);
	m_data.castShadows = OvCore::Helpers::Serializer::DeserializeBoolean(p_doc, p_node, "cast_shadows");
	m_data.shadowAreaSize = OvCore::Helpers::Serializer::DeserializeFloat(p_doc, p_node, "shadow_area_size");
	m_data.shadowFollowCamera = OvCore::Helpers::Serializer::DeserializeBoolean(p_doc, p_node, "shadow_follow_camera");
	m_data.shadowMapResolution = OvCore::Helpers::Serializer::DeserializeInt(p_doc, p_node, "shadow_map_resolution");
}

void OvCore::ECS::Components::CDirectionalLight::OnInspector(OvUI::Internal::WidgetContainer& p_root)
{
	CLight::OnInspector(p_root);
	OvCore::Helpers::GUIDrawer::DrawBoolean(p_root, "Cast Shadows", m_data.castShadows);
	OvCore::Helpers::GUIDrawer::DrawScalar(p_root, "Shadow Area Size", m_data.shadowAreaSize);
	OvCore::Helpers::GUIDrawer::DrawBoolean(p_root, "Shadow Follow Camera", m_data.shadowFollowCamera);
	
	Helpers::GUIDrawer::CreateTitle(p_root, "Shadow Map Resolution");

	auto& shadowMapResolution = p_root.CreateWidget<OvUI::Widgets::Selection::ComboBox>(m_data.shadowMapResolution);
	shadowMapResolution.choices = {
		{ 512, "512" },
		{ 1024, "1024" },
		{ 2048, "2048" },
		{ 4096, "4096" },
		{ 8192, "8192" },
		{ 16384, "16384 (Experimental)" }
	};

	auto& shadowMapResolutionDispatcher = shadowMapResolution.AddPlugin<OvUI::Plugins::DataDispatcher<int>>();
	shadowMapResolutionDispatcher.RegisterGatherer([this]() { return m_data.shadowMapResolution; });
	shadowMapResolutionDispatcher.RegisterProvider([this](int p_choice) { m_data.shadowMapResolution = p_choice; });
}
