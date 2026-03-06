/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include <string>


namespace OvDebug
{
	/**
	* Some log levels
	*/
	enum class ELogLevel
	{
		LOG_DEFAULT,
		LOG_INFO,
		LOG_WARNING,
		LOG_ERROR
	};

	/**
	* Some log modes (Indicates the handler to use)
	*/
	enum class ELogMode
	{
		DEFAULT,
		CONSOLE,
		FILE,
		HISTORY,
		ALL
	};

	/**
	* Store the log information
	*/
	struct LogData
	{
		std::string message;
		ELogLevel logLevel;
		std::string date;
	};

	/*
	* Treat the log in a certain way (Defined by children)
	*/
	class ILogHandler
	{
		friend class Logger;

	private:
		/**
		* Log the the handler
		* @param p_logData
		*/
		virtual void Log(const LogData& p_logData) = 0;
	};
}