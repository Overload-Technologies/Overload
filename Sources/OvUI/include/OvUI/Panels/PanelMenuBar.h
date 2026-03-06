/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include <vector>
#include <memory>
#include <algorithm>

#include <OvTools/Eventing/Event.h>

#include "OvUI/Panels/APanel.h"
#include "OvUI/Widgets/Menu/MenuList.h"

namespace OvUI::Panels
{
	/**
	* A simple panel that will be displayed on the top side of the canvas
	*/
	class PanelMenuBar : public APanel
	{
	protected:
		void _Draw_Impl() override;
	};
}