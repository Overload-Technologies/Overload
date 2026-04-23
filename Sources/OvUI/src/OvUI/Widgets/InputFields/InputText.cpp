/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include <imgui.h>

#include <OvUI/Widgets/InputFields/InputText.h>

OvUI::Widgets::InputFields::InputText::InputText(const std::string& p_content, const std::string& p_label) :
	DataWidget<std::string>(content), content(p_content), label(p_label)
{
}

void OvUI::Widgets::InputFields::InputText::_Draw_Impl()
{
	std::string previousContent = content;

	bool needFocus = focusOnNextDraw;
	focusOnNextDraw = false;

	const bool hasIcon = iconTextureID != 0;
	const float buttonSize = ImGui::GetFrameHeight();
	const float innerSize = buttonSize - 2.0f * ImGui::GetStyle().FramePadding.x;

	if (hasIcon)
	{
		const ImVec4 frameBg = ImGui::GetStyleColorVec4(ImGuiCol_FrameBg);
		ImGui::PushStyleColor(ImGuiCol_Button, frameBg);
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, frameBg);
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, frameBg);
		const bool iconClicked = ImGui::ImageButton(
			("icon" + m_widgetID).c_str(),
			(ImTextureID)(uintptr_t)iconTextureID,
			ImVec2(innerSize, innerSize),
			ImVec2(0.f, 1.f), ImVec2(1.f, 0.f)
		);
		ImGui::PopStyleColor(3);

		if (iconClicked)
			needFocus = true;

		ImGui::SameLine(0, 0);
	}

	if (fullWidth)
		ImGui::SetNextItemWidth(-FLT_MIN);

	if (needFocus)
		ImGui::SetKeyboardFocusHere(0);

	content.resize(256, '\0');
	bool enterPressed = ImGui::InputText((label + m_widgetID).c_str(), &content[0], 256, ImGuiInputTextFlags_EnterReturnsTrue | (selectAllOnClick ? ImGuiInputTextFlags_AutoSelectAll : 0));
	content = content.c_str();

	if (content != previousContent)
	{
		ContentChangedEvent.Invoke(content);
		this->NotifyChange();
	}

	if (enterPressed)
		EnterPressedEvent.Invoke(content);
}