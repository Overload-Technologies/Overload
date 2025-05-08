/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include <string>

#include <OvUI/Settings/EDirection.h>
#include <OvUI/Widgets/Buttons/AButton.h>

namespace OvUI::Widgets::Buttons
{
	/**
	* Button widget with an arrow image
	*/
	class ButtonArrow : public AButton
	{
	public:
		/**
		* Create the button
		* @param p_direction
		*/
		ButtonArrow(Settings::EDirection p_direction = Settings::EDirection::NONE);

	protected:
		void _Draw_Impl() override;

	public:
		Settings::EDirection direction;
	};
}