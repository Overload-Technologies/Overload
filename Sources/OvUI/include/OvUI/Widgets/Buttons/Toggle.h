/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include <string>

#include <OvTools/Eventing/Event.h>

#include <OvUI/Types/ColorEffector.h>
#include <OvUI/Styling/Style.h>
#include <OvUI/Widgets/AWidget.h>

namespace OvUI::Widgets::Buttons
{
	/**
	* A two-state segmented toggle.
	* Clicking anywhere toggles between state A (false) and state B (true).
	*/
	class Toggle : public AWidget
	{
	public:
		/**
		* Constructor
		* @param p_labelA  Label for the first state (false)
		* @param p_labelB  Label for the second state (true)
		* @param p_state   Initial state (false = A active, true = B active)
		*/
		Toggle(const std::string& p_labelA = "A", const std::string& p_labelB = "B", bool p_state = false);

	protected:
		void _Draw_Impl() override;

	public:
		std::string labelA;
		std::string labelB;
		bool state = false;

		Types::ColorEffector activeColor = OVUI_STYLE(WarningButton);
		Types::ColorEffector activeHoveredColor = OVUI_STYLE(WarningButtonHovered);
		Types::ColorEffector activePressedColor = OVUI_STYLE(WarningButtonActive);
		Types::ColorEffector inactiveColor = OVUI_STYLE(Button);
		Types::ColorEffector inactiveHoveredColor = OVUI_STYLE(ButtonHovered);
		Types::ColorEffector inactivePressedColor = OVUI_STYLE(ButtonActive);

		OvTools::Eventing::Event<bool> StateChangedEvent; // true = B active
	};
}
