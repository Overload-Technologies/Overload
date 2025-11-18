/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include "OvUI/Widgets/Buttons/ButtonColored.h"
#include "OvUI/Internal/Converter.h"

OvUI::Widgets::Buttons::ButtonColored::ButtonColored(const std::string & p_label, const Types::Color& p_color, const OvMaths::FVector2& p_size, bool p_enableAlpha) :
	label(p_label), color(p_color), size(p_size), enableAlpha(p_enableAlpha)
{
}

void OvUI::Widgets::Buttons::ButtonColored::_Draw_Impl()
{
	ImVec4 imColor = Internal::Converter::ToImVec4(color);

	if (ImGui::ColorButton((label + m_widgetID).c_str(), imColor, !enableAlpha ? ImGuiColorEditFlags_NoAlpha : 0, Internal::Converter::ToImVec2(size)))
		ClickedEvent.Invoke();

	color = Internal::Converter::ToColor(imColor);
}
