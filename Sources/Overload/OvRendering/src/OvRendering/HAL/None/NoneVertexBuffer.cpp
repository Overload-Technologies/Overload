/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include <OvRendering/HAL/None/NoneVertexBuffer.h>

template<>
OvRendering::HAL::NoneVertexBuffer::TVertexBuffer() : TBuffer(Settings::EBufferType::VERTEX)
{
}
