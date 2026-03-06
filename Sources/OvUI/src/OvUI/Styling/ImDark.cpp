/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include <imgui.h>

#include <OvUI/Styling/TStyle.h>

template<>
ImGuiStyle OvUI::Styling::TStyle<OvUI::Styling::EStyle::IM_DARK_STYLE>::GetStyle()
{
	ImGuiStyle style;
	ImGui::StyleColorsDark(&style);
	return style;
}
