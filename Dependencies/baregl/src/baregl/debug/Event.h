/**
* @project: baregl
* @author: Adrien Givry
* @licence: MIT
*/

#pragma once

#define NOTIFY_BUFFER_CREATED baregl::debug::OnBufferCreated(*this)
#define NOTIFY_BUFFER_DESTROYED baregl::debug::OnBufferDestroyed(*this)
#define NOTIFY_FRAMEBUFFER_CREATED baregl::debug::OnFramebufferCreated(*this)
#define NOTIFY_FRAMEBUFFER_DESTROYED baregl::debug::OnFramebufferDestroyed(*this)
#define NOTIFY_RENDERBUFFER_CREATED baregl::debug::OnRenderbufferCreated(*this)
#define NOTIFY_RENDERBUFFER_DESTROYED baregl::debug::OnRenderbufferDestroyed(*this)
#define NOTIFY_SHADER_PROGRAM_CREATED baregl::debug::OnShaderProgramCreated(*this)
#define NOTIFY_SHADER_PROGRAM_DESTROYED baregl::debug::OnShaderProgramDestroyed(*this)
#define NOTIFY_SHADER_STAGE_CREATED baregl::debug::OnShaderStageCreated(*this)
#define NOTIFY_SHADER_STAGE_DESTROYED baregl::debug::OnShaderStageDestroyed(*this)
#define NOTIFY_TEXTURE_CREATED baregl::debug::OnTextureCreated(*this)
#define NOTIFY_TEXTURE_DESTROYED baregl::debug::OnTextureDestroyed(*this)
#define NOTIFY_VERTEX_ARRAY_CREATED baregl::debug::OnVertexArrayCreated(*this)
#define NOTIFY_VERTEX_ARRAY_DESTROYED baregl::debug::OnVertexArrayDestroyed(*this)

namespace baregl
{
	class Buffer;
	class Framebuffer;
	class Renderbuffer;
	class ShaderProgram;
	class ShaderStage;
	class Texture;
	class VertexArray;
}

namespace baregl::debug
{
	void OnBufferCreated(const Buffer& p_buffer);
	void OnBufferDestroyed(const Buffer& p_buffer);
	void OnFramebufferCreated(const Framebuffer& p_framebuffer);
	void OnFramebufferDestroyed(const Framebuffer& p_framebuffer);
	void OnRenderbufferCreated(const Renderbuffer& p_renderbuffer);
	void OnRenderbufferDestroyed(const Renderbuffer& p_renderbuffer);
	void OnShaderProgramCreated(const ShaderProgram& p_shaderProgram);
	void OnShaderProgramDestroyed(const ShaderProgram& p_shaderProgram);
	void OnShaderStageCreated(const ShaderStage& p_shaderStage);
	void OnShaderStageDestroyed(const ShaderStage& p_shaderStage);
	void OnTextureCreated(const Texture& p_texture);
	void OnTextureDestroyed(const Texture& p_texture);
	void OnVertexArrayCreated(const VertexArray& p_vertexArray);
	void OnVertexArrayDestroyed(const VertexArray& p_vertexArray);
}

