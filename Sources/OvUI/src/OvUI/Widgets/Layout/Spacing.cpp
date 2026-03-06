/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include <imgui.h>

#include <OvUI/Widgets/Layout/Spacing.h>

OvUI::Widgets::Layout::Spacing::Spacing(uint16_t p_spaces) : spaces(p_spaces)
{
}

void OvUI::Widgets::Layout::Spacing::_Draw_Impl()
{
	for (uint16_t i = 0; i < spaces; ++i)
	{
		ImGui::Spacing();

		if (i + 1 < spaces)
			ImGui::SameLine();
	}
}
