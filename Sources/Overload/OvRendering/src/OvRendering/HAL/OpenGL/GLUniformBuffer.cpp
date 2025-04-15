/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include <glad.h>

#include <OvRendering/HAL/OpenGL/GLUniformBuffer.h>

template<>
OvRendering::HAL::GLUniformBuffer::TUniformBuffer() : GLBuffer(Settings::EBufferType::UNIFORM)
{
}
