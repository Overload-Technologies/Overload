/**
* @project: baregl
* @author: Adrien Givry
* @licence: MIT
*/

#include <baregl/debug/Assert.h>

#include <baregl/debug/IAssertHandler.h>

#include <cstdio>
#include <memory>

namespace
{
	class DefaultAssertHandler final : public baregl::debug::IAssertHandler
	{
	public:
		virtual void OnAssert(const std::string_view p_message) override
		{
			fprintf(
				stdout,
				"[baregl] <assert> %s\n",
				p_message.data()
			);
			std::abort();
		}
	};

	std::unique_ptr<baregl::debug::IAssertHandler> g_assertHandler = std::make_unique<DefaultAssertHandler>();
}

namespace baregl::debug
{
	void SetAssertHandler(std::unique_ptr<IAssertHandler> p_handler)
	{
		g_assertHandler = std::move(p_handler);
	}

	void ResetAssertHandler()
	{
		g_assertHandler = std::make_unique<DefaultAssertHandler>();
	}

	void Assert(bool condition, const std::string_view p_message)
	{
		if (!condition)
		{
			g_assertHandler->OnAssert(p_message);
		}
	}
}

