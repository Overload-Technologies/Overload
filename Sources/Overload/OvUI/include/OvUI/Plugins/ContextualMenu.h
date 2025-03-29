/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include "OvUI/Plugins/IPlugin.h"
#include "OvUI/Internal/WidgetContainer.h"
#include "OvUI/Widgets/Menu/MenuList.h"
#include "OvUI/Widgets/Menu/MenuItem.h"

namespace OvUI::Plugins
{
	/**
	* A simple plugin that will show a contextual menu on right click
	* You can add widgets to a contextual menu
	*/
	class ContextualMenu : public IPlugin, public Internal::WidgetContainer
	{
	public:
		/**
		* Execute the plugin
		* @param p_context
		*/
		void Execute(EPluginExecutionContext p_context);

		/**
		* Force close the contextual menu
		*/
		void Close();
	};
}
