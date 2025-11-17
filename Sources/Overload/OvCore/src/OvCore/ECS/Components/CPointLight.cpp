/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include <OvCore/ECS/Actor.h>
#include <OvCore/ECS/Components/CPointLight.h>

#include <OvUI/Widgets/Texts/Text.h>
#include <OvUI/Widgets/Drags/DragFloat.h>
#include <OvUI/Widgets/Selection/ColorEdit.h>
#include <OvUI/Widgets/Buttons/Button.h>
#include <OvUI/Widgets/Layout/Group.h>

OvCore::ECS::Components::CPointLight::CPointLight(ECS::Actor& p_owner) :
	CLight(p_owner)
{
	m_data.type = OvRendering::Settings::ELightType::POINT;
}

std::string OvCore::ECS::Components::CPointLight::GetName()
{
	return "Point Light";
}

float OvCore::ECS::Components::CPointLight::GetConstant() const
{
	return m_data.constant;
}

float OvCore::ECS::Components::CPointLight::GetLinear() const
{
	return m_data.linear;
}

float OvCore::ECS::Components::CPointLight::GetQuadratic() const
{
	return m_data.quadratic;
}

void OvCore::ECS::Components::CPointLight::SetConstant(float p_constant)
{
	m_data.constant = p_constant;
}

void OvCore::ECS::Components::CPointLight::SetLinear(float p_linear)
{
	m_data.linear = p_linear;
}

void OvCore::ECS::Components::CPointLight::SetQuadratic(float p_quadratic)
{
	m_data.quadratic = p_quadratic;
}

void OvCore::ECS::Components::CPointLight::OnSerialize(tinyxml2::XMLDocument & p_doc, tinyxml2::XMLNode * p_node)
{
	using namespace OvCore::Helpers;

	CLight::OnSerialize(p_doc, p_node);

	Serializer::SerializeFloat(p_doc, p_node, "constant", m_data.constant);
	Serializer::SerializeFloat(p_doc, p_node, "linear", m_data.linear);
	Serializer::SerializeFloat(p_doc, p_node, "quadratic", m_data.quadratic);
}

void OvCore::ECS::Components::CPointLight::OnDeserialize(tinyxml2::XMLDocument & p_doc, tinyxml2::XMLNode * p_node)
{
	using namespace OvCore::Helpers;

	CLight::OnDeserialize(p_doc, p_node);

	Serializer::DeserializeFloat(p_doc, p_node, "constant", m_data.constant);
	Serializer::DeserializeFloat(p_doc, p_node, "linear", m_data.linear);
	Serializer::DeserializeFloat(p_doc, p_node, "quadratic", m_data.quadratic);
}

void OvCore::ECS::Components::CPointLight::OnInspector(OvUI::Internal::WidgetContainer& p_root)
{
	using namespace OvCore::Helpers;

	CLight::OnInspector(p_root);

	GUIDrawer::CreateTitle(p_root, "Fall-off presets");
	auto& presetsRoot = p_root.CreateWidget<OvUI::Widgets::Layout::Group>();

	auto& constantPreset = presetsRoot.CreateWidget<OvUI::Widgets::Buttons::Button>("Constant");
	constantPreset.ClickedEvent += [this] { m_data.constant = 1.f, m_data.linear = m_data.quadratic = 0.f; };
	constantPreset.lineBreak = false;
	constantPreset.idleBackgroundColor = { 0.7f, 0.5f, 0.f };

	auto& linearPreset = presetsRoot.CreateWidget<OvUI::Widgets::Buttons::Button>("Linear");
	linearPreset.ClickedEvent += [this] { m_data.linear = 1.f, m_data.constant = m_data.quadratic = 0.f; };
	linearPreset.lineBreak = false;
	linearPreset.idleBackgroundColor = { 0.7f, 0.5f, 0.f };

	auto& quadraticPreset = presetsRoot.CreateWidget<OvUI::Widgets::Buttons::Button>("Quadratic");
	quadraticPreset.ClickedEvent += [this] { m_data.quadratic = 1.f, m_data.constant = m_data.linear = 0.f; };
	quadraticPreset.idleBackgroundColor = { 0.7f, 0.5f, 0.f };

	GUIDrawer::DrawScalar<float>(p_root, "Constant", m_data.constant, 0.005f, 0.f);
	GUIDrawer::DrawScalar<float>(p_root, "Linear", m_data.linear, 0.005f, 0.f);
	GUIDrawer::DrawScalar<float>(p_root, "Quadratic", m_data.quadratic, 0.005f, 0.f);
}
