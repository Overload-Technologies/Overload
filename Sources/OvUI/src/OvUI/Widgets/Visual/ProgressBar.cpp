/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include "OvUI/Widgets/Visual/ProgressBar.h"
#include "OvUI/Internal/Converter.h"

OvUI::Widgets::Visual::ProgressBar::ProgressBar(float p_fraction, const OvMaths::FVector2 & p_size, const std::string & p_overlay) :
	fraction(p_fraction), size(p_size), overlay(p_overlay)
{
}

void OvUI::Widgets::Visual::ProgressBar::_Draw_Impl()
{
	ImGui::ProgressBar(fraction, Internal::Converter::ToImVec2(size), !overlay.empty() ? overlay.c_str() : nullptr);
}
