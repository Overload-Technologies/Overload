/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include <imgui.h>

#include <OvUI/Widgets/Selection/CheckBox.h>

OvUI::Widgets::Selection::CheckBox::CheckBox(bool p_value, const std::string & p_label) :
	DataWidget<bool>(value), value(p_value), label(p_label)
{
}

void OvUI::Widgets::Selection::CheckBox::_Draw_Impl()
{
	bool previousValue = value;

	ImGui::Checkbox((label + m_widgetID).c_str(), &value);

	if (value != previousValue)
	{
		ValueChangedEvent.Invoke(value);
		this->NotifyChange();
	}
}
