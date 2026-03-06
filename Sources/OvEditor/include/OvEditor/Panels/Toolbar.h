/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include <OvUI/Widgets/Buttons/ButtonImage.h>
#include <OvUI/Panels/PanelWindow.h>

namespace OvEditor::Panels
{
	class Toolbar : public OvUI::Panels::PanelWindow
	{
	public:
		/**
		* Constructor
		* @param p_title
		* @param p_opened
		* @param p_windowSettings
		*/
		Toolbar
		(
			const std::string& p_title,
			bool p_opened,
			const OvUI::Settings::PanelWindowSettings& p_windowSettings
		);

		/**
		* Custom implementation of the draw method
		*/
		void _Draw_Impl() override;

	private:
		OvUI::Widgets::Buttons::ButtonImage* m_playButton;
		OvUI::Widgets::Buttons::ButtonImage* m_pauseButton;
		OvUI::Widgets::Buttons::ButtonImage* m_stopButton;
		OvUI::Widgets::Buttons::ButtonImage* m_nextButton;
	};
}