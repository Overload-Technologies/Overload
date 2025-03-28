/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include <GL/glew.h>

#include <OvRendering/HAL/OpenGL/GLTypes.h>
#include <OvRendering/HAL/OpenGL/GLShaderStorageBuffer.h>

template<>
OvRendering::HAL::GLShaderStorageBuffer::TShaderStorageBuffer() : GLBuffer(Settings::EBufferType::SHADER_STORAGE)
{
}
