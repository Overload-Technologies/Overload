/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include "OvUI/Internal/Converter.h"

ImVec4 OvUI::Internal::Converter::ToImVec4(const Types::Color & p_value)
{
	return ImVec4(p_value.r, p_value.g, p_value.b, p_value.a);
}

OvUI::Types::Color OvUI::Internal::Converter::ToColor(const ImVec4 & p_value)
{
	return Types::Color(p_value.x, p_value.y, p_value.z, p_value.w);
}

ImVec2 OvUI::Internal::Converter::ToImVec2(const OvMaths::FVector2 & p_value)
{
	return ImVec2(p_value.x, p_value.y);
}

OvMaths::FVector2 OvUI::Internal::Converter::ToFVector2(const ImVec2 & p_value)
{
	return OvMaths::FVector2(p_value.x, p_value.y);
}
