/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include "OvUI/Widgets/Sliders/SliderSingleScalar.h"

namespace OvUI::Widgets::Sliders
{
	/**
	* Slider widget of type float
	*/
	class SliderFloat : public SliderSingleScalar<float>
	{
	public:
		/**
		* Constructor
		* @param p_min
		* @param p_max
		* @param p_value
		* @param p_orientation
		* @param p_label
		* @param p_format
		*/
		SliderFloat
		(
			float p_min = 0.0f,
			float p_max = 1.0f,
			float p_value = 0.5f,
			ESliderOrientation p_orientation = ESliderOrientation::HORIZONTAL,
			const std::string& p_label = "",
			const std::string& p_format = "%.3f"
		);
	};
}