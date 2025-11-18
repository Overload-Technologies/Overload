/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include "OvUI/Widgets/Texts/Text.h"

namespace OvUI::Widgets::Texts
{
	/**
	* Widget to display a disabled text on a panel
	*/
	class TextDisabled : public Text
	{
	public:
		/**
		* Constructor
		* @param p_content
		*/
		TextDisabled(const std::string& p_content = "");

	protected:
		virtual void _Draw_Impl() override;
	};
}