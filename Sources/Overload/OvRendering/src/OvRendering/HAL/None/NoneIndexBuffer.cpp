/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include <OvRendering/HAL/None/NoneIndexBuffer.h>

template<>
OvRendering::HAL::NoneIndexBuffer::TIndexBuffer() : TBuffer(Settings::EBufferType::INDEX)
{
}
