/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include <GL/glew.h>

#include <OvRendering/HAL/OpenGL/GLTextureHandle.h>

template<>
OvRendering::HAL::GLTextureHandle::TTextureHandle()
{
}

template<>
OvRendering::HAL::GLTextureHandle::TTextureHandle(uint32_t p_id) : m_context{ .id = p_id }
{
}

template<>
void OvRendering::HAL::GLTextureHandle::Bind(std::optional<uint32_t> p_slot) const
{
	if (p_slot.has_value())
	{
		glActiveTexture(GL_TEXTURE0 + p_slot.value());
	}

	glBindTexture(GL_TEXTURE_2D, m_context.id);
}

template<>
void OvRendering::HAL::GLTextureHandle::Unbind() const
{
	glBindTexture(GL_TEXTURE_2D, 0);
}

template<>
uint32_t OvRendering::HAL::GLTextureHandle::GetID() const
{
	return m_context.id;
}
