/**
* @project: baregl
* @author: Adrien Givry
* @licence: MIT
*/

#pragma once

#include <string_view>

#define BAREGL_LOG_INFO(message) baregl::debug::Log<baregl::debug::ELogLevel::INFO>(message)
#define BAREGL_LOG_WARNING(message) baregl::debug::Log<baregl::debug::ELogLevel::WARNING>(message)
#define BAREGL_LOG_ERROR(message) baregl::debug::Log<baregl::debug::ELogLevel::ERROR>(message)

namespace baregl::debug
{
	enum class ELogLevel
	{
		INFO,
		WARNING,
		ERROR
	};

	template<ELogLevel Level>
	void Log(const std::string_view p_message);
}
