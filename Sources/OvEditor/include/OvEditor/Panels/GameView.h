/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include <OvCore/SceneSystem/SceneManager.h>

#include "OvEditor/Panels/AView.h"

namespace OvEditor::Panels
{
	class GameView : public OvEditor::Panels::AView
	{
	public:
		/**
		* Constructor
		* @param p_title
		* @param p_opened
		* @param p_windowSettings
		*/
		GameView(
			const std::string& p_title,
			bool p_opened,
			const OvUI::Settings::PanelWindowSettings& p_windowSettings
		);

		/**
		* Returns the main camera used by the attached scene
		*/
		virtual OvRendering::Entities::Camera* GetCamera();

		/**
		* Returns the scene used by this view
		*/
		virtual OvCore::SceneSystem::Scene* GetScene();

	private:
		OvCore::SceneSystem::SceneManager& m_sceneManager;
	};
}