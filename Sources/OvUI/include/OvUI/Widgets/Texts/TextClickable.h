/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include <OvTools/Eventing/Event.h>

#include "OvUI/Widgets/Texts/Text.h"

namespace OvUI::Widgets::Texts
{
	/**
	* Widget to display text on a panel that is also clickable
	*/
	class TextClickable : public Text
	{
	public:
		/**
		* Constructor
		* @param p_content
		*/
		TextClickable(const std::string& p_content = "");

	protected:
		virtual void _Draw_Impl() override;

	public:
		OvTools::Eventing::Event<> ClickedEvent;
		OvTools::Eventing::Event<> DoubleClickedEvent;
	};
}