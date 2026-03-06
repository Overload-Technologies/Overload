/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include <OvRendering/HAL/None/NoneShaderStorageBuffer.h>

template<>
OvRendering::HAL::NoneShaderStorageBuffer::TShaderStorageBuffer() : NoneBuffer(Settings::EBufferType::SHADER_STORAGE)
{
}
