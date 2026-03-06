/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include "OvUI/Widgets/InputFields/InputSingleScalar.h"

namespace OvUI::Widgets::InputFields
{
	/**
	* Input widget of type int
	*/
	class InputInt : public InputSingleScalar<int>
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
		InputInt
		(
			int p_defaultValue = 0,
			int p_step = 1,
			int p_fastStep = 0,
			const std::string& p_label = "",
			const std::string& p_format = "%d",
			bool p_selectAllOnClick = true
		);
	};
}