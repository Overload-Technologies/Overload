/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include <imgui.h>

#include <OvUI/Widgets/Texts/TextClickable.h>

OvUI::Widgets::Texts::TextClickable::TextClickable(const std::string & p_content) :
	Text(p_content)
{
}

void OvUI::Widgets::Texts::TextClickable::_Draw_Impl()
{
	bool useless = false;

	if (ImGui::Selectable((content + m_widgetID).c_str(), &useless, ImGuiSelectableFlags_AllowDoubleClick))
	{
		if (ImGui::IsMouseDoubleClicked(0))
		{
			DoubleClickedEvent.Invoke();
		}
		else
		{
			ClickedEvent.Invoke();
		}
	}
}
