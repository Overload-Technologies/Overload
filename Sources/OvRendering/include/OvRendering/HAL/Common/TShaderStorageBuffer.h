/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include <OvRendering/HAL/Common/TBuffer.h>
#include <OvRendering/Settings/EGraphicsBackend.h>

namespace OvRendering::HAL
{
	/**
	* Represents a shader storage buffer, used to store data of variable size that can be accessed by shaders.
	*/
	template<Settings::EGraphicsBackend Backend, class ShaderStorageBufferContext, class BufferContext>
	class TShaderStorageBuffer : public TBuffer<Backend, BufferContext>
	{
	public:
		/**
		* Creates a shader storage buffer.
		*/
		TShaderStorageBuffer();

	private:
		ShaderStorageBufferContext m_context;
	};
}
