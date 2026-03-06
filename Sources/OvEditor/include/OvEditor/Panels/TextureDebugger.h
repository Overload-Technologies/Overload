/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include <unordered_map>
#include <memory>

#include <OvRendering/HAL/Texture.h>
#include <OvUI/Widgets/Visual/Image.h>
#include <OvUI/Widgets/Selection/ComboBox.h>
#include <OvUI/Panels/PanelWindow.h>
#include <OvEditor/Utils/TextureRegistry.h>

namespace OvEditor::Panels
{
	enum class EScaleMode : uint8_t
	{
		ONE_TO_ONE,
		PERCENTAGE_5,
		PERCENTAGE_10,
		PERCENTAGE_25,
		PERCENTAGE_50,
		PERCENTAGE_100,
		PERCENTAGE_200,
		PERCENTAGE_400,
		PERCENTAGE_800
	};

	class TextureDebugger : public OvUI::Panels::PanelWindow
	{
	public:
		/**
		* Creates a texture debugger.
		* @param p_title
		* @param p_opened
		* @param p_windowSettings
		*/
		TextureDebugger(
			const std::string& p_title,
			bool p_opened,
			const OvUI::Settings::PanelWindowSettings& p_windowSettings
		);

		/**
		* Destroys the texture debugger.
		*/
		~TextureDebugger();

		/**
		* Updates the texture debugger.
		* @param p_deltaTime
		*/
		void Update(float p_deltaTime);

	private:
		EScaleMode m_scaleMode = EScaleMode::ONE_TO_ONE;
		OvUI::Widgets::Selection::ComboBox& m_textureSelector;
		OvUI::Widgets::Selection::ComboBox& m_scaleSelector;
		OvUI::Widgets::Visual::Image& m_image;
		OvTools::Eventing::ListenerID m_creationListenerID;
		OvTools::Eventing::ListenerID m_destructionListenerID;
		OvTools::Utils::OptRef<OvRendering::HAL::Texture> m_selectedTexture;
	};
}
