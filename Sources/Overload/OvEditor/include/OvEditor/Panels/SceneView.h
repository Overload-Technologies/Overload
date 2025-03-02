/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include "OvEditor/Panels/AViewControllable.h"
#include "OvEditor/Core/GizmoBehaviour.h"

namespace OvEditor::Panels
{
	class SceneView : public OvEditor::Panels::AViewControllable
	{
	public:
		/**
		* Constructor
		* @param p_title
		* @param p_opened
		* @param p_windowSettings
		*/
		SceneView(
			const std::string& p_title,
			bool p_opened,
			const OvUI::Settings::PanelWindowSettings& p_windowSettings
		);

		/**
		* Update the scene view
		*/
		virtual void Update(float p_deltaTime) override;

		/**
		* Prepare the renderer for rendering
		*/
		virtual void InitFrame() override;

		/**
		* Returns the scene used by this view
		*/
		virtual OvCore::SceneSystem::Scene* GetScene();

	protected:
		virtual OvCore::Rendering::SceneRenderer::SceneDescriptor CreateSceneDescriptor() override;

	private:
		virtual void DrawFrame() override;
		void HandleActorPicking();

	private:
		OvCore::SceneSystem::SceneManager& m_sceneManager;
		OvEditor::Core::GizmoBehaviour m_gizmoOperations;
		OvEditor::Core::EGizmoOperation m_currentOperation = OvEditor::Core::EGizmoOperation::TRANSLATE;
		OvCore::Resources::Material m_fallbackMaterial;

		OvTools::Utils::OptRef<OvCore::ECS::Actor> m_highlightedActor;
		std::optional<OvEditor::Core::GizmoBehaviour::EDirection> m_highlightedGizmoDirection;
	};
}