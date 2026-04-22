/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include "OvUI/Widgets/Texts/TextColored.h"
#include "OvUI/Internal/Converter.h"

OvUI::Widgets::Texts::TextColored::TextColored(const std::string& p_content, const Types::ColorEffector& p_color) :
	Text(p_content)
{
	if (p_color.HasSource())
		color = p_color;
}

void OvUI::Widgets::Texts::TextColored::_Draw_Impl()
{
	ImGui::TextColored(Internal::Converter::ToImVec4(color.Resolve()), "%s", content.c_str());
}
