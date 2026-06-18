/**
* @project: baregl
* @author: Adrien Givry
* @licence: MIT
*/

#pragma once

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
	/**
	* Base interface for custom event handlers
	*/
	class IEventHandler
	{
	public:
		/**
		* Destructor
		*/
		virtual ~IEventHandler() = default;

		/**
		* Invoked when a buffer is created
		* @param p_buffer
		*/
		virtual void OnBufferCreated(const Buffer& p_buffer) = 0;

		/**
		* Invoked when a buffer is destroyed
		* @param p_buffer
		*/
		virtual void OnBufferDestroyed(const Buffer& p_buffer) = 0;

		/**
		* Invoked when a framebuffer is created
		* @param p_framebuffer
		*/
		virtual void OnFramebufferCreated(const Framebuffer& p_framebuffer) = 0;

		/**
		* Invoked when a framebuffer is destroyed
		* @param p_framebuffer
		*/
		virtual void OnFramebufferDestroyed(const Framebuffer& p_framebuffer) = 0;

		/**
		* Invoked when a renderbuffer is created
		* @param p_renderbuffer
		*/
		virtual void OnRenderbufferCreated(const Renderbuffer& p_renderbuffer) = 0;

		/**
		* Invoked when a renderbuffer is destroyed
		* @param p_renderbuffer
		*/
		virtual void OnRenderbufferDestroyed(const Renderbuffer& p_renderbuffer) = 0;

		/**
		* Invoked when a shader program is created
		* @param p_shaderProgram
		*/
		virtual void OnShaderProgramCreated(const ShaderProgram& p_shaderProgram) = 0;

		/**
		* Invoked when a shader program is destroyed
		* @param p_shaderProgram
		*/
		virtual void OnShaderProgramDestroyed(const ShaderProgram& p_shaderProgram) = 0;

		/**
		* Invoked when a shader stage is created
		* @param p_shaderStage
		*/
		virtual void OnShaderStageCreated(const ShaderStage& p_shaderStage) = 0;

		/**
		* Invoked when a shader stage is destroyed
		* @param p_shaderStage
		*/
		virtual void OnShaderStageDestroyed(const ShaderStage& p_shaderStage) = 0;

		/**
		* Invoked when a texture is created
		* @param p_texture
		*/
		virtual void OnTextureCreated(const Texture& p_texture) = 0;

		/**
		* Invoked when a texture is destroyed
		* @param p_texture
		*/
		virtual void OnTextureDestroyed(const Texture& p_texture) = 0;

		/**
		* Invoked when a vertex array is created
		* @param p_vertexArray
		*/
		virtual void OnVertexArrayCreated(const VertexArray& p_vertexArray) = 0;

		/**
		* Invoked when a vertex array is destroyed
		* @param p_vertexArray
		*/
		virtual void OnVertexArrayDestroyed(const VertexArray& p_vertexArray) = 0;
	};
}

