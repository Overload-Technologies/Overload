/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include "OvEditor/Panels/AViewControllable.h"
#include "OvEditor/Rendering/DebugSceneRenderer.h"
#include "OvEditor/Rendering/GridRenderPass.h"
#include "OvEditor/Core/EditorActions.h"
#include "OvEditor/Settings/EditorSettings.h"

const OvMaths::FVector3 kDefaultGridColor{ 1.0f, 1.0f, 1.0f };
const OvMaths::FVector3 kDefaultClearColor{ 0.0f, 0.0f, 0.0f };
const OvMaths::FVector3 kDefaultCameraPosition{ -10.0f, 3.0f, 10.0f };
const OvMaths::FQuaternion kDefaultCameraRotation({ 0.0f, 135.0f, 0.0f });

OvEditor::Panels::AViewControllable::AViewControllable(
	const std::string& p_title,
	bool p_opened,
	const OvUI::Settings::PanelWindowSettings& p_windowSettings
) :
	AView(p_title, p_opened, p_windowSettings),
	m_cameraController(*this, m_camera)
{
	ResetCameraTransform();
	ResetGridColor();
	ResetClearColor();
}

void OvEditor::Panels::AViewControllable::Update(float p_deltaTime)
{
	m_cameraController.HandleInputs(p_deltaTime);
	AView::Update(p_deltaTime);
}

void OvEditor::Panels::AViewControllable::InitFrame()
{
	m_camera.SetFrustumGeometryCulling(Settings::EditorSettings::EditorFrustumGeometryCulling);
	m_camera.SetFrustumLightCulling(Settings::EditorSettings::EditorFrustumLightCulling);

	AView::InitFrame();

	m_renderer->AddDescriptor<Rendering::GridRenderPass::GridDescriptor>({
		m_gridColor,
		m_camera.GetPosition()
	});
}

void OvEditor::Panels::AViewControllable::ResetCameraTransform()
{
	m_camera.transform->SetWorldPosition(kDefaultCameraPosition);
	m_camera.transform->SetWorldRotation(kDefaultCameraRotation);
}

OvEditor::Core::CameraController& OvEditor::Panels::AViewControllable::GetCameraController()
{
	return m_cameraController;
}

OvRendering::Entities::Camera* OvEditor::Panels::AViewControllable::GetCamera()
{
	return &m_camera;
}

const OvMaths::FVector3& OvEditor::Panels::AViewControllable::GetGridColor() const
{
	return m_gridColor;
}

void OvEditor::Panels::AViewControllable::SetGridColor(const OvMaths::FVector3& p_color)
{
	m_gridColor = p_color;
}

void OvEditor::Panels::AViewControllable::ResetGridColor()
{
	m_gridColor = kDefaultGridColor;
}

void OvEditor::Panels::AViewControllable::ResetClearColor()
{
	m_camera.SetClearColor(kDefaultClearColor);
}
