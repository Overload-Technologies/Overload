/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include <OvUI/Styling/EStyle.h>

struct ImGuiStyle;

namespace OvUI::Styling
{
	/**
	* Base class (no actual inheritence, but rather a template specialization) for any style
	*/
	template<EStyle Style>
	class TStyle
	{
	public:
		/**
		* Returns a pointer to the style struct.
		*/
		static ImGuiStyle GetStyle();
	};
}
