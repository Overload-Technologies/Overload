/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include <imgui.h>

#include <OvUI/Panels/PanelUndecorated.h>

void OvUI::Panels::PanelUndecorated::_Draw_Impl()
{
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 0, 0 });
	ImGui::PushStyleVar(ImGuiStyleVar_WindowMinSize, { 0, 0 });

	if (ImGui::Begin(m_panelID.c_str(), nullptr, CollectFlags()))
	{
		ImGui::PopStyleVar(2);

		Update();
			
		DrawWidgets();

		ImGui::End();
	}
	else
	{
		ImGui::PopStyleVar(2);
	}
}

int OvUI::Panels::PanelUndecorated::CollectFlags()
{
	ImGuiWindowFlags flags =
		ImGuiWindowFlags_AlwaysUseWindowPadding |
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoScrollbar |
		ImGuiWindowFlags_NoScrollWithMouse |
		ImGuiWindowFlags_NoCollapse |
		ImGuiWindowFlags_NoTitleBar |
		ImGuiWindowFlags_NoBackground;

	if (autoSize) flags |= ImGuiWindowFlags_AlwaysAutoResize;

	return flags;
}
