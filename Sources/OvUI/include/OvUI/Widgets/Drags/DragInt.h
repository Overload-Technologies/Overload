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
	* Drag widget of type int
	*/
	class DragInt : public DragSingleScalar<int>
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
		DragInt
		(
			int p_min = 0,
			int p_max = 100,
			int p_value = 50,
			float p_speed = 1.0f,
			const std::string& p_label = "",
			const std::string& p_format = "%d"
		);
	};
}