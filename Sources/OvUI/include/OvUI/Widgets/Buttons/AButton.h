/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include <string>

#include <OvTools/Eventing/Event.h>

#include "OvUI/Widgets/AWidget.h"

namespace OvUI::Widgets::Buttons
{
	/**
	* Base class for any button widget
	*/
	class AButton : public AWidget
	{
	protected:
		void _Draw_Impl() override = 0;

	public:
		OvTools::Eventing::Event<> ClickedEvent;
	};
}