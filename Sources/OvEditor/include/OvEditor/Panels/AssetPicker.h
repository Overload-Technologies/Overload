/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include <filesystem>
#include <functional>
#include <string>
#include <vector>

#include <OvCore/Helpers/GUIDrawer.h>
#include <OvTools/Utils/PathParser.h>
#include <OvUI/Panels/PanelWindow.h>

namespace OvUI::Widgets
{
	namespace InputFields { class InputText; }
	namespace Layout { class Group; }
}

namespace OvEditor::Panels
{
	/**
	* A floating panel that lets the user pick an asset of a given type,
	* or an arbitrary pre-built list of items (e.g. components + scripts).
	*/
	class AssetPicker : public OvUI::Panels::PanelWindow
	{
	public:
		AssetPicker(
			const std::string& p_title,
			bool p_opened,
			const OvUI::Settings::PanelWindowSettings& p_windowSettings
		);

		/**
		* Open the picker filtered by the given file type.
		* @param p_fileType           Asset type to show (UNKNOWN = all known types)
		* @param p_callback           Called with the selected resource path when the user picks an asset
		* @param p_searchProjectFiles Include project assets in the results
		* @param p_searchEngineFiles  Include engine assets in the results
		*/
		void Open(
			OvTools::Utils::PathParser::EFileType p_fileType,
			std::function<void(std::string)> p_callback,
			bool p_searchProjectFiles = true,
			bool p_searchEngineFiles = true
		);

		/**
		* Open the picker with a pre-built list of items (e.g. components + scripts).
		* Each item carries its own icon and selection callback.
		* @param p_items
		*/
		void Open(std::vector<OvCore::Helpers::GUIDrawer::PickerItem> p_items);

	private:
		enum class EMode { FileBased, PreBuilt };

		void Populate();
		void FilterList(const std::string& p_search);

		void OpenInternal();

	private:
		EMode m_mode = EMode::FileBased;

		/* FileBased mode */
		OvTools::Utils::PathParser::EFileType m_fileType = OvTools::Utils::PathParser::EFileType::UNKNOWN;
		bool m_searchProjectFiles = true;
		bool m_searchEngineFiles = true;
		std::function<void(std::string)> m_callback;

		/* PreBuilt mode */
		std::vector<OvCore::Helpers::GUIDrawer::PickerItem> m_prebuiltItems;

		OvUI::Widgets::InputFields::InputText* m_searchField = nullptr;
		OvUI::Widgets::Layout::Group* m_assetListGroup = nullptr;

		/* Each entry: (search key, row group widget) */
		std::vector<std::pair<std::string, OvUI::Widgets::Layout::Group*>> m_items;
	};
}
