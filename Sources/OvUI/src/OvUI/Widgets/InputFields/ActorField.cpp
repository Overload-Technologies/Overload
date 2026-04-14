/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include <imgui.h>

#include <OvUI/Widgets/InputFields/ActorField.h>

OvUI::Widgets::InputFields::ActorField::ActorField(uint64_t p_guid, const std::string& p_displayName)
	: DataWidget<uint64_t>(guid), guid(p_guid), displayName(p_displayName)
{
}

void OvUI::Widgets::InputFields::ActorField::_Draw_Impl()
{
	const float buttonSize = ImGui::GetFrameHeight();
	const float innerSize = buttonSize - 2.0f * ImGui::GetStyle().FramePadding.x;

	const std::string label = displayName.empty() ? "None" : displayName;

	ImGui::BeginGroup();

	ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - buttonSize);

	char buf[512] = {};
	label.copy(buf, sizeof(buf) - 1);
	ImGui::BeginDisabled();
	ImGui::InputText((m_widgetID + "i").c_str(), buf, sizeof(buf), ImGuiInputTextFlags_ReadOnly);
	ImGui::EndDisabled();

	// GUID tooltip when hovering the text field
	if (guid != 0 && ImGui::IsItemHovered(ImGuiHoveredFlags_ForTooltip))
		ImGui::SetTooltip("GUID: %016llX", (unsigned long long)guid);

	// Double-click → navigate to actor
	if (guid != 0 && ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled) && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
		DoubleClickedEvent.Invoke();

	ImGui::SameLine(0, 0);

	bool clicked;
	if (iconTextureID != 0)
	{
		clicked = ImGui::ImageButton(
			("icon" + m_widgetID).c_str(),
			(ImTextureID)(uintptr_t)iconTextureID,
			ImVec2(innerSize, innerSize),
			ImVec2(0.f, 1.f),
			ImVec2(1.f, 0.f)
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
