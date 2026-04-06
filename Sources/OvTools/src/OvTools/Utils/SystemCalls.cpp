/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include <assert.h>
#include <memory>

#include <OvTools/Utils/PathParser.h>
#include <OvTools/Utils/SystemCalls.h>

#ifdef _WIN32
#include <ShlObj.h>
#include <Windows.h>
#include <shellapi.h>
#else
#include <cstdlib>
#include <pwd.h>
#include <sys/types.h>
#include <unistd.h>
#endif

void OvTools::Utils::SystemCalls::ShowInExplorer(const std::string& p_path)
{
#ifdef _WIN32
	ShellExecuteA(nullptr, "open", OvTools::Utils::PathParser::MakeWindowsStyle(p_path).c_str(), nullptr, nullptr, SW_SHOWNORMAL);
#else
	std::string command = "xdg-open \"" + p_path + "\" &";
	std::system(command.c_str());
#endif
}

void OvTools::Utils::SystemCalls::OpenFile(const std::string& p_file, const std::string& p_workingDir)
{
#ifdef _WIN32
	const std::string filePath = OvTools::Utils::PathParser::MakeWindowsStyle(p_file);
	const std::string workingDirectory = p_workingDir.empty() ? std::string{} : OvTools::Utils::PathParser::MakeWindowsStyle(p_workingDir);

	ShellExecuteA(
		nullptr,
		"open",
		filePath.c_str(),
		nullptr,
		workingDirectory.empty() ? nullptr : workingDirectory.c_str(),
		SW_SHOWNORMAL
	);
#else
	std::string command = "xdg-open \"" + p_file + "\" &";
	if (!p_workingDir.empty())
	{
		command = "cd \"" + p_workingDir + "\" && " + command;
	}

	std::system(command.c_str());
#endif
}

void OvTools::Utils::SystemCalls::RunProgram(const std::string& p_file, const std::string& p_workingDir)
{
#ifdef _WIN32
	OpenFile(p_file, p_workingDir);
#else
	std::string command = "\"" + p_file + "\" &";
	if (!p_workingDir.empty())
	{
		command = "cd \"" + p_workingDir + "\" && " + command;
	}

	std::system(command.c_str());
#endif
}

void OvTools::Utils::SystemCalls::EditFile(const std::string& p_file)
{
#ifdef _WIN32
	ShellExecuteW(nullptr, nullptr, std::wstring(p_file.begin(), p_file.end()).c_str(), nullptr, nullptr, SW_NORMAL);
#else
	std::string command = "xdg-open \"" + p_file + "\" &";
	std::system(command.c_str());
#endif
}

void OvTools::Utils::SystemCalls::OpenURL(const std::string& p_url)
{
#ifdef _WIN32
	ShellExecuteA(nullptr, nullptr, p_url.c_str(), nullptr, nullptr, SW_SHOW);
#else
	std::string command = "xdg-open \"" + p_url + "\" &";
	std::system(command.c_str());
#endif
}

std::string OvTools::Utils::SystemCalls::GetPathToAppdata()
{
#ifdef _WIN32
	PWSTR rawPath = nullptr;
	const HRESULT hr = SHGetKnownFolderPath(FOLDERID_RoamingAppData, 0, nullptr, &rawPath);
	std::unique_ptr<wchar_t, decltype(&CoTaskMemFree)> path(rawPath, CoTaskMemFree);
	assert(SUCCEEDED(hr) && "Failed to get AppData path");

	const int utf8PathSize = WideCharToMultiByte(CP_UTF8, 0, path.get(), -1, nullptr, 0, nullptr, nullptr);
	assert(utf8PathSize > 0 && "failed to convert from wide char to UTF-8");

	std::string appDataPath(utf8PathSize - 1, 0);
	WideCharToMultiByte(CP_UTF8, 0, path.get(), -1, &appDataPath[0], utf8PathSize, nullptr, nullptr);
	return appDataPath;
#else
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

	struct passwd* password = getpwuid(getuid());
	assert(password != nullptr && "Failed to get user home directory");
	return std::string(password->pw_dir) + "/.config";
#endif
}

