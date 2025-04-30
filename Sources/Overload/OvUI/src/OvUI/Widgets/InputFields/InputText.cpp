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