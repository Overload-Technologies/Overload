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

#include <OvTools/Utils/PathParser.h>
#include <OvUI/Panels/PanelWindow.h>

namespace OvUI::Widgets
{
	namespace InputFields { class InputText; }
	namespace Layout { class Group; }
	namespace Texts { class TextClickable; }
}

namespace OvEditor::Panels
{
	/**
	* A floating panel that lets the user pick an asset of a given type.
	* Open it with Open(fileType, callback) — on selection the callback receives the
	* resource-format path and the window closes automatically.
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
		* @param p_fileType    Asset type to show (UNKNOWN = all known types)
		* @param p_buttonMin   Screen-space top-left of the button that triggered the picker
		* @param p_buttonMax   Screen-space bottom-right of the button that triggered the picker
		* @param p_callback    Called with the selected resource path when the user picks an asset
		*/
		void Open(
			OvTools::Utils::PathParser::EFileType p_fileType,
			std::function<void(std::string)> p_callback
		);

	private:
		void Populate();
		void FilterList(const std::string& p_search);

	private:
		OvTools::Utils::PathParser::EFileType m_fileType = OvTools::Utils::PathParser::EFileType::UNKNOWN;
		std::function<void(std::string)> m_callback;

		OvUI::Widgets::InputFields::InputText* m_searchField = nullptr;
		OvUI::Widgets::Layout::Group* m_assetListGroup = nullptr;

		/* Each entry: (resource-format path, corresponding widget) */
		std::vector<std::pair<std::string, OvUI::Widgets::Texts::TextClickable*>> m_items;
	};
}
