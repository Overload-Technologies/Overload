/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include <glad.h>

#include <OvRendering/HAL/OpenGL/GLTextureHandle.h>
#include <OvRendering/HAL/OpenGL/GLTypes.h>

template<>
OvRendering::HAL::GLTextureHandle::TTextureHandle(Settings::ETextureType p_type) : m_context{ 
	.type = EnumToValue<GLenum>(p_type)
}
{
}

template<>
OvRendering::HAL::GLTextureHandle::TTextureHandle(Settings::ETextureType p_type, uint32_t p_id) : m_context{
	.id = p_id,
	.type = EnumToValue<GLenum>(p_type)
}
{
}

template<>
void OvRendering::HAL::GLTextureHandle::Bind(std::optional<uint32_t> p_slot) const
{
	// TODO: Use glBindTextureUnit
	if (p_slot.has_value())
	{
		glActiveTexture(GL_TEXTURE0 + p_slot.value());
	}

	glBindTexture(m_context.type, m_context.id);
}

template<>
void OvRendering::HAL::GLTextureHandle::Unbind() const
{
	glBindTexture(m_context.type, 0);
}

template<>
uint32_t OvRendering::HAL::GLTextureHandle::GetID() const
{
	return m_context.id;
}

template<>
OvRendering::Settings::ETextureType OvRendering::HAL::GLTextureHandle::GetType() const
{
	return ValueToEnum<Settings::ETextureType>(m_context.type);
}
