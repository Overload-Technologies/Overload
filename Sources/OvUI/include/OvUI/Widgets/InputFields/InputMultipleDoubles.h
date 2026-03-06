/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include "OvUI/Widgets/InputFields/InputMultipleScalars.h"

namespace OvUI::Widgets::InputFields
{
	/**
	* Input widget of multiple type double
	*/
	template <size_t _Size>
	class InputMultipleDoubles : public InputMultipleScalars<double, _Size>
	{
	public:
		/**
		* Constructor
		* @param p_defaultValue
		* @param p_step
		* @param p_fastStep
		* @param p_label
		* @param p_format
		* @param p_selectAllOnClick
		*/
		InputMultipleDoubles
		(
			double p_defaultValue = 0.0,
			double p_step = 0.1,
			double p_fastStep = 0.0,
			const std::string& p_label = "",
			const std::string& p_format = "%.6f",
			bool p_selectAllOnClick = true
		) : InputMultipleScalars<double, _Size>(ImGuiDataType_::ImGuiDataType_Double, p_defaultValue, p_step, p_fastStep, p_label, p_format, p_selectAllOnClick) {}
	};
}