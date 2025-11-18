/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include <OvEditor/Core/Context.h>
#include <OvEditor/Core/Editor.h>

namespace OvEditor::Core
{
	/**
	* Entry point of OvEditor
	*/
	class Application
	{
	public:
		/**
		* Constructor
		* @param p_projectFolder (The folder containing the .ovproject file)
		*/
		Application(const std::filesystem::path& p_projectFolder);

		/**
		* Destructor
		*/
		virtual ~Application();

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
		Editor m_editor;
	};
}