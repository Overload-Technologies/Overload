/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include <OvTools/Utils/PathParser.h>
#include <OvTools/Utils/SystemCalls.h>

#ifdef _WIN32
#include <Windows.h>
#include <ShlObj.h>
#else
#include <cstdlib>
#include <unistd.h>
#include <pwd.h>
#include <sys/types.h>
#include <sys/wait.h>
#endif

#include <cassert>
#include <cstring>
#include <format>
#include <limits>
#include <memory>
#include <vector>

namespace
{
	bool CommandExists(const std::string_view p_cmd)
	{
#ifdef _WIN32

		const std::string cmd{ p_cmd.substr(0, p_cmd.find(' ')) };
		const std::string query{ "where /q " + cmd + " 2>NUL" };
		return std::system(query.c_str()) == 0;
#else
		std::string checkCmd = std::format("command -v {} > /dev/null 2>&1", p_cmd);
		FILE* pipe = popen(checkCmd.c_str(), "r");
		if (!pipe) return false;
		return WEXITSTATUS(pclose(pipe)) == 0;
#endif
	}
}

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

bool OvTools::Utils::SystemCalls::SetExecutableIcon(
	[[maybe_unused]] const std::filesystem::path& p_executablePath,
	[[maybe_unused]] const std::span<const uint8_t> p_iconData,
	[[maybe_unused]] const uint32_t p_iconWidth,
	[[maybe_unused]] const uint32_t p_iconHeight
)
{
#if defined(_WIN32)
	constexpr WORD kGroupResourceId = 101;
	constexpr WORD kImageResourceId = 65000;
	constexpr UINT kMaxIconSize = 256;
	constexpr WORD kLanguageIds[] = {
		MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US),
		MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_CAN),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL)
	};

	if (p_iconWidth == 0u || p_iconHeight == 0u || p_iconData.empty())
	{
		return false;
	}

	if (p_iconWidth > (std::numeric_limits<size_t>::max)() / 4u)
	{
		return false;
	}

	const size_t sourceRowBytes = static_cast<size_t>(p_iconWidth) * 4u;
	if (p_iconHeight > (std::numeric_limits<size_t>::max)() / sourceRowBytes)
	{
		return false;
	}

	const size_t sourceBytes = sourceRowBytes * static_cast<size_t>(p_iconHeight);
	if (p_iconData.size() < sourceBytes)
	{
		return false;
	}

	auto writeWord = [](std::vector<BYTE>& p_data, const size_t p_offset, const WORD p_value)
	{
		p_data[p_offset] = static_cast<BYTE>(p_value & 0xFFu);
		p_data[p_offset + 1u] = static_cast<BYTE>((p_value >> 8u) & 0xFFu);
	};
	auto writeDword = [](std::vector<BYTE>& p_data, const size_t p_offset, const DWORD p_value)
	{
		p_data[p_offset] = static_cast<BYTE>(p_value & 0xFFu);
		p_data[p_offset + 1u] = static_cast<BYTE>((p_value >> 8u) & 0xFFu);
		p_data[p_offset + 2u] = static_cast<BYTE>((p_value >> 16u) & 0xFFu);
		p_data[p_offset + 3u] = static_cast<BYTE>((p_value >> 24u) & 0xFFu);
	};

	const UINT iconWidth = p_iconWidth > kMaxIconSize ? kMaxIconSize : p_iconWidth;
	const UINT iconHeight = p_iconHeight > kMaxIconSize ? kMaxIconSize : p_iconHeight;
	if (iconWidth == 0u || iconHeight == 0u)
	{
		return false;
	}

	const size_t rowBytes = static_cast<size_t>(iconWidth) * 4u;
	std::vector<BYTE> xorBitmapData(rowBytes * static_cast<size_t>(iconHeight));

	// OvRendering::Data::Image loads pixels in bottom-up order, which matches ICO DIB expectations.
	for (UINT y = 0u; y < iconHeight; ++y)
	{
		const UINT sourceY = static_cast<UINT>((static_cast<uint64_t>(y) * p_iconHeight) / iconHeight);
		const size_t dstOffset = static_cast<size_t>(y) * rowBytes;

		for (UINT x = 0u; x < iconWidth; ++x)
		{
			const UINT sourceX = static_cast<UINT>((static_cast<uint64_t>(x) * p_iconWidth) / iconWidth);
			const size_t sourceOffset = (static_cast<size_t>(sourceY) * p_iconWidth + sourceX) * 4u;
			const size_t dstPixelOffset = dstOffset + static_cast<size_t>(x) * 4u;

			xorBitmapData[dstPixelOffset + 0u] = p_iconData[sourceOffset + 2u];
			xorBitmapData[dstPixelOffset + 1u] = p_iconData[sourceOffset + 1u];
			xorBitmapData[dstPixelOffset + 2u] = p_iconData[sourceOffset + 0u];
			xorBitmapData[dstPixelOffset + 3u] = p_iconData[sourceOffset + 3u];
		}
	}

	const size_t andMaskRowBytes = static_cast<size_t>((iconWidth + 31u) / 32u) * 4u;
	std::vector<BYTE> andMaskData(andMaskRowBytes * static_cast<size_t>(iconHeight), 0u);
	BITMAPINFOHEADER iconHeader{};
	iconHeader.biSize = sizeof(BITMAPINFOHEADER);
	iconHeader.biWidth = static_cast<LONG>(iconWidth);
	iconHeader.biHeight = static_cast<LONG>(iconHeight * 2u);
	iconHeader.biPlanes = 1;
	iconHeader.biBitCount = 32;
	iconHeader.biCompression = BI_RGB;
	iconHeader.biSizeImage = static_cast<DWORD>(xorBitmapData.size());

	std::vector<BYTE> iconResourceData(sizeof(BITMAPINFOHEADER) + xorBitmapData.size() + andMaskData.size());
	std::memcpy(iconResourceData.data(), &iconHeader, sizeof(BITMAPINFOHEADER));
	std::memcpy(iconResourceData.data() + sizeof(BITMAPINFOHEADER), xorBitmapData.data(), xorBitmapData.size());
	std::memcpy(iconResourceData.data() + sizeof(BITMAPINFOHEADER) + xorBitmapData.size(), andMaskData.data(), andMaskData.size());

	std::vector<BYTE> groupResourceData(20u, 0u);
	writeWord(groupResourceData, 2u, 1u);
	writeWord(groupResourceData, 4u, 1u);
	groupResourceData[6u] = iconWidth >= kMaxIconSize ? 0u : static_cast<BYTE>(iconWidth);
	groupResourceData[7u] = iconHeight >= kMaxIconSize ? 0u : static_cast<BYTE>(iconHeight);
	writeWord(groupResourceData, 10u, 1u);
	writeWord(groupResourceData, 12u, 32u);
	writeDword(groupResourceData, 14u, static_cast<DWORD>(iconResourceData.size()));
	writeWord(groupResourceData, 18u, kImageResourceId);

	const std::wstring executablePath = p_executablePath.wstring();
	HANDLE updateHandle = BeginUpdateResourceW(executablePath.c_str(), FALSE);
	if (!updateHandle)
	{
		return false;
	}

	bool updated = false;
	for (const WORD languageId : kLanguageIds)
	{
		const bool iconUpdated = UpdateResourceW(
			updateHandle,
			MAKEINTRESOURCEW(3),
			MAKEINTRESOURCEW(kImageResourceId),
			languageId,
			iconResourceData.data(),
			static_cast<DWORD>(iconResourceData.size())
		) != FALSE;
		const bool groupUpdated = UpdateResourceW(
			updateHandle,
			MAKEINTRESOURCEW(14),
			MAKEINTRESOURCEW(kGroupResourceId),
			languageId,
			groupResourceData.data(),
			static_cast<DWORD>(groupResourceData.size())
		) != FALSE;

		if (iconUpdated && groupUpdated)
		{
			updated = true;
		}
	}

	if (!updated)
	{
		EndUpdateResourceW(updateHandle, TRUE);
		return false;
	}

	return EndUpdateResourceW(updateHandle, FALSE) != FALSE;
#else
	return false;
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

bool OvTools::Utils::SystemCalls::ExecuteCommand(const std::string_view p_command)
{
#if defined(_WIN32)
	STARTUPINFO startupInfo;
	PROCESS_INFORMATION processInfo;

	ZeroMemory(&startupInfo, sizeof(startupInfo));
	startupInfo.cb = sizeof(startupInfo);
	ZeroMemory(&processInfo, sizeof(processInfo));

	std::string command = std::format("cmd.exe /c {}", p_command);

	bool success = CreateProcess(
		nullptr,			// Application name (nullptr uses command line)
		command.data(),			// Command to execute
		nullptr,			// Process security attributes
		nullptr,			// Thread security attributes
		FALSE,				// Do not inherit handles
		CREATE_NO_WINDOW,		// Run the process without a window
		nullptr,			// Environment variables
		nullptr,			// Current directory
		&startupInfo,			// STARTUPINFO structure
		&processInfo			// PROCESS_INFORMATION structure
	);

	if (!success)
		return false;

	// Wait until child process exits
	WaitForSingleObject(processInfo.hProcess, INFINITE);

	// Check the exit code of the child process
	DWORD exitCode = 0;
	GetExitCodeProcess(processInfo.hProcess, &exitCode);

	// Close the process and thread handles
	CloseHandle(processInfo.hProcess);
	CloseHandle(processInfo.hThread);

	return exitCode == 0;
#else
	// Best way I found to reliably check if the command is going to succeed on UNIX.
	// Running the command detached doesn't give us enough information on the exit code.
	if (!CommandExists(p_command))
	{
		return false;
	}

	std::string command{ p_command };
	command += " &"; // Ensures the command is run detached on UNIX
	return std::system(command.c_str()) == 0;
#endif
}
