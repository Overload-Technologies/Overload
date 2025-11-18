/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include "OvUI/Widgets/Layout/Dummy.h"
#include "OvUI/Internal/Converter.h"

OvUI::Widgets::Layout::Dummy::Dummy(const OvMaths::FVector2& p_size) : size(p_size)
{
}

void OvUI::Widgets::Layout::Dummy::_Draw_Impl()
{
	ImGui::Dummy(Internal::Converter::ToImVec2(size));
}
