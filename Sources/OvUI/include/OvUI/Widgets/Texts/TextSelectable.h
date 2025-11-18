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
	* Simple widget to display a selectable text on a panel
	*/
	class TextSelectable : public Text
	{
	public:
		/**
		* Constructor
		* @param p_content
		* @param p_selected
		* @param p_disabled
		*/
		TextSelectable(const std::string& p_content = "", bool p_selected = false, bool p_disabled = false);

	protected:
		virtual void _Draw_Impl() override;

	public:
		bool selected;
		bool disabled;

		OvTools::Eventing::Event<bool> ClickedEvent;
		OvTools::Eventing::Event<> SelectedEvent;
		OvTools::Eventing::Event<> UnselectedEvent;
	};
}