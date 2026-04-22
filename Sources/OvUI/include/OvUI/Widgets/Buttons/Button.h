/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

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

		Types::ColorEffector backgroundColor        = OVUI_STYLE_REF(Button);
		Types::ColorEffector hoveredBackgroundColor  = OVUI_STYLE_REF(ButtonHovered);
		Types::ColorEffector clickedBackgroundColor  = OVUI_STYLE_REF(ButtonActive);
		Types::ColorEffector textColor               = OVUI_STYLE_REF(Text);
	};
}
