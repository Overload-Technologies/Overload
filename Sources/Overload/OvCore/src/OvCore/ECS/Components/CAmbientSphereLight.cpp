/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include <OvCore/ECS/Actor.h>
#include <OvCore/ECS/Components/CAmbientSphereLight.h>

#include <OvUI/Widgets/Texts/Text.h>
#include <OvUI/Widgets/Drags/DragFloat.h>
#include <OvUI/Widgets/Selection/ColorEdit.h>
#include <OvUI/Widgets/Buttons/Button.h>

OvCore::ECS::Components::CAmbientSphereLight::CAmbientSphereLight(ECS::Actor & p_owner) :
	CLight(p_owner)
{
	m_data.intensity = 0.03f;
	m_data.constant = 1.0f;

	m_data.type = OvRendering::Settings::ELightType::AMBIENT_SPHERE;
}

std::string OvCore::ECS::Components::CAmbientSphereLight::GetName()
{
	return "Ambient Sphere Light";
}

std::string OvCore::ECS::Components::CAmbientSphereLight::GetTypeName()
{
	return std::string{ComponentTraits<CAmbientSphereLight>::Name};
}

float OvCore::ECS::Components::CAmbientSphereLight::GetRadius() const
{
	return m_data.quadratic;
}

void OvCore::ECS::Components::CAmbientSphereLight::SetRadius(float p_radius)
{
	m_data.constant = p_radius;
}

void OvCore::ECS::Components::CAmbientSphereLight::OnSerialize(tinyxml2::XMLDocument & p_doc, tinyxml2::XMLNode * p_node)
{
	using namespace OvCore::Helpers;

	CLight::OnSerialize(p_doc, p_node);

	Serializer::SerializeFloat(p_doc, p_node, "radius", m_data.constant);
}

void OvCore::ECS::Components::CAmbientSphereLight::OnDeserialize(tinyxml2::XMLDocument & p_doc, tinyxml2::XMLNode * p_node)
{
	using namespace OvCore::Helpers;

	CLight::OnDeserialize(p_doc, p_node);

	Serializer::DeserializeFloat(p_doc, p_node, "radius", m_data.constant);
}

void OvCore::ECS::Components::CAmbientSphereLight::OnInspector(OvUI::Internal::WidgetContainer& p_root)
{
	using namespace OvCore::Helpers;

	CLight::OnInspector(p_root);

	GUIDrawer::DrawScalar<float>(p_root, "Radius", m_data.constant, 0.1f, 0.f);
}
