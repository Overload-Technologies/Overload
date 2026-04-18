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
#include <OvUI/Types/Color.h>

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

		std::optional<Types::Color> idleBackgroundColor;
		std::optional<Types::Color> hoveredBackgroundColor;
		std::optional<Types::Color> clickedBackgroundColor;
		std::optional<Types::Color> textColor;
	};
}
