/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include <GL/glew.h>

#include <OvDebug/Assertion.h>
#include <OvRendering/HAL/OpenGL/GLRenderbuffer.h>
#include <OvRendering/HAL/OpenGL/GLTypes.h>

template<>
OvRendering::HAL::GLRenderbuffer::TRenderbuffer()
{
	glGenRenderbuffers(1, &m_context.id);
}

template<>
OvRendering::HAL::GLRenderbuffer::~TRenderbuffer()
{
	glDeleteBuffers(1, &m_context.id);
}

template<>
void OvRendering::HAL::GLRenderbuffer::Bind() const
{
	glBindRenderbuffer(GL_RENDERBUFFER, m_context.id);
}

template<>
void OvRendering::HAL::GLRenderbuffer::Unbind() const
{
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
}

template<>
uint32_t OvRendering::HAL::GLRenderbuffer::GetID() const
{
	return m_context.id;
}

template<>
void OvRendering::HAL::GLRenderbuffer::Allocate(uint16_t p_width, uint16_t p_height, Settings::EInternalFormat p_format)
{
	m_context.width = p_width;
	m_context.height = p_height;
	m_context.format = p_format;

	glNamedRenderbufferStorageEXT(m_context.id, EnumToValue<GLenum>(m_context.format), m_context.width, m_context.height);

	m_context.allocated = true;
}

template<>
bool OvRendering::HAL::GLRenderbuffer::IsValid() const
{
	return m_context.allocated;
}

template<>
void OvRendering::HAL::GLRenderbuffer::Resize(uint16_t p_width, uint16_t p_height)
{
	OVASSERT(IsValid(), "Cannot resize a renderbuffer that has not been allocated");
	Allocate(p_width, p_height, m_context.format);
}


template<>
uint16_t OvRendering::HAL::GLRenderbuffer::GetWidth() const
{
	OVASSERT(IsValid(), "Cannot get width of an invalid renderbuffer");
	return m_context.width;
}

template<>
uint16_t OvRendering::HAL::GLRenderbuffer::GetHeight() const
{
	OVASSERT(IsValid(), "Cannot get height of an invalid renderbuffer");
	return m_context.height;
}
