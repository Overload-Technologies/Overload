/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include "OvDebug/HistoryHandler.h"

std::queue<OvDebug::LogData> OvDebug::HistoryHandler::LOG_QUEUE;

void OvDebug::HistoryHandler::Log(const LogData& p_logData)
{
	LOG_QUEUE.push(p_logData);
}

std::queue<OvDebug::LogData>& OvDebug::HistoryHandler::GetLogQueue()
{
	return LOG_QUEUE;
}