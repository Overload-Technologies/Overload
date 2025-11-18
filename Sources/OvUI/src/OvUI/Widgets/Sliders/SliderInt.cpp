/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include "OvUI/Widgets/Sliders/SliderInt.h"

OvUI::Widgets::Sliders::SliderInt::SliderInt
(
	int p_min,
	int p_max,
	int p_value,
	ESliderOrientation p_orientation,
	const std::string& p_label,
	const std::string & p_format
) : SliderSingleScalar<int>(ImGuiDataType_::ImGuiDataType_S32, p_min, p_max, p_value, p_orientation, p_label, p_format)
{
}