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
	* Drag widget of multiple type int
	*/
	template <size_t _Size>
	class DragMultipleInts : public DragMultipleScalars<int, _Size>
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
		DragMultipleInts
		(
			int p_min = 0,
			int p_max = 100,
			int p_value = 50,
			float p_speed = 1.0f,
			const std::string& p_label = "",
			const std::string& p_format = "%d"
		) : DragMultipleScalars<int, _Size>(ImGuiDataType_::ImGuiDataType_S32, p_min, p_max, p_value, p_speed, p_label, p_format) {}
	};
}