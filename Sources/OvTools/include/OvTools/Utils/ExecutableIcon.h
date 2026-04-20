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
	/**
	* Change the icon resource of an executable.
	* @param p_executablePath
	* @param p_iconPath
	* @param p_error
	* @return true on success
	*/
	bool ChangeExecutableIcon(
		const std::filesystem::path& p_executablePath,
		const std::filesystem::path& p_iconPath,
		std::string& p_error
	);
}
