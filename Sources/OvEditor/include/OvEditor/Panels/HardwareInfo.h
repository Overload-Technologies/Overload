/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include <OvUI/Panels/PanelWindow.h>

namespace OvEditor::Panels
{
	class HardwareInfo : public OvUI::Panels::PanelWindow
	{
	public:
		/**
		* Constructor
		* @param p_title
		* @param p_opened
		* @param p_windowSettings
		* @param p_logFrequency
		* @param p_maxElements
		* @param p_displayMode
		*/
		HardwareInfo
		(
			const std::string& p_title,
			bool p_opened,
			const OvUI::Settings::PanelWindowSettings& p_windowSettings
		);
	};
}