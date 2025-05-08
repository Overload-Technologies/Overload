/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include <tinyxml2.h>

#include <OvCore/ECS/Actor.h>
#include <OvCore/ECS/Components/CCamera.h>

#include <OvUI/Plugins/DataDispatcher.h>
#include <OvUI/Widgets/Drags/DragFloat.h>
#include <OvUI/Widgets/Selection/ComboBox.h>

OvCore::ECS::Components::CCamera::CCamera(ECS::Actor& p_owner)
	: AComponent(p_owner), m_camera{ p_owner.transform.GetFTransform() }
{
	/* Default clear color for the CCamera (Different from Camera default clear color) */
	SetClearColor({ 0.1921569f, 0.3019608f, 0.4745098f });
}

std::string OvCore::ECS::Components::CCamera::GetName()
{
	return "Camera";
}

void OvCore::ECS::Components::CCamera::SetFov(float p_value)
{
	m_camera.SetFov(p_value);
}

void OvCore::ECS::Components::CCamera::SetSize(float p_value)
{
    m_camera.SetSize(p_value);
}

void OvCore::ECS::Components::CCamera::SetNear(float p_value)
{
	m_camera.SetNear(p_value);
}

void OvCore::ECS::Components::CCamera::SetFar(float p_value)
{
	m_camera.SetFar(p_value);
}

void OvCore::ECS::Components::CCamera::SetFrustumGeometryCulling(bool p_enable)
{
	m_camera.SetFrustumGeometryCulling(p_enable);
}

void OvCore::ECS::Components::CCamera::SetFrustumLightCulling(bool p_enable)
{
	m_camera.SetFrustumLightCulling(p_enable);
}

void OvCore::ECS::Components::CCamera::SetProjectionMode(OvRendering::Settings::EProjectionMode p_projectionMode)
{
    m_camera.SetProjectionMode(p_projectionMode);
}

float OvCore::ECS::Components::CCamera::GetFov() const
{
	return m_camera.GetFov();
}

float OvCore::ECS::Components::CCamera::GetSize() const
{
    return m_camera.GetSize();
}

float OvCore::ECS::Components::CCamera::GetNear() const
{
	return m_camera.GetNear();
}

float OvCore::ECS::Components::CCamera::GetFar() const
{
	return m_camera.GetFar();
}

const OvMaths::FVector3 & OvCore::ECS::Components::CCamera::GetClearColor() const
{
	return m_camera.GetClearColor();
}

bool OvCore::ECS::Components::CCamera::HasFrustumGeometryCulling() const
{
	return m_camera.HasFrustumGeometryCulling();
}

void OvCore::ECS::Components::CCamera::SetClearColor(const OvMaths::FVector3 & p_clearColor)
{
	m_camera.SetClearColor(p_clearColor);
}

bool OvCore::ECS::Components::CCamera::HasFrustumLightCulling() const
{
	return m_camera.HasFrustumLightCulling();
}

OvRendering::Settings::EProjectionMode OvCore::ECS::Components::CCamera::GetProjectionMode() const
{
    return m_camera.GetProjectionMode();
}

OvRendering::Entities::Camera & OvCore::ECS::Components::CCamera::GetCamera()
{
	return m_camera;
}

void OvCore::ECS::Components::CCamera::OnSerialize(tinyxml2::XMLDocument & p_doc, tinyxml2::XMLNode * p_node)
{
	OvCore::Helpers::Serializer::SerializeFloat(p_doc, p_node, "fov", m_camera.GetFov());
	OvCore::Helpers::Serializer::SerializeFloat(p_doc, p_node, "size", m_camera.GetSize());
	OvCore::Helpers::Serializer::SerializeFloat(p_doc, p_node, "near", m_camera.GetNear());
	OvCore::Helpers::Serializer::SerializeFloat(p_doc, p_node, "far", m_camera.GetFar());
	OvCore::Helpers::Serializer::SerializeVec3(p_doc, p_node, "clear_color", m_camera.GetClearColor());
	OvCore::Helpers::Serializer::SerializeBoolean(p_doc, p_node, "frustum_geometry_culling", m_camera.HasFrustumGeometryCulling());
	OvCore::Helpers::Serializer::SerializeBoolean(p_doc, p_node, "frustum_light_culling", m_camera.HasFrustumLightCulling());
	OvCore::Helpers::Serializer::SerializeInt(p_doc, p_node, "projection_mode", static_cast<int>(m_camera.GetProjectionMode()));
}

