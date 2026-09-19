/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include <cfloat>
#include <cstddef>
#include <imgui.h>

#include <OvUI/Widgets/InputFields/InputText.h>

namespace
{
	int ResizeInputTextBuffer(ImGuiInputTextCallbackData* p_data)
	{
		if (p_data->EventFlag != ImGuiInputTextFlags_CallbackResize || !p_data->UserData)
		{
			return 0;
		}

		auto& content = *static_cast<std::string*>(p_data->UserData);
		content.resize(static_cast<std::size_t>(p_data->BufTextLen));
		p_data->Buf = content.data();
		return 0;
	}
}

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

	const auto commonFlags =
		(selectAllOnClick ? ImGuiInputTextFlags_AutoSelectAll : ImGuiInputTextFlags_None) |
		ImGuiInputTextFlags_CallbackResize;
	bool enterPressed = false;

	if (multiline)
	{
		const float fieldHeight = multilineHeight > 0.0f ?
			multilineHeight :
			ImGui::GetTextLineHeightWithSpacing() * 4.0f;
		const float fieldWidth = fullWidth ? -FLT_MIN : 0.0f;

		enterPressed = ImGui::InputTextMultiline(
			(label + m_widgetID).c_str(),
			content.data(),
			content.capacity() + 1,
			ImVec2(fieldWidth, fieldHeight),
			commonFlags,
			ResizeInputTextBuffer,
			&content
		);
	}
	else
	{
		enterPressed = ImGui::InputText(
			(label + m_widgetID).c_str(),
			content.data(),
			content.capacity() + 1,
			ImGuiInputTextFlags_EnterReturnsTrue | commonFlags,
			ResizeInputTextBuffer,
			&content
		);
	}

	if (content != previousContent)
	{
		ContentChangedEvent.Invoke(content);
		this->NotifyChange();
	}

	if (enterPressed && !multiline)
		EnterPressedEvent.Invoke(content);
}
