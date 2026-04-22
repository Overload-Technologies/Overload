/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include <string>

#include <OvTools/Eventing/Event.h>

#include <OvUI/Types/ColorEffector.h>
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

		Types::ColorEffector activeColor;
		Types::ColorEffector activeHoveredColor;
		Types::ColorEffector activePressedColor;
		Types::ColorEffector inactiveColor;
		Types::ColorEffector inactiveHoveredColor;
		Types::ColorEffector inactivePressedColor;

		OvTools::Eventing::Event<bool> StateChangedEvent; // true = B active
	};
}
