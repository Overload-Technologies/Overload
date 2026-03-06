/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include <OvTools/Eventing/Event.h>

#include "OvUI/Widgets/DataWidget.h"

namespace OvUI::Widgets::Selection
{
	/**
	* Checkbox widget that can be checked or not
	*/
	class CheckBox : public DataWidget<bool>
	{
	public:
		/** 
		* Constructor
		* @param p_value
		* @param p_label
		*/
		CheckBox(bool p_value = false, const std::string& p_label = "");

	protected:
		void _Draw_Impl() override;

	public:
		bool value;
		std::string label;
		OvTools::Eventing::Event<bool> ValueChangedEvent;
	};
}