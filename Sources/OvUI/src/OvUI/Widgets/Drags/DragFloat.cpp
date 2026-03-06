/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include "OvUI/Widgets/Drags/DragFloat.h"

OvUI::Widgets::Drags::DragFloat::DragFloat
(
	float p_min,
	float p_max,
	float p_value,
	float p_speed,
	const std::string& p_label,
	const std::string & p_format
) : DragSingleScalar<float>(ImGuiDataType_::ImGuiDataType_Float, p_min, p_max, p_value, p_speed, p_label, p_format)
{
}