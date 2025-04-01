/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include <OvUI/ImGui/imgui.h>
#include <OvUI/Styling/TStyle.h>

template<>
ImGuiStyle OvUI::Styling::TStyle<OvUI::Styling::EStyle::IM_CLASSIC_STYLE>::GetStyle()
{
	ImGuiStyle style;
	ImGui::StyleColorsClassic(&style);
	return style;
}
