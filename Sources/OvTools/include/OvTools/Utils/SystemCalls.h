/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include <filesystem>
#include <string>


namespace OvTools::Utils
{
	class SystemCalls
	{
	public:
		/**
		* Disabled constructor
		*/
		SystemCalls() = delete;

		/**
		* Open the windows explorer at the given path
		* @param p_path
		*/
		static void ShowInExplorer(const std::string& p_path);

		/**
		* Open the given file with the default application
		* @param p_file
		* @param p_workingDir
		*/
		static void OpenFile(const std::string& p_file, const std::string & p_workingDir = "");

		/**
		* Run a program
		* @param p_file
		* @param p_workingDir
		*/
		static void RunProgram(const std::string& p_file, const std::string& p_workingDir = "");

		/**
		* Apply the given icon image to an executable icon resource.
		* @note No-op on Linux, as executable files don't embed icons.
		* @param p_executablePath
		* @param p_iconPath
		*/
		static bool SetExecutableIcon(
			[[maybe_unused]] const std::filesystem::path& p_executablePath,
			[[maybe_unused]] const std::filesystem::path& p_iconPath
		);

		/**
		* Open the given file for edition with the default application
		* @param p_file
		*/
		static void EditFile(const std::string& p_file);

		/**
		* Open the given url with the default browser
		* @param p_url
		*/
		static void OpenURL(const std::string& p_url);

		/**
		* Return the path to APPDATA
 		*/
		static std::string GetPathToAppdata();

		/**
		* Execute a custom command. Returns true if the command invocation succeeded
		* @param p_command
		*/
		static bool ExecuteCommand(const std::string_view p_command);
	};
}
