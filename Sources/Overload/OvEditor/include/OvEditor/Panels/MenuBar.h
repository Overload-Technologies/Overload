/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include <OvUI/Panels/PanelMenuBar.h>
#include <OvUI/Panels/PanelWindow.h>
#include <OvUI/Widgets/Menu/MenuItem.h>

namespace OvEditor::Panels
{
	class MenuBar : public OvUI::Panels::PanelMenuBar
	{
		using PanelMap = std::unordered_map<std::string, std::pair<std::reference_wrapper<OvUI::Panels::PanelWindow>, std::reference_wrapper<OvUI::Widgets::Menu::MenuItem>>>;

	public:
		/**
		* Constructor
		*/
		MenuBar();
		
		/**
		* Check inputs for menubar shortcuts
		* @param p_deltaTime
		*/
		void HandleShortcuts(float p_deltaTime);

		/**
		* Register a panel to the menu bar window menu
		*/
		void RegisterPanel(const std::string& p_name, OvUI::Panels::PanelWindow& p_panel);

		/**
		* @note This needs to be called after all other panels have been intialized, as the content of other
		* panels is required to initialize some settings
		*/
		void InitializeSettingsMenu();

	private:
		void CreateFileMenu();
		void CreateBuildMenu();
		void CreateWindowMenu();
		void CreateActorsMenu();
		void CreateResourcesMenu();
		void CreateToolsMenu();
		void CreateSettingsMenu();
		void CreateLayoutMenu();
		void CreateHelpMenu();

		void UpdateToggleableItems();
		void OpenEveryWindows(bool p_state);

	private:
		PanelMap m_panels;
		OvUI::Widgets::Menu::MenuList* m_settingsMenu = nullptr;
		OvUI::Widgets::Menu::MenuList* m_windowMenu = nullptr;
	};
}