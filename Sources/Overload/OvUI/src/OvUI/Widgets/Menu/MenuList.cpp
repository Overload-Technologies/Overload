/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include <imgui.h>

#include <OvUI/Widgets/Menu/MenuList.h>

OvUI::Widgets::Menu::MenuList::MenuList(const std::string & p_name, bool p_locked) :
	name(p_name), locked(p_locked)
{
}

void OvUI::Widgets::Menu::MenuList::_Draw_Impl()
{
	if (ImGui::BeginMenu(name.c_str(), !locked))
	{
		if (!m_opened)
		{
			ClickedEvent.Invoke();
			m_opened = true;
		}

		DrawWidgets();
		ImGui::EndMenu();
	}
	else
	{
		m_opened = false;
	}
}
