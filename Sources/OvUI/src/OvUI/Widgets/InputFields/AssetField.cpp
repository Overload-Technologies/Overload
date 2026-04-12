/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include <imgui.h>

#include <OvUI/Widgets/InputFields/AssetField.h>

OvUI::Widgets::InputFields::AssetField::AssetField(const std::string& p_content)
	: DataWidget<std::string>(content), content(p_content)
{
}

void OvUI::Widgets::InputFields::AssetField::_Draw_Impl()
{
	const float buttonSize = ImGui::GetFrameHeight();

	ImGui::BeginGroup();

	ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - buttonSize);

	char buf[512] = {};
	strncpy(buf, content.c_str(), sizeof(buf) - 1);
	ImGui::BeginDisabled();
	ImGui::InputText((m_widgetID + "i").c_str(), buf, sizeof(buf), ImGuiInputTextFlags_ReadOnly);
	ImGui::EndDisabled();

	ImGui::SameLine(0, 0);

	if (ImGui::Button(("..." + m_widgetID).c_str(), ImVec2(buttonSize, buttonSize)))
		ClickedEvent.Invoke();

	ImGui::EndGroup();
}
