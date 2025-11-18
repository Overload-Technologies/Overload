/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include <string>

#include "OvRendering/HAL/Common/TBuffer.h"

namespace OvRendering::Resources { class Shader; }

namespace OvRendering::HAL
{
	/**
	* Represents a uniform buffer, used to store uniform data that can be accessed by shaders
	*/
	template<Settings::EGraphicsBackend Backend, class UniformBufferContext, class BufferContext>
	class TUniformBuffer : public TBuffer<Backend, BufferContext>
	{
	public:
		/**
		* Creates a UniformBuffer
		*/
		TUniformBuffer();

	private:
		UniformBufferContext m_context;
	};
}
