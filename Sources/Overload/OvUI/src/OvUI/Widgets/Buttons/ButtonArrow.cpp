/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include <imgui.h>

#include <OvTools/Utils/EnumMapper.h>
#include <OvUI/Widgets/Buttons/ButtonArrow.h>

template <>
struct OvTools::Utils::MappingFor<OvUI::Settings::EDirection, ImGuiDir>
{
	using enum ImGuiDir;
	using EnumType = OvUI::Settings::EDirection;
	using type = std::tuple<
		EnumValuePair<EnumType::NONE, ImGuiDir_None>,
		EnumValuePair<EnumType::LEFT, ImGuiDir_Left>,
		EnumValuePair<EnumType::RIGHT, ImGuiDir_Right>,
		EnumValuePair<EnumType::UP, ImGuiDir_Up>,
		EnumValuePair<EnumType::DOWN, ImGuiDir_Down>
	>;
};

OvUI::Widgets::Buttons::ButtonArrow::ButtonArrow(Settings::EDirection p_direction) :
	direction(p_direction)
{
}

void OvUI::Widgets::Buttons::ButtonArrow::_Draw_Impl()
{
	if (ImGui::ArrowButton(m_widgetID.c_str(), OvTools::Utils::ToValueImpl<Settings::EDirection, ImGuiDir>(direction)))
	{
		ClickedEvent.Invoke();
	}
}
