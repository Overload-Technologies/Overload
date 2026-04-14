/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include <filesystem>
#include <string>

#include <imgui.h>

#include <OvTools/Utils/PathParser.h>
#include <OvUI/Widgets/InputFields/AssetField.h>

OvUI::Widgets::InputFields::AssetField::AssetField(const std::string& p_content)
	: DataWidget<std::string>(content), content(p_content)
{
}

void OvUI::Widgets::InputFields::AssetField::_Draw_Impl()
{
	const float buttonSize = ImGui::GetFrameHeight();
	const float innerSize = buttonSize - 2.0f * ImGui::GetStyle().FramePadding.x;

	const std::string friendly = OvTools::Utils::PathParser::GetFriendlyPath(content);
	const std::string displayName = friendly.empty() ? "None" : std::filesystem::path(friendly).stem().string();
	const std::string tooltip = friendly;

	ImGui::BeginGroup();

	ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - buttonSize);

	char buf[512] = {};
	displayName.copy(buf, sizeof(buf) - 1);
	ImGui::BeginDisabled();
	ImGui::InputText((m_widgetID + "i").c_str(), buf, sizeof(buf), ImGuiInputTextFlags_ReadOnly);
	ImGui::EndDisabled();

	if (!tooltip.empty() && ImGui::IsItemHovered(ImGuiHoveredFlags_ForTooltip))
		ImGui::SetTooltip("%s", tooltip.c_str());

	if (!content.empty() && ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled) && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
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

	if (previewTextureID != 0)
	{
		// Tighten the gap between the field row and preview to match frame inner padding.
		const float gap = ImGui::GetStyle().FramePadding.y;
		ImGui::SetCursorPosY(ImGui::GetCursorPosY() - ImGui::GetStyle().ItemSpacing.y + gap);

		ImVec2 imgMin = ImGui::GetCursorScreenPos();
		ImGui::Image(
			(ImTextureID)(uintptr_t)previewTextureID,
			ImVec2(previewSize, previewSize),
			ImVec2(0.f, 1.f), ImVec2(1.f, 0.f)
		);
		ImGui::GetWindowDrawList()->AddRect(
			imgMin,
			ImVec2(imgMin.x + previewSize, imgMin.y + previewSize),
			ImGui::GetColorU32(ImGuiCol_Border)
		);
		if (ImGui::IsItemClicked())
			ClickedEvent.Invoke();
	}

	ImGui::EndGroup();
}
