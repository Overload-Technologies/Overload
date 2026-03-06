/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include "OvUI/Widgets/Drags/DragSingleScalar.h"

namespace OvUI::Widgets::Drags
{
	/**
	* Drag widget of type float
	*/
	class DragFloat : public DragSingleScalar<float>
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
		DragFloat
		(
			float p_min = 0.0f,
			float p_max = 1.0f,
			float p_value = 0.5f,
			float p_speed = 0.1f,
			const std::string& p_label = "",
			const std::string& p_format = "%.3f"
		);
	};
}