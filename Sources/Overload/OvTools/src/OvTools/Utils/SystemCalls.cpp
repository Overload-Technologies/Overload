/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include "OvTools/Utils/PathParser.h"
#include "OvTools/Utils/SystemCalls.h"

#ifdef _WIN32
#include <Windows.h>
#include <ShlObj.h>
#else
#include <cstdlib>
#include <unistd.h>
#include <pwd.h>
#include <sys/types.h>
#endif

#include <memory>
#include <assert.h>

void OvTools::Utils::SystemCalls::ShowInExplorer(const std::string & p_path)
{
#ifdef _WIN32
	ShellExecuteA(NULL, "open", OvTools::Utils::PathParser::MakeWindowsStyle(p_path).c_str(), NULL, NULL, SW_SHOWNORMAL);
#else
	std::string command = "xdg-open \"" + p_path + "\" &";
	std::system(command.c_str());
#endif
}

void OvTools::Utils::SystemCalls::OpenFile(const std::string & p_file, const std::string & p_workingDir)
{
#ifdef _WIN32
	ShellExecuteA(NULL, "open", OvTools::Utils::PathParser::MakeWindowsStyle(p_file).c_str(), NULL,
		p_workingDir.empty() ? NULL : OvTools::Utils::PathParser::MakeWindowsStyle(p_workingDir).c_str(),
		SW_SHOWNORMAL);
#else
	std::string command = "xdg-open \"" + p_file + "\" &";
	if (!p_workingDir.empty())
	{
		command = "cd \"" + p_workingDir + "\" && " + command;
	}
	std::system(command.c_str());
#endif
}

void OvTools::Utils::SystemCalls::EditFile(const std::string & p_file)
{
#ifdef _WIN32
	ShellExecuteW(NULL, NULL, std::wstring(p_file.begin(), p_file.end()).c_str(), NULL, NULL, SW_NORMAL);
#else
	std::string command = "xdg-open \"" + p_file + "\" &";
	std::system(command.c_str());
#endif
}

void OvTools::Utils::SystemCalls::OpenURL(const std::string& p_url)
{
#ifdef _WIN32
	ShellExecute(0, 0, p_url.c_str(), 0, 0, SW_SHOW);
#else
	std::string command = "xdg-open \"" + p_url + "\" &";
	std::system(command.c_str());
#endif
}

std::string OvTools::Utils::SystemCalls::GetPathToAppdata()
{
#ifdef _WIN32
	// Retrieve app-data path
	PWSTR rawPath = nullptr;
	const HRESULT hr = SHGetKnownFolderPath(FOLDERID_RoamingAppData, 0, nullptr, &rawPath);
	std::unique_ptr<wchar_t, decltype(&CoTaskMemFree)> path(rawPath, CoTaskMemFree);
	assert(SUCCEEDED(hr) && "Failed to get AppData path");
	
	// Convert app-data path from wide char to UTF-8 string
	const int size_needed = WideCharToMultiByte(CP_UTF8, 0, path.get(), -1, nullptr, 0, nullptr, nullptr);
	assert(size_needed > 0 && "failed to convert from wide char to UTF-8");
	std::string appDataPath(size_needed - 1, 0);
	WideCharToMultiByte(CP_UTF8, 0, path.get(), -1, &appDataPath[0], size_needed, nullptr, nullptr);
	return appDataPath;
#else
	// On Linux, use the home directory or XDG_CONFIG_HOME
	const char* configHome = std::getenv("XDG_CONFIG_HOME");
	if (configHome != nullptr && configHome[0] != '\0')
	{
		return std::string(configHome);
	}
	
	const char* home = std::getenv("HOME");
	if (home != nullptr && home[0] != '\0')
	{
		return std::string(home) + "/.config";
	}
	
	// Fallback to pwd if HOME is not set
	struct passwd* pw = getpwuid(getuid());
	assert(pw != nullptr && "Failed to get user home directory");
	return std::string(pw->pw_dir) + "/.config";
#endif
}