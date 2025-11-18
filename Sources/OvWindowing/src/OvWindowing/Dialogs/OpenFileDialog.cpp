/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include "OvWindowing/Dialogs/OpenFileDialog.h"

#ifdef _WIN32
#include <Windows.h>

OvWindowing::Dialogs::OpenFileDialog::OpenFileDialog(const std::string & p_dialogTitle) : FileDialog(GetOpenFileNameA, p_dialogTitle)
{
}
#else
// Dummy callback for Linux (not used, but needed for constructor)
static int DummyOpenCallback(tagOFNA*) { return 0; }

OvWindowing::Dialogs::OpenFileDialog::OpenFileDialog(const std::string & p_dialogTitle) : FileDialog(DummyOpenCallback, p_dialogTitle)
{
}
#endif

void OvWindowing::Dialogs::OpenFileDialog::AddFileType(const std::string & p_label, const std::string & p_filter)
{
	m_filter += p_label + '\0' + p_filter + '\0';
}
