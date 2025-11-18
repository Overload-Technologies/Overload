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
	* Drag widget of type double
	*/
	class DragDouble : public DragSingleScalar<double>
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
		DragDouble
		(
			double p_min = 0.0,
			double p_max = 1.0,
			double p_value = 0.5,
			float p_speed = 0.1f,
			const std::string& p_label = "",
			const std::string& p_format = "%.6f"
		);
	};
}