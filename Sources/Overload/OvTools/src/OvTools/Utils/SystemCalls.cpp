/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include <cassert>
#include <format>
#include <iostream>
#include <memory>

#include <OvTools/Utils/PathParser.h>
#include <OvTools/Utils/SystemCalls.h>

#include <ShlObj.h>
#include <Windows.h>

void OvTools::Utils::SystemCalls::ShowInExplorer(const std::string & p_path)
{
	ShellExecuteA(NULL, "open", OvTools::Utils::PathParser::MakeWindowsStyle(p_path).c_str(), NULL, NULL, SW_SHOWNORMAL);
}

void OvTools::Utils::SystemCalls::OpenFile(const std::string & p_file, const std::string & p_workingDir)
{
	ShellExecuteA(NULL, "open", OvTools::Utils::PathParser::MakeWindowsStyle(p_file).c_str(), NULL,
		p_workingDir.empty() ? NULL : OvTools::Utils::PathParser::MakeWindowsStyle(p_workingDir).c_str(),
		SW_SHOWNORMAL);
}

void OvTools::Utils::SystemCalls::EditFile(const std::string & p_file)
{
	ShellExecuteW(NULL, NULL, std::wstring(p_file.begin(), p_file.end()).c_str(), NULL, NULL, SW_SHOWNORMAL);
}

bool OvTools::Utils::SystemCalls::ExecuteCommand(const std::string_view p_command)
{
	STARTUPINFO startupInfo;
	PROCESS_INFORMATION processInfo;

	ZeroMemory(&startupInfo, sizeof(startupInfo));
	startupInfo.cb = sizeof(startupInfo);
	ZeroMemory(&processInfo, sizeof(processInfo));

	std::string command = std::format("cmd.exe /c {}", p_command);

	bool success = CreateProcess(
		nullptr,							// Application name (nullptr uses command line)
		command.data(),						// Command to execute
		nullptr,							// Process security attributes
		nullptr,							// Thread security attributes
		FALSE,								// Do not inherit handles
		CREATE_NO_WINDOW,					// Run the process without a window
		nullptr,							// Environment variables
		nullptr,							// Current directory
		&startupInfo,						// STARTUPINFO structure
		&processInfo						// PROCESS_INFORMATION structure
	);

	// Wait until child process exits.
	WaitForSingleObject(processInfo.hProcess, INFINITE);

	// Close the process and thread handles
	CloseHandle(processInfo.hProcess);
	CloseHandle(processInfo.hThread);

	return success;
}


void OvTools::Utils::SystemCalls::OpenURL(const std::string& p_url)
{
	ShellExecute(0, 0, p_url.c_str(), 0, 0, SW_SHOW);
}

std::string OvTools::Utils::SystemCalls::GetPathToAppdata()
{
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
}