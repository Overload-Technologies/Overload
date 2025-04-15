/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include <glad.h>

#include <OvRendering/HAL/OpenGL/GLIndexBuffer.h>
#include <OvRendering/HAL/OpenGL/GLTypes.h>

template<>
OvRendering::HAL::GLIndexBuffer::TIndexBuffer() : TBuffer(Settings::EBufferType::INDEX)
{
}
