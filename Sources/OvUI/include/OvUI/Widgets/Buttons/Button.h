/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include <optional>
#include <string>

#include <OvMaths/FVector2.h>

#include <OvUI/Widgets/Buttons/AButton.h>
#include <OvUI/Types/ColorEffector.h>
#include <OvUI/Styling/Style.h>

namespace OvUI::Widgets::Buttons
{
	/**
	* Simple button widget
	*/
	class Button : public AButton
	{
	public:
		/**
		* Constructor
		* @param p_label
		* @param p_size
		* @param p_disabled
		*/
		Button(const std::string& p_label = "", const OvMaths::FVector2& p_size = OvMaths::FVector2(0.f, 0.f), bool p_disabled = false);

	protected:
		void _Draw_Impl() override;

	public:
		std::string label;
		OvMaths::FVector2 size;
		bool disabled = false;

		// Base background color. Hovered and clicked states are derived automatically
		// (×1.3 and ×0.75 on RGB) unless explicitly overridden via their optional fields.
		Types::ColorEffector backgroundColor = Types::ColorEffector::Ref(OVUI_STYLE(Button));
		std::optional<Types::ColorEffector> hoveredBackgroundColor;
		std::optional<Types::ColorEffector> clickedBackgroundColor;
		Types::ColorEffector textColor = Types::ColorEffector::Ref(OVUI_STYLE(Text));
	};
}
