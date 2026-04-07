/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include <filesystem>
#include <fstream>
#include <iterator>

#ifdef _WIN32
#include <Windows.h>
#else
#include <algorithm>
#include <array>
#include <cstdio>
#include <cstring>
#include <memory>
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
    std::string tempFile = "/tmp/ov_file_dialog.txt";
    std::string command = "zenity --file-selection --title=\"" + m_dialogTitle + "\"";
    
    if (m_isSaveDialog) command += " --save --confirm-overwrite";
    if (!m_initialDirectory.empty()) command += " --filename=\"" + m_initialDirectory + "/\"";

	if (!m_filter.empty())
	{
		std::istringstream stream(m_filter);
		std::string label, pattern;
		
		while (std::getline(stream, label, '\0') && std::getline(stream, pattern, '\0'))
		{
			std::ranges::replace(pattern, ';', ' ');
			if (pattern.ends_with(' ')) pattern.pop_back();
			command += " --file-filter=\"" + label + " | " + pattern + "\"";
		}
	}

    command += " > " + tempFile + " 2>/dev/null";

    int exitCode = std::system(command.c_str());

    if (exitCode == 0)
    {
        std::ifstream ifs(tempFile);
        std::string result((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
        ifs.close();
        std::filesystem::remove(tempFile);

        result.erase(std::remove(result.begin(), result.end(), '\n'), result.end());
        result.erase(std::remove(result.begin(), result.end(), '\r'), result.end());

        if (!result.empty())
        {
            m_filepath = result;
            m_succeeded = true;

            size_t lastSlash = m_filepath.find_last_of("/\\");
            m_filename = (lastSlash == std::string::npos) ? m_filepath : m_filepath.substr(lastSlash + 1);

            if (m_callback)
                m_callback(nullptr);
        }
    }
    else
    {
        m_succeeded = false;
        std::filesystem::remove(tempFile);
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
