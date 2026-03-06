/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include "OvGame/Core/Context.h"
#include "OvGame/Core/Game.h"

namespace OvGame::Core
{
	/**
	* Entry point of OvGame
	*/
	class Application
	{
	public:
		/**
		* Constructor
		*/
		Application();

		/**
		* Destructor
		*/
		~Application();

		/**
		* Run the app
		*/
		void Run();

		/**
		* Returns true if the app is running
		*/
		bool IsRunning() const;

	private:
		Context m_context;
		Game m_game;
	};
}