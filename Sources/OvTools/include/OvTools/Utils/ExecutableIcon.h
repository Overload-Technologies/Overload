/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include <filesystem>

namespace OvTools::Utils
{
	/**
	* On Windows, apply the given icon image to an executable icon resource.
	* On other platforms, this function does nothing and returns false.
	* @param p_executablePath
	* @param p_iconPath
	*/
	bool SetWindowsExecutableIcon(
		const std::filesystem::path& p_executablePath,
		const std::filesystem::path& p_iconPath
	);
}
