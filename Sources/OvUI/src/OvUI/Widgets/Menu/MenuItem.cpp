/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include <imgui.h>

#include <OvUI/Widgets/Menu/MenuItem.h>

OvUI::Widgets::Menu::MenuItem::MenuItem(const std::string & p_name, const std::string & p_shortcut, bool p_checkable, bool p_checked) :
	DataWidget(m_selected), name(p_name), shortcut(p_shortcut), checkable(p_checkable), checked(p_checked)
{
}

void OvUI::Widgets::Menu::MenuItem::_Draw_Impl()
{
	bool previousValue = checked;

	if (ImGui::MenuItem((name + m_widgetID).c_str(), shortcut.c_str(), checkable ? &checked : nullptr, enabled))
		ClickedEvent.Invoke();

	if (checked != previousValue)
	{
		ValueChangedEvent.Invoke(checked);
		this->NotifyChange();
	}
}
