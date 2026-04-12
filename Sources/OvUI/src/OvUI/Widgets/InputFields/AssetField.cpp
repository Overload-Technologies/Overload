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
	const float innerSize = buttonSize - 2.0f * ImGui::GetStyle().FramePadding.x;

	ImGui::BeginGroup();

	ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - buttonSize);

	char buf[512] = {};
	content.copy(buf, sizeof(buf) - 1);
	ImGui::BeginDisabled();
	ImGui::InputText((m_widgetID + "i").c_str(), buf, sizeof(buf), ImGuiInputTextFlags_ReadOnly);
	ImGui::EndDisabled();

	ImGui::SameLine(0, 0);

	bool clicked;
	if (iconTextureID != 0)
	{
		clicked = ImGui::ImageButton(
			("icon" + m_widgetID).c_str(),
			(ImTextureID)(uintptr_t)iconTextureID,
			ImVec2(innerSize, innerSize)
		);
	}
	else
	{
		clicked = ImGui::Button(("..." + m_widgetID).c_str(), ImVec2(buttonSize, buttonSize));
	}

	if (clicked)
		ClickedEvent.Invoke();

	ImGui::EndGroup();
}
