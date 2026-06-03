/**
* @project: baregl
* @author: Adrien Givry
* @licence: MIT
*/

#include <baregl/debug/Log.h>

#include <baregl/debug/ILogHandler.h>

#include <cstdio>
#include <memory>

namespace
{
	class DefaultLogHandler final : public baregl::debug::ILogHandler
	{
	public:
		virtual void LogInfo(const std::string_view p_message) override
		{
			fprintf(
				stdout,
				"[baregl] <info> %s\n",
				p_message.data()
			);
		}

		virtual void LogWarning(const std::string_view p_message) override
		{
			fprintf(
				stdout,
				"[baregl] <warning> %s\n",
				p_message.data()
			);
		}

		virtual void LogError(const std::string_view p_message) override
		{
			fprintf(
				stderr,
				"[baregl] <error> %s\n",
				p_message.data()
			);
		}
	};

	std::unique_ptr<baregl::debug::ILogHandler> g_logHandler = std::make_unique<DefaultLogHandler>();
}

namespace baregl::debug
{
	void SetLogHandler(std::unique_ptr<ILogHandler> p_handler)
	{
		g_logHandler = std::move(p_handler);
	}

	void ResetLogHandler()
	{
		g_logHandler = std::make_unique<DefaultLogHandler>();
	}

	template<>
	void Log<ELogLevel::INFO>(const std::string_view p_message)
	{
		g_logHandler->LogInfo(p_message);
	}

	template<>
	void Log<ELogLevel::WARNING>(const std::string_view p_message)
	{
		g_logHandler->LogWarning(p_message);
	}

	template<>
	void Log<ELogLevel::ERROR>(const std::string_view p_message)
	{
		g_logHandler->LogError(p_message);
	}
}
