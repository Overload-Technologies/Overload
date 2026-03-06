/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include <OvWindowing/Context/Device.h>
#include <OvWindowing/Window.h>
#include <OvRendering/Context/Driver.h>
#include <OvUI/Core/UIManager.h>
#include <OvUI/Panels/PanelWindow.h>

namespace OvEditor::Core
{
	struct ProjectHubResult
	{
		std::filesystem::path projectPath;
	};

	/**
	* A simple panel that allow the user to select the project to launch
	*/
	class ProjectHub
	{
	public:
		/**
		* Constructor
		*/
		ProjectHub();

		/**
		* Run the project hub logic
		*/
		std::optional<ProjectHubResult> Run();

		/**
		* Setup the project hub specific context (minimalist context)
		*/
		void SetupContext();

	private:
		std::unique_ptr<OvWindowing::Context::Device> m_device;
		std::unique_ptr<OvWindowing::Window> m_window;
		std::unique_ptr<OvRendering::Context::Driver> m_driver;
		std::unique_ptr<OvUI::Core::UIManager> m_uiManager;
		OvUI::Modules::Canvas m_canvas;
	};
}