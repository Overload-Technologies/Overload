/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include <filesystem>
#include <queue>
#include <unordered_map>

#include <OvRendering/Resources/Loaders/TextureLoader.h>
#include <OvUI/Panels/PanelWindow.h>
#include <OvUI/Widgets/Layout/TreeNode.h>
#include <OvUI/Widgets/Layout/Group.h>

namespace OvEditor::Panels
{
	/**
	* A panel that handle asset management
	*/
	class AssetBrowser : public OvUI::Panels::PanelWindow
	{
	public:
		/**
		* Constructor
		* @param p_title
		* @parma p_opened
		* @param p_windowSettings
		*/
		AssetBrowser(
			const std::string& p_title,
			bool p_opened,
			const OvUI::Settings::PanelWindowSettings& p_windowSettings
		);

		/**
		* Fill the asset browser panels with widgets corresponding to elements in the asset folder
		*/
		void Fill();

		/**
		* Clear the asset browser widgets
		*/
		void Clear();

		/**
		* Refresh the asset browser widgets (Clear + Fill)
		*/
		void Refresh();

	private:
		void ParseFolder(OvUI::Widgets::Layout::TreeNode& p_root, const std::filesystem::directory_entry& p_directory, bool p_isEngineItem, bool p_scriptFolder = false);
		void ConsiderItem(OvUI::Widgets::Layout::TreeNode* p_root, const std::filesystem::directory_entry& p_entry, bool p_isEngineItem, bool p_autoOpen = false, bool p_scriptFolder = false);

	private:
		OvUI::Widgets::Layout::Group* m_assetList;
		std::unordered_map<OvUI::Widgets::Layout::TreeNode*, std::filesystem::path> m_pathUpdate;
	};
}