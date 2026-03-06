/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include <imgui.h>

#include <OvUI/Widgets/Texts/TextLabelled.h>

OvUI::Widgets::Texts::TextLabelled::TextLabelled(const std::string& p_content, const std::string& p_label) :
	Text(p_content), label(p_label)
{
}

void OvUI::Widgets::Texts::TextLabelled::_Draw_Impl()
{
	ImGui::LabelText((label + m_widgetID).c_str(), "%s", content.c_str());
}
