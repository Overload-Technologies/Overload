/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include <OvUI/Panels/PanelUndecorated.h>
#include <OvUI/Widgets/Texts/TextColored.h>

#include <OvWindowing/Window.h>

namespace OvGame::Utils
{
	/**
	* Panel that display the framerate
	*/
	class FPSCounter : public OvUI::Panels::PanelUndecorated
	{
	public:
		/**
		* Constructor
		* @param p_window
		*/
		FPSCounter(OvWindowing::Window& p_window);

		/**
		* Update the data
		* @param p_deltaTime
		*/
		void Update(float p_deltaTime);

	private:
		OvUI::Widgets::Texts::TextColored m_text;

		OvWindowing::Window& m_window;
		float m_elapsed = 0.0f;
		uint32_t m_frames = 0;
	};
}