/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include <variant>

#include <OvTools/Filesystem/IniFile.h>
#include <OvTools/Eventing/Event.h>

#include <OvUI/Widgets/Texts/Text.h>
#include <OvUI/Panels/PanelWindow.h>
#include <OvUI/Widgets/Layout/Group.h>
#include <OvUI/Widgets/Layout/Columns.h>
#include <OvUI/Widgets/Buttons/Button.h>

#include <OvRendering/Resources/Model.h>
#include <OvRendering/Resources/Texture.h>

namespace OvEditor::Panels
{
	class AssetProperties : public OvUI::Panels::PanelWindow
	{
	public:
		using EditableAssets = std::variant<OvRendering::Resources::Model*, OvRendering::Resources::Texture*>;

		/**
		* Constructor
		* @param p_title
		* @param p_opened
		* @param p_windowSettings
		*/
		AssetProperties(
			const std::string& p_title,
			bool p_opened,
			const OvUI::Settings::PanelWindowSettings& p_windowSettings
		);

		/**
		* Defines the target of the asset settings editor
		* @param p_path
		*/
		void SetTarget(const std::string& p_path);

        /**
        * Refresh the panel to show the current target settings
        */
        void Refresh();

		/**
		* Launch the preview of the target asset
		*/
		void Preview();

	private:
		void CreateHeaderButtons();
        void CreateAssetSelector();
		void CreateSettings();
		void CreateInfo();
		void CreateModelSettings();
		void CreateTextureSettings();
		void Apply();

	private:
		std::string m_resource;

        OvTools::Eventing::Event<> m_targetChanged;
        OvUI::Widgets::Layout::Group* m_settings = nullptr;
        OvUI::Widgets::Layout::Group* m_info = nullptr;
        OvUI::Widgets::Buttons::Button* m_applyButton = nullptr;
        OvUI::Widgets::Buttons::Button* m_revertButton = nullptr;
        OvUI::Widgets::Buttons::Button* m_previewButton = nullptr;
        OvUI::Widgets::Buttons::Button* m_resetButton = nullptr;
        OvUI::Widgets::AWidget* m_headerSeparator = nullptr;
        OvUI::Widgets::AWidget* m_headerLineBreak = nullptr;
		OvUI::Widgets::Layout::Columns<2>* m_settingsColumns = nullptr;
		OvUI::Widgets::Layout::Columns<2>* m_infoColumns = nullptr;
        OvUI::Widgets::Texts::Text* m_assetSelector = nullptr;
		std::unique_ptr<OvTools::Filesystem::IniFile> m_metadata;
	};
}