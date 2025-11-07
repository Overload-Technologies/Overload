/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include <filesystem>

#ifdef _WIN32
#include <Windows.h>
#else
#include <cstdio>
#include <memory>
#include <array>
#include <cstring>
#endif

#include "OvWindowing/Dialogs/FileDialog.h"

OvWindowing::Dialogs::FileDialog::FileDialog(std::function<int(tagOFNA*)> p_callback, const std::string & p_dialogTitle) :
	m_callback(p_callback),
	m_dialogTitle(p_dialogTitle),
	m_initialDirectory("")
{
}

void OvWindowing::Dialogs::FileDialog::SetInitialDirectory(const std::string & p_initialDirectory)
{
	m_initialDirectory = p_initialDirectory;
}

void OvWindowing::Dialogs::FileDialog::Show(EExplorerFlags p_flags)
{
#ifdef _WIN32
	OPENFILENAME ofn;

	if (!m_initialDirectory.empty())
		m_filepath = m_initialDirectory;

	m_filepath.resize(MAX_PATH);

	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = NULL;  // If you have a window to center over, put its HANDLE here
	ofn.lpstrFilter = m_filter.c_str();
	ofn.lpstrFile = m_filepath.data();
	ofn.nMaxFile = MAX_PATH;
	ofn.lpstrTitle = m_dialogTitle.c_str();

	if (!m_initialDirectory.empty())
		ofn.lpstrInitialDir = m_initialDirectory.c_str();

	ofn.Flags = static_cast<DWORD>(p_flags);

	m_succeeded = m_callback(&ofn);

	if (!m_succeeded)
		HandleError();
	else
		m_filepath = m_filepath.c_str();

	/* Extract filename from filepath */
	m_filename.clear();
	for (auto it = m_filepath.rbegin(); it != m_filepath.rend() && *it != '\\' && *it != '/'; ++it)
		m_filename += *it;
	std::reverse(m_filename.begin(), m_filename.end());
#else
	// Linux implementation using zenity
	std::string command = "zenity --file-selection --title=\"" + m_dialogTitle + "\"";
	
	// Check if this is a save dialog (callback would be for save)
	bool isSaveDialog = false;
	if (reinterpret_cast<void*>(m_callback.target<int(*)(tagOFNA*)>()) != nullptr)
	{
		// This is a heuristic - if OVERWRITEPROMPT flag is set, it's likely a save dialog
		if ((static_cast<int>(p_flags) & static_cast<int>(EExplorerFlags::OVERWRITEPROMPT)) != 0)
			isSaveDialog = true;
	}
	
	if (isSaveDialog)
		command += " --save";
	
	if (!m_initialDirectory.empty())
		command += " --filename=\"" + m_initialDirectory + "/\"";
	
	// Add file filters if present
	if (!m_filter.empty())
	{
		std::string filters = m_filter;
		size_t pos = 0;
		while (pos < filters.length())
		{
			// Skip label (terminated by null)
			size_t labelEnd = filters.find('\0', pos);
			if (labelEnd == std::string::npos) break;
			
			pos = labelEnd + 1;
			if (pos >= filters.length()) break;
			
			// Get filter pattern
			size_t patternEnd = filters.find('\0', pos);
			if (patternEnd == std::string::npos) patternEnd = filters.length();
			
			std::string pattern = filters.substr(pos, patternEnd - pos);
			if (!pattern.empty())
			{
				command += " --file-filter='" + pattern + "'";
			}
			
			pos = patternEnd + 1;
		}
	}
	
	command += " 2>/dev/null"; // Suppress GTK warnings
	
	// Execute zenity and capture output
	std::array<char, 2048> buffer;
	std::string result;
	std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(command.c_str(), "r"), pclose);
	
	if (!pipe)
	{
		m_succeeded = false;
		m_error = "Failed to open zenity";
		return;
	}
	
	while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr)
	{
		result += buffer.data();
	}
	
	// Remove trailing newline
	if (!result.empty() && result.back() == '\n')
		result.pop_back();
	
	m_succeeded = !result.empty();
	
	if (m_succeeded)
	{
		m_filepath = result;
		
		/* Extract filename from filepath */
		m_filename.clear();
		for (auto it = m_filepath.rbegin(); it != m_filepath.rend() && *it != '\\' && *it != '/'; ++it)
			m_filename += *it;
		std::reverse(m_filename.begin(), m_filename.end());
	}
	else
	{
		m_error = "Dialog cancelled or zenity not available";
	}
#endif
}

bool OvWindowing::Dialogs::FileDialog::HasSucceeded() const
{
	return m_succeeded;
}

std::string OvWindowing::Dialogs::FileDialog::GetSelectedFileName()
{
	return m_filename;
}

std::string OvWindowing::Dialogs::FileDialog::GetSelectedFilePath()
{
	return m_filepath;
}

std::string OvWindowing::Dialogs::FileDialog::GetErrorInfo()
{
	return m_error;
}

bool OvWindowing::Dialogs::FileDialog::IsFileExisting() const
{
	return std::filesystem::exists(m_filepath);
}

void OvWindowing::Dialogs::FileDialog::HandleError()
{
#ifdef _WIN32
	switch (CommDlgExtendedError())
	{
	case CDERR_DIALOGFAILURE:	m_error = "CDERR_DIALOGFAILURE";   break;
	case CDERR_FINDRESFAILURE:	m_error = "CDERR_FINDRESFAILURE";  break;
	case CDERR_INITIALIZATION:	m_error = "CDERR_INITIALIZATION";  break;
	case CDERR_LOADRESFAILURE:	m_error = "CDERR_LOADRESFAILURE";  break;
	case CDERR_LOADSTRFAILURE:	m_error = "CDERR_LOADSTRFAILURE";  break;
	case CDERR_LOCKRESFAILURE:	m_error = "CDERR_LOCKRESFAILURE";  break;
	case CDERR_MEMALLOCFAILURE: m_error = "CDERR_MEMALLOCFAILURE"; break;
	case CDERR_MEMLOCKFAILURE:	m_error = "CDERR_MEMLOCKFAILURE";  break;
	case CDERR_NOHINSTANCE:		m_error = "CDERR_NOHINSTANCE";     break;
	case CDERR_NOHOOK:			m_error = "CDERR_NOHOOK";          break;
	case CDERR_NOTEMPLATE:		m_error = "CDERR_NOTEMPLATE";      break;
	case CDERR_STRUCTSIZE:		m_error = "CDERR_STRUCTSIZE";      break;
	case FNERR_BUFFERTOOSMALL:	m_error = "FNERR_BUFFERTOOSMALL";  break;
	case FNERR_INVALIDFILENAME: m_error = "FNERR_INVALIDFILENAME"; break;
	case FNERR_SUBCLASSFAILURE: m_error = "FNERR_SUBCLASSFAILURE"; break;
	default:					m_error = "You cancelled.";
	}
#else
	m_error = "Dialog operation failed";
#endif
}
