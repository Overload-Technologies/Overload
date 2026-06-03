/**
* @project: baregl
* @author: Adrien Givry
* @licence: MIT
*/

#pragma once

#include <string_view>

namespace baregl::debug
{
	/**
	* Base interface for custom log handlers
	*/
	class ILogHandler
	{
	public:
		/**
		* Destructor
		*/
		virtual ~ILogHandler() = default;

		/**
		* Invoked when baregl wants to log an info message
		* @param p_message
		*/
		virtual void LogInfo(const std::string_view p_message) = 0;

		/**
		* Invoked when baregl wants to log a warning message
		* @param p_message
		*/
		virtual void LogWarning(const std::string_view p_message) = 0;

		/**
		* Invoked when baregl wants to log an error message
		* @param p_message
		*/
		virtual void LogError(const std::string_view p_message) = 0;
	};
}

