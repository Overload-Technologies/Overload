/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include <OvRendering/Features/FrameInfoRenderFeature.h>

#include <OvCore/ECS/Components/CCamera.h>
#include <OvCore/Rendering/SceneRenderer.h>

#include "OvEditor/Panels/GameView.h"
#include "OvEditor/Core/EditorActions.h"
#include "OvEditor/Settings/EditorSettings.h"

OvEditor::Panels::GameView::GameView
(
	const std::string & p_title,
	bool p_opened,
	const OvUI::Settings::PanelWindowSettings & p_windowSettings
) :
	AView(p_title, p_opened, p_windowSettings),
	m_sceneManager(EDITOR_CONTEXT(sceneManager))
{
	m_renderer = std::make_unique<OvCore::Rendering::SceneRenderer>(*EDITOR_CONTEXT(driver));
	m_renderer->AddFeature<OvRendering::Features::FrameInfoRenderFeature>();
}

OvRendering::Entities::Camera* OvEditor::Panels::GameView::GetCamera()
{
	if (auto scene = m_sceneManager.GetCurrentScene())
	{
		if (auto camera = scene->FindMainCamera())
		{
			return &camera->GetCamera();
		}
	}

	return nullptr;
}

OvCore::SceneSystem::Scene* OvEditor::Panels::GameView::GetScene()
{
	return m_sceneManager.GetCurrentScene();
}

