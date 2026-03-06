/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include "OvUI/Widgets/Drags/DragMultipleScalars.h"

namespace OvUI::Widgets::Drags
{
	/**
	* Drag widget of multiple type float
	*/
	template <size_t _Size>
	class DragMultipleFloats : public DragMultipleScalars<float, _Size>
	{
	public:
		/**
		* Constructor
		* @param p_min
		* @param p_max
		* @param p_value
		* @param p_speed
		* @param p_label
		* @param p_format
		*/
		DragMultipleFloats
		(
			float p_min = 0.0f,
			float p_max = 1.0f,
			float p_value = 0.5f,
			float p_speed = 0.1f,
			const std::string& p_label = "",
			const std::string& p_format = "%.3f"
		) : DragMultipleScalars<float, _Size>(ImGuiDataType_::ImGuiDataType_Float, p_min, p_max, p_value, p_speed, p_label, p_format) {}
	};
}