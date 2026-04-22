#pragma once

#include "OvUI/Widgets/Texts/Text.h"
#include "OvUI/Types/ColorEffector.h"
#include "OvUI/Styling/Style.h"

namespace OvUI::Widgets::Texts
{
	/**
	* Widget to display text on a panel that can be colored
	*/
	class TextColored : public Text
	{
	public:
		/**
		* Constructor
		* @param p_content
		* @param p_color   When omitted, defaults to the style's Text color.
		*/
		TextColored(const std::string& p_content = "", const Types::ColorEffector& p_color = OVUI_STYLE(Text));

	public:
		Types::ColorEffector color = OVUI_STYLE(Text);

	protected:
		virtual void _Draw_Impl() override;
	};
}
