/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include <OvUI/Widgets/Texts/Text.h>
#include <OvUI/Panels/PanelWindow.h>
#include <OvUI/Widgets/Layout/Group.h>
#include <OvUI/Widgets/Layout/Columns.h>
#include <OvRendering/Resources/Shader.h>

namespace OvCore::Resources { class Material; }

namespace OvEditor::Panels
{
	class MaterialEditor : public OvUI::Panels::PanelWindow
	{
	public:
		/**
		* Constructor
		* @param p_title
		* @param p_opened
		* @param p_windowSettings
		*/
		MaterialEditor
		(
			const std::string& p_title,
			bool p_opened,
			const OvUI::Settings::PanelWindowSettings& p_windowSettings
		);

		/**
		* Refresh the material editor
		*/
		void Refresh();

		/**
		* Defines the target material of the material editor
		* @param p_newTarget
		*/
		void SetTarget(OvCore::Resources::Material& p_newTarget);

		/**
		* Returns the target of the material editor
		*/
		OvCore::Resources::Material* GetTarget() const;

		/**
		* Remove the target of the material editor (Clear the material editor)
		*/
		void RemoveTarget();

		/**
		* Launch the preview of the currently targeted material
		*/
		void Preview();

		/**
		* Reset material
		*/
		void Reset();
		
	private:
		void OnMaterialDropped();
		void OnShaderDropped();

		void CreateHeaderButtons();
		void CreateMaterialSelector();
		void CreateShaderSelector();
		void CreateMaterialSettings();
		void CreateShaderSettings();
		void CreateFeatureSettings();

		void GenerateShaderSettingsContent();
		void GenerateMaterialSettingsContent();
		void GenerateMaterialFeaturesContent();

	private:
		OvCore::Resources::Material* m_target		= nullptr;
		OvRendering::Resources::Shader* m_shader	= nullptr;

		OvUI::Widgets::Texts::Text* m_targetMaterialText	= nullptr;
		OvUI::Widgets::Texts::Text* m_shaderText			= nullptr;

		OvTools::Eventing::Event<> m_materialDroppedEvent;
		OvTools::Eventing::Event<> m_shaderDroppedEvent;

		OvUI::Widgets::Layout::Group* m_settings			= nullptr;
		OvUI::Widgets::Layout::Group* m_materialSettings	= nullptr;
		OvUI::Widgets::Layout::Group* m_shaderSettings = nullptr;
		OvUI::Widgets::Layout::Group* m_featureSettings = nullptr;

		OvUI::Widgets::Layout::Columns<2>* m_shaderSettingsColumns = nullptr;
		OvUI::Widgets::Layout::Columns<2>* m_materialSettingsColumns = nullptr;
		OvUI::Widgets::Layout::Columns<2>* m_featureSettingsColumns = nullptr;
	};
}