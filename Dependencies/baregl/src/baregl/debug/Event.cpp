/**
* @project: baregl
* @author: Adrien Givry
* @licence: MIT
*/

#include <baregl/debug/Event.h>

#include <baregl/debug/IEventHandler.h>

#include <memory>

namespace
{
	class DefaultEventHandler final : public baregl::debug::IEventHandler
	{
	public:
		virtual void OnBufferCreated([[maybe_unused]] const baregl::Buffer&) override {}
		virtual void OnBufferDestroyed([[maybe_unused]] const baregl::Buffer&) override {}
		virtual void OnFramebufferCreated([[maybe_unused]] const baregl::Framebuffer&) override {}
		virtual void OnFramebufferDestroyed([[maybe_unused]] const baregl::Framebuffer&) override {}
		virtual void OnRenderbufferCreated([[maybe_unused]] const baregl::Renderbuffer&) override {}
		virtual void OnRenderbufferDestroyed([[maybe_unused]] const baregl::Renderbuffer&) override {}
		virtual void OnShaderProgramCreated([[maybe_unused]] const baregl::ShaderProgram&) override {}
		virtual void OnShaderProgramDestroyed([[maybe_unused]] const baregl::ShaderProgram&) override {}
		virtual void OnShaderStageCreated([[maybe_unused]] const baregl::ShaderStage&) override {}
		virtual void OnShaderStageDestroyed([[maybe_unused]] const baregl::ShaderStage&) override {}
		virtual void OnTextureCreated([[maybe_unused]] const baregl::Texture&) override {}
		virtual void OnTextureDestroyed([[maybe_unused]] const baregl::Texture&) override {}
		virtual void OnVertexArrayCreated([[maybe_unused]] const baregl::VertexArray&) override {}
		virtual void OnVertexArrayDestroyed([[maybe_unused]] const baregl::VertexArray&) override {}
	};

	std::unique_ptr<baregl::debug::IEventHandler> g_eventHandler = std::make_unique<DefaultEventHandler>();
}

namespace baregl::debug
{
	void SetEventHandler(std::unique_ptr<IEventHandler> p_handler)
	{
		g_eventHandler = std::move(p_handler);
	}

	void ResetEventHandler()
	{
		g_eventHandler = std::make_unique<DefaultEventHandler>();
	}

	void OnBufferCreated(const Buffer& p_buffer) { g_eventHandler->OnBufferCreated(p_buffer); }
	void OnBufferDestroyed(const Buffer& p_buffer) { g_eventHandler->OnBufferDestroyed(p_buffer); }
	void OnFramebufferCreated(const Framebuffer& p_framebuffer) { g_eventHandler->OnFramebufferCreated(p_framebuffer); }
	void OnFramebufferDestroyed(const Framebuffer& p_framebuffer) { g_eventHandler->OnFramebufferDestroyed(p_framebuffer); }
	void OnRenderbufferCreated(const Renderbuffer& p_renderbuffer) { g_eventHandler->OnRenderbufferCreated(p_renderbuffer); }
	void OnRenderbufferDestroyed(const Renderbuffer& p_renderbuffer) { g_eventHandler->OnRenderbufferDestroyed(p_renderbuffer); }
	void OnShaderProgramCreated(const ShaderProgram& p_shaderProgram) { g_eventHandler->OnShaderProgramCreated(p_shaderProgram); }
	void OnShaderProgramDestroyed(const ShaderProgram& p_shaderProgram) { g_eventHandler->OnShaderProgramDestroyed(p_shaderProgram); }
	void OnShaderStageCreated(const ShaderStage& p_shaderStage) { g_eventHandler->OnShaderStageCreated(p_shaderStage); }
	void OnShaderStageDestroyed(const ShaderStage& p_shaderStage) { g_eventHandler->OnShaderStageDestroyed(p_shaderStage); }
	void OnTextureCreated(const Texture& p_texture) { g_eventHandler->OnTextureCreated(p_texture); }
	void OnTextureDestroyed(const Texture& p_texture) { g_eventHandler->OnTextureDestroyed(p_texture); }
	void OnVertexArrayCreated(const VertexArray& p_vertexArray) { g_eventHandler->OnVertexArrayCreated(p_vertexArray); }
	void OnVertexArrayDestroyed(const VertexArray& p_vertexArray) { g_eventHandler->OnVertexArrayDestroyed(p_vertexArray); }
}

