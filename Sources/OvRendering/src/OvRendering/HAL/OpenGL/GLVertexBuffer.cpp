/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include <OvRendering/HAL/OpenGL/GLVertexBuffer.h>

template<>
OvRendering::HAL::GLVertexBuffer::TVertexBuffer() : TBuffer(Settings::EBufferType::VERTEX)
{
}
