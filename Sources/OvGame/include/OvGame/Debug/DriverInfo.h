/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#ifdef _DEBUG

#include <OvRendering/Context/Driver.h>
#include <OvWindowing/Window.h>

#include <OvUI/Panels/PanelUndecorated.h>
#include <OvUI/Widgets/Texts/TextColored.h>

namespace OvGame::Debug
{
	/**
	* Panel that display information about the driver (hardware and software)
	*/
	class DriverInfo : public OvUI::Panels::PanelUndecorated
	{
	public:
		/**
		* Constructor
		* @param p_driver
		* @param p_window
		*/
		DriverInfo(OvRendering::Context::Driver& p_rdriver, OvWindowing::Window& p_window);
	};
}

#endif