/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include <cstdint>

#include "OvDebug/Logger.h"
#include "OvTools/Time/Date.h"

OvTools::Eventing::Event<const OvDebug::LogData&> OvDebug::Logger::LogEvent;

std::map<std::string, OvDebug::ConsoleHandler>	OvDebug::Logger::CONSOLE_HANDLER_MAP;
std::map<std::string, OvDebug::FileHandler>		OvDebug::Logger::FILE_HANDLER_MAP;
std::map<std::string, OvDebug::HistoryHandler>	OvDebug::Logger::HISTORY_HANDLER_MAP;

void OvDebug::Logger::Log(const std::string& p_message, ELogLevel p_logLevel, ELogMode p_logMode, std::string p_handlerId)
{
	LogData logData{ p_message, p_logLevel, OvTools::Time::Date::GetDateAsString() };

	switch (p_logMode)
	{
	case ELogMode::DEFAULT:
	case ELogMode::CONSOLE: LogToHandlerMap<ConsoleHandler>(CONSOLE_HANDLER_MAP, logData, p_handlerId); break;
	case ELogMode::FILE:	LogToHandlerMap<FileHandler>(FILE_HANDLER_MAP, logData, p_handlerId);		break;
	case ELogMode::HISTORY: LogToHandlerMap<HistoryHandler>(HISTORY_HANDLER_MAP, logData, p_handlerId);	break;
	case ELogMode::ALL:
		LogToHandlerMap<ConsoleHandler>(CONSOLE_HANDLER_MAP, logData, p_handlerId);
		LogToHandlerMap<FileHandler>(FILE_HANDLER_MAP, logData, p_handlerId);
		LogToHandlerMap<HistoryHandler>(HISTORY_HANDLER_MAP, logData, p_handlerId);
		break;
	}

	LogEvent.Invoke(logData);
}

OvDebug::ConsoleHandler& OvDebug::Logger::CreateConsoleHandler(std::string p_id)
{
	CONSOLE_HANDLER_MAP.emplace(p_id, OvDebug::ConsoleHandler());
	return CONSOLE_HANDLER_MAP[p_id];
}

OvDebug::FileHandler& OvDebug::Logger::CreateFileHandler(std::string p_id)
{
	FILE_HANDLER_MAP.emplace(p_id, OvDebug::FileHandler());
	return FILE_HANDLER_MAP[p_id];
}

OvDebug::HistoryHandler& OvDebug::Logger::CreateHistoryHandler(std::string p_id)
{
	HISTORY_HANDLER_MAP.emplace(p_id, OvDebug::HistoryHandler());
	return HISTORY_HANDLER_MAP[p_id];
}

OvDebug::ConsoleHandler& OvDebug::Logger::GetConsoleHandler(std::string p_id)
{
	return CONSOLE_HANDLER_MAP[p_id];
}

OvDebug::FileHandler& OvDebug::Logger::GetFileHandler(std::string p_id)
{
	return FILE_HANDLER_MAP[p_id];
}

OvDebug::HistoryHandler& OvDebug::Logger::GetHistoryHandler(std::string p_id)
{
	return HISTORY_HANDLER_MAP[p_id];
}
