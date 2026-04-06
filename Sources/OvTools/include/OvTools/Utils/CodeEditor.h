/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include <string>
#include <vector>


namespace OvTools::Utils
{
	/**
	* Handle code editor registrations and project opening commands.
	*/
	class CodeEditor
	{
	public:
		/**
		* Disabled constructor
		*/
		CodeEditor() = delete;

		/**
		* Register a code editor with its command and argument format.
		* Available placeholders in argument format are:
		* {path}, {path_windows}, {path_unix}
		* @param p_name
		* @param p_command
		* @param p_argumentFormat
		*/
		static void Register
		(
			const std::string& p_name,
			const std::string& p_command,
			const std::string& p_argumentFormat = "\"{path}\""
		);

		/**
		* Unregister a code editor.
		* Returns false if the editor name is not registered.
		* @param p_name
		*/
		static bool Unregister(const std::string& p_name);

		/**
		* Open the given path in the requested registered code editor.
		* Returns true if the editor command was launched.
		* @param p_name
		* @param p_path
		*/
		static bool Open(const std::string& p_name, const std::string& p_path);

		/**
		* Returns the list of registered code editor names.
		*/
		static std::vector<std::string> GetRegistered();

		/**
		* Set the default code editor used by OpenDefault.
		* Returns false if the editor name is not registered.
		* @param p_name
		*/
		static bool SetDefault(const std::string& p_name);

		/**
		* Open the given path in the default code editor.
		* Falls back to file explorer if the editor is unavailable.
		* Returns true if opened in editor, false if fallback was used.
		* @param p_path
		*/
		static bool OpenDefault(const std::string& p_path);
	};
}

