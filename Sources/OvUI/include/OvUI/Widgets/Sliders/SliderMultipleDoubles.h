/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include "OvUI/Widgets/Sliders/SliderMultipleScalars.h"

namespace OvUI::Widgets::Sliders
{
	/**
	* Slider widget of multiple type double
	*/
	template <size_t _Size>
	class SliderMultipleDoubles : public SliderMultipleScalars<double, _Size>
	{
	public:
		/**
		* Constructor
		* @param p_min
		* @param p_max
		* @param p_value
		* @param p_label
		* @param p_format
		*/
		SliderMultipleDoubles
		(
			double p_min = 0.0,
			double p_max = 1.0,
			double p_value = 0.5,
			const std::string& p_label = "",
			const std::string& p_format = "%.6f"
		) : SliderMultipleScalars<double, _Size>(ImGuiDataType_::ImGuiDataType_Double, p_min, p_max, p_value, p_label, p_format) {}
	};
}