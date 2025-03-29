/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

namespace OvUI::Plugins
{
	/**
	* Context of execution for a plugin
	*/
	enum class EPluginExecutionContext
	{
		WIDGET,
		PANEL
	};

	/**
	* Interface to any plugin of OvUI.
	* A plugin is basically a behaviour that you can plug to a widget
	*/
	class IPlugin
	{
	public:
		/**
		* Execute the plugin behaviour
		* @param p_context
		*/
		virtual void Execute(EPluginExecutionContext p_context) = 0;

		/* Feel free to store any data you want here */
		void* userData = nullptr;
	};
}