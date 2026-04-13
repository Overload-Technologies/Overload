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

	if (focusOnNextDraw)
	{
		ImGui::SetKeyboardFocusHere();
		focusOnNextDraw = false;
	}

	const bool hasIcon = iconTextureID != 0;

	if (hasIcon)
	{
		const ImVec2 padding = ImGui::GetStyle().FramePadding;
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(padding.x + iconSize + padding.x, padding.y));
	}

	if (fullWidth)
		ImGui::SetNextItemWidth(-FLT_MIN);

	content.resize(256, '\0');
	bool enterPressed = ImGui::InputText((label + m_widgetID).c_str(), &content[0], 256, ImGuiInputTextFlags_EnterReturnsTrue | (selectAllOnClick ? ImGuiInputTextFlags_AutoSelectAll : 0));
	content = content.c_str();

	if (hasIcon)
	{
		ImGui::PopStyleVar();

		ImDrawList* drawList = ImGui::GetWindowDrawList();
		const ImVec2 rectMin = ImGui::GetItemRectMin();
		const ImVec2 rectMax = ImGui::GetItemRectMax();
		const float frameHeight = rectMax.y - rectMin.y;
		const float padding = ImGui::GetStyle().FramePadding.y;
		const float fitSize = std::min(iconSize, frameHeight - padding * 2.0f);
		const float centerY = (rectMin.y + rectMax.y) * 0.5f;
		const float iconX = rectMin.x + ImGui::GetStyle().FramePadding.x;
		drawList->AddImage(iconTextureID,
			ImVec2(iconX, centerY - fitSize * 0.5f),
			ImVec2(iconX + fitSize, centerY + fitSize * 0.5f),
			ImVec2(0.f, 1.f), ImVec2(1.f, 0.f));
	}

	if (content != previousContent)
	{
		ContentChangedEvent.Invoke(content);
		this->NotifyChange();
	}

	if (enterPressed)
		EnterPressedEvent.Invoke(content);
}