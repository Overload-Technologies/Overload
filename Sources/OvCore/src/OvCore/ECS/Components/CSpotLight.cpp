/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include <OvCore/ECS/Actor.h>
#include <OvCore/ECS/Components/CSpotLight.h>

#include <OvUI/Widgets/Buttons/Button.h>
#include <OvUI/Widgets/Drags/DragFloat.h>
#include <OvUI/Widgets/Layout/Group.h>
#include <OvUI/Widgets/Selection/ColorEdit.h>
#include <OvUI/Widgets/Texts/Text.h>

OvCore::ECS::Components::CSpotLight::CSpotLight(ECS::Actor & p_owner) :
	CLight(p_owner)
{
	m_data.type = OvRendering::Settings::ELightType::SPOT;
}

std::string OvCore::ECS::Components::CSpotLight::GetName()
{
	return "Spot Light";
}

std::string OvCore::ECS::Components::CSpotLight::GetTypeName()
{
	return std::string{ComponentTraits<CSpotLight>::Name};
}

float OvCore::ECS::Components::CSpotLight::GetConstant() const
{
	return m_data.constant;
}

float OvCore::ECS::Components::CSpotLight::GetLinear() const
{
	return m_data.linear;
}

float OvCore::ECS::Components::CSpotLight::GetQuadratic() const
{
	return m_data.quadratic;
}

float OvCore::ECS::Components::CSpotLight::GetCutoff() const
{
	return m_data.cutoff;
}

float OvCore::ECS::Components::CSpotLight::GetOuterCutoff() const
{
	return m_data.outerCutoff;
}

void OvCore::ECS::Components::CSpotLight::SetConstant(float p_constant)
{
	m_data.constant = p_constant;
}

void OvCore::ECS::Components::CSpotLight::SetLinear(float p_linear)
{
	m_data.linear = p_linear;
}

void OvCore::ECS::Components::CSpotLight::SetQuadratic(float p_quadratic)
{
	m_data.quadratic = p_quadratic;
}

void OvCore::ECS::Components::CSpotLight::SetCutoff(float p_cutoff)
{
	m_data.cutoff = p_cutoff;
}

void OvCore::ECS::Components::CSpotLight::SetOuterCutoff(float p_outerCutoff)
{
	m_data.outerCutoff = p_outerCutoff;
}

void OvCore::ECS::Components::CSpotLight::OnSerialize(tinyxml2::XMLDocument & p_doc, tinyxml2::XMLNode * p_node)
{
	using namespace OvCore::Helpers;

	CLight::OnSerialize(p_doc, p_node);

	Serializer::SerializeFloat(p_doc, p_node, "constant", m_data.constant);
	Serializer::SerializeFloat(p_doc, p_node, "linear", m_data.linear);
	Serializer::SerializeFloat(p_doc, p_node, "quadratic", m_data.quadratic);
	Serializer::SerializeFloat(p_doc, p_node, "cutoff", m_data.cutoff);
	Serializer::SerializeFloat(p_doc, p_node, "outercutoff", m_data.outerCutoff);
}

void OvCore::ECS::Components::CSpotLight::OnDeserialize(tinyxml2::XMLDocument & p_doc, tinyxml2::XMLNode * p_node)
{
	using namespace OvCore::Helpers;

	CLight::OnDeserialize(p_doc, p_node);

	Serializer::DeserializeFloat(p_doc, p_node, "constant", m_data.constant);
	Serializer::DeserializeFloat(p_doc, p_node, "linear", m_data.linear);
	Serializer::DeserializeFloat(p_doc, p_node, "quadratic", m_data.quadratic);
	Serializer::DeserializeFloat(p_doc, p_node, "cutoff", m_data.cutoff);
	Serializer::DeserializeFloat(p_doc, p_node, "outercutoff", m_data.outerCutoff);
}

void OvCore::ECS::Components::CSpotLight::OnInspector(OvUI::Internal::WidgetContainer& p_root)
{
	using namespace OvCore::Helpers;

	CLight::OnInspector(p_root);

	GUIDrawer::DrawScalar<float>(p_root, "Cut-Off", m_data.cutoff, 0.1f, 0.f, 180.f);
	GUIDrawer::DrawScalar<float>(p_root, "Outer Cut-Off", m_data.outerCutoff, 0.1f, 0.f, 180.f);

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
