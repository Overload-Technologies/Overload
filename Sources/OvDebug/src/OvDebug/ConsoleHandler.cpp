/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include "OvDebug/ConsoleHandler.h"
#include "OvDebug/ConsoleColor.h"

#include <iostream>

std::string const OvDebug::ConsoleHandler::__DEFAULT_HEADER;
std::string const OvDebug::ConsoleHandler::__INFO_HEADER = "[INFO] ";
std::string const OvDebug::ConsoleHandler::__WARNING_HEADER = "[WARNING] ";
std::string const OvDebug::ConsoleHandler::__ERROR_HEADER = "[ERROR] ";

void OvDebug::ConsoleHandler::Log(const LogData& p_logData)
{
	switch (p_logData.logLevel)
	{
	case ELogLevel::LOG_DEFAULT:
		std::cout << COLOR_WHITE;
		break;
	case ELogLevel::LOG_INFO:
		std::cout << COLOR_BLUE;
		break;
	case ELogLevel::LOG_WARNING:
		std::cout << COLOR_YELLOW;
		break;
	case ELogLevel::LOG_ERROR:
		std::cout << COLOR_RED;
		break;
	}

	std::ostream& output = p_logData.logLevel == ELogLevel::LOG_ERROR ? std::cerr : std::cout;

	output << GetLogHeader(p_logData.logLevel) << p_logData.date << " " << p_logData.message << std::endl;

	std::cout << COLOR_DEFAULT;
}

std::string OvDebug::ConsoleHandler::GetLogHeader(ELogLevel p_logLevel)
{
	switch (p_logLevel)
	{
	case ELogLevel::LOG_DEFAULT:	return __DEFAULT_HEADER;
	case ELogLevel::LOG_INFO:		return __INFO_HEADER;
	case ELogLevel::LOG_WARNING:	return __WARNING_HEADER;
	case ELogLevel::LOG_ERROR:		return __ERROR_HEADER;
	}

	return "";
}
