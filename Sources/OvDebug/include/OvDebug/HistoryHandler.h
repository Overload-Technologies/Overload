/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include <string>
#include <queue>

#include "ILogHandler.h"

namespace OvDebug
{
	/*
	* Treat the log in a history queue
	*/
	class HistoryHandler : public ILogHandler
	{
	public:

		/**
		* Log to the history
		*/
		void Log(const LogData& p_logData);

		/**
		* Return log queue
		*/
		static std::queue<LogData>& GetLogQueue();

	private:

		static std::queue<LogData> LOG_QUEUE;
	};
}