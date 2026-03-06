/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#ifdef _WIN32
#include <windows.h>
#else
#include <cstdio>
#include <memory>
#include <array>
#endif

#include "OvWindowing/Dialogs/MessageBox.h"

#undef MessageBox

OvWindowing::Dialogs::MessageBox::MessageBox(std::string p_title, std::string p_message, EMessageType p_messageType, EButtonLayout p_buttonLayout, bool p_autoSpawn) :
	m_title(p_title),
	m_message(p_message),
	m_buttonLayout(p_buttonLayout),
	m_messageType(p_messageType)
{
	if (p_autoSpawn)
		Spawn();
}

const OvWindowing::Dialogs::MessageBox::EUserAction& OvWindowing::Dialogs::MessageBox::GetUserAction() const
{
	return m_userResult;
}

void OvWindowing::Dialogs::MessageBox::Spawn()
{
#ifdef _WIN32
	int msgboxID = MessageBoxA
	(
		nullptr,
		static_cast<LPCSTR>(m_message.c_str()),
		static_cast<LPCSTR>(m_title.c_str()),
		static_cast<UINT>(m_messageType) | static_cast<UINT>(m_buttonLayout) | MB_DEFBUTTON2
	);

	m_userResult = static_cast<EUserAction>(msgboxID);
#else
	// Linux implementation using zenity
	std::string command = "zenity --question"; // Always use question dialog for flexibility
	
	// Add title and message
	command += " --title=\"" + m_title + "\"";
	command += " --text=\"" + m_message + "\"";
	command += " --no-markup"; // Prevent markup interpretation issues
	
	// Handle button layout
	bool useExtraButtons = false;
	
	switch (m_buttonLayout)
	{
		case EButtonLayout::OK:
			command += " --ok-label=\"OK\" --no-cancel";
			break;
		case EButtonLayout::OK_CANCEL:
			command += " --ok-label=\"OK\" --cancel-label=\"Cancel\"";
			break;
		case EButtonLayout::YES_NO:
			command += " --ok-label=\"Yes\" --cancel-label=\"No\"";
			break;
		case EButtonLayout::YES_NO_CANCEL:
			useExtraButtons = true;
			// When using extra buttons, don't use ok-label or cancel-label
			command += " --extra-button=\"Yes\" --extra-button=\"No\" --extra-button=\"Cancel\"";
			break;
		case EButtonLayout::RETRY_CANCEL:
			command += " --ok-label=\"Retry\" --cancel-label=\"Cancel\"";
			break;
		case EButtonLayout::ABORT_RETRY_IGNORE:
			useExtraButtons = true;
			command += " --extra-button=\"Abort\" --extra-button=\"Retry\" --extra-button=\"Ignore\"";
			break;
		case EButtonLayout::CANCEL_TRYAGAIN_CONTINUE:
			useExtraButtons = true;
			command += " --extra-button=\"Cancel\" --extra-button=\"Try Again\" --extra-button=\"Continue\"";
			break;
		case EButtonLayout::HELP:
			command += " --ok-label=\"Help\" --no-cancel";
			break;
	}
	
	command += " 2>/dev/null"; // Suppress GTK warnings
	
	// Execute zenity and check exit code
	int exitCode = system(command.c_str());
	int zenityResult = WEXITSTATUS(exitCode);
	
	// Map zenity exit codes to EUserAction
	// zenity returns: 0 for OK/Yes, 1 for Cancel/No, 5 for timeout, 
	// -1 for error, and button text for extra buttons
	
	if (useExtraButtons)
	{
		// For extra buttons, we need to capture the output
		std::string captureCommand = command.substr(0, command.find(" 2>/dev/null"));
		captureCommand += " 2>/dev/null";
		
		std::array<char, 128> buffer;
		std::string result;
		std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(captureCommand.c_str(), "r"), pclose);
		
		if (pipe)
		{
			while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr)
			{
				result += buffer.data();
			}
		}
		
		// Remove trailing newline
		if (!result.empty() && result.back() == '\n')
			result.pop_back();
		
		// Map based on button text and layout
		if (m_buttonLayout == EButtonLayout::YES_NO_CANCEL)
		{
			if (result == "Yes")
				m_userResult = EUserAction::YES;
			else if (result == "No")
				m_userResult = EUserAction::NO;
			else
				m_userResult = EUserAction::CANCEL;
		}
		else if (m_buttonLayout == EButtonLayout::ABORT_RETRY_IGNORE)
		{
			if (result == "Abort")
				m_userResult = EUserAction::CANCEL; // Map Abort to Cancel (no direct equivalent)
			else if (result == "Retry")
				m_userResult = EUserAction::RETRY;
			else if (result == "Ignore")
				m_userResult = EUserAction::IGNORE;
			else
				m_userResult = EUserAction::CANCEL; // Dialog closed
		}
		else if (m_buttonLayout == EButtonLayout::CANCEL_TRYAGAIN_CONTINUE)
		{
			if (result == "Try Again")
				m_userResult = EUserAction::TRYAGAIN;
			else if (result == "Continue")
				m_userResult = EUserAction::CONTINUE;
			else
				m_userResult = EUserAction::CANCEL;
		}
		else
		{
			m_userResult = EUserAction::CANCEL;
		}
	}
	else
	{
		// Simple OK/Cancel mapping
		if (zenityResult == 0)
		{
			// OK/Yes/Retry button pressed
			switch (m_buttonLayout)
			{
				case EButtonLayout::YES_NO:
					m_userResult = EUserAction::YES;
					break;
				case EButtonLayout::RETRY_CANCEL:
					m_userResult = EUserAction::RETRY;
					break;
				default:
					m_userResult = EUserAction::OK;
					break;
			}
		}
		else
		{
			// Cancel/No button pressed
			switch (m_buttonLayout)
			{
				case EButtonLayout::YES_NO:
					m_userResult = EUserAction::NO;
					break;
				default:
					m_userResult = EUserAction::CANCEL;
					break;
			}
		}
	}
#endif
}