void OvCore::ECS::Components::CCamera::OnDeserialize(tinyxml2::XMLDocument & p_doc, tinyxml2::XMLNode * p_node)
{
	m_camera.SetFov(OvCore::Helpers::Serializer::DeserializeFloat(p_doc, p_node, "fov"));
	m_camera.SetSize(OvCore::Helpers::Serializer::DeserializeFloat(p_doc, p_node, "size"));
	m_camera.SetNear(OvCore::Helpers::Serializer::DeserializeFloat(p_doc, p_node, "near"));
	m_camera.SetFar(OvCore::Helpers::Serializer::DeserializeFloat(p_doc, p_node, "far"));
	m_camera.SetClearColor(OvCore::Helpers::Serializer::DeserializeVec3(p_doc, p_node, "clear_color"));
	m_camera.SetFrustumGeometryCulling(OvCore::Helpers::Serializer::DeserializeBoolean(p_doc, p_node, "frustum_geometry_culling"));
	m_camera.SetFrustumLightCulling(OvCore::Helpers::Serializer::DeserializeBoolean(p_doc, p_node, "frustum_light_culling"));

    // We have to make sure the "projection_mode" exists in the serialized component, otherwise we do not want to modify the default setting (Perspective).
    // This is a bad practice to have each components calling setters in `OnDeserialize` even if no XML node hasn't been found for a given property.
    // We should rework this system later. As it is out of the scope of the orthographic projection scope, this will be left as is for now.
    if (p_node->FirstChildElement("projection_mode"))
    {
        m_camera.SetProjectionMode(static_cast<OvRendering::Settings::EProjectionMode>(OvCore::Helpers::Serializer::DeserializeInt(p_doc, p_node, "projection_mode")));
    }
}

void OvCore::ECS::Components::CCamera::OnInspector(OvUI::Internal::WidgetContainer& p_root)
{
    auto currentProjectionMode = GetProjectionMode();

	OvCore::Helpers::GUIDrawer::DrawScalar<float>(p_root, "Field of view", std::bind(&CCamera::GetFov, this), std::bind(&CCamera::SetFov, this, std::placeholders::_1));
    auto& fovWidget = *p_root.GetWidgets()[p_root.GetWidgets().size() - 1].first;
    auto& fovWidgetLabel = *p_root.GetWidgets()[p_root.GetWidgets().size() - 2].first;
    fovWidget.enabled = fovWidgetLabel.enabled = currentProjectionMode == OvRendering::Settings::EProjectionMode::PERSPECTIVE;

	OvCore::Helpers::GUIDrawer::DrawScalar<float>(p_root, "Size", std::bind(&CCamera::GetSize, this), std::bind(&CCamera::SetSize, this, std::placeholders::_1));
    auto& sizeWidget = *p_root.GetWidgets()[p_root.GetWidgets().size() - 1].first;
    auto& sizeWidgetLabel = *p_root.GetWidgets()[p_root.GetWidgets().size() - 2].first;
    sizeWidget.enabled = sizeWidgetLabel.enabled = currentProjectionMode == OvRendering::Settings::EProjectionMode::ORTHOGRAPHIC;

	OvCore::Helpers::GUIDrawer::DrawScalar<float>(p_root, "Near", std::bind(&CCamera::GetNear, this), std::bind(&CCamera::SetNear, this, std::placeholders::_1));
	OvCore::Helpers::GUIDrawer::DrawScalar<float>(p_root, "Far", std::bind(&CCamera::GetFar, this), std::bind(&CCamera::SetFar, this, std::placeholders::_1));
	OvCore::Helpers::GUIDrawer::DrawColor(p_root, "Clear color", [this]() {return reinterpret_cast<const OvUI::Types::Color&>(GetClearColor()); }, [this](OvUI::Types::Color p_color) { SetClearColor({ p_color.r, p_color.g, p_color.b }); }, false);
	OvCore::Helpers::GUIDrawer::DrawBoolean(p_root, "Frustum Geometry Culling", std::bind(&CCamera::HasFrustumGeometryCulling, this), std::bind(&CCamera::SetFrustumGeometryCulling, this, std::placeholders::_1));
	OvCore::Helpers::GUIDrawer::DrawBoolean(p_root, "Frustum Light Culling", std::bind(&CCamera::HasFrustumLightCulling, this), std::bind(&CCamera::SetFrustumLightCulling, this, std::placeholders::_1));

    Helpers::GUIDrawer::CreateTitle(p_root, "Projection Mode");
    auto& projectionMode = p_root.CreateWidget<OvUI::Widgets::Selection::ComboBox>(static_cast<int>(GetProjectionMode()));
    projectionMode.choices.emplace(0, "Orthographic");
    projectionMode.choices.emplace(1, "Perspective");
    projectionMode.ValueChangedEvent += [this, &fovWidget, &fovWidgetLabel, &sizeWidget, &sizeWidgetLabel](int p_choice)
    {
        const auto newProjectionMode = static_cast<OvRendering::Settings::EProjectionMode>(p_choice);
        SetProjectionMode(newProjectionMode);
        fovWidget.enabled = fovWidgetLabel.enabled = newProjectionMode == OvRendering::Settings::EProjectionMode::PERSPECTIVE;
        sizeWidget.enabled = sizeWidgetLabel.enabled = newProjectionMode == OvRendering::Settings::EProjectionMode::ORTHOGRAPHIC;
    };
}
