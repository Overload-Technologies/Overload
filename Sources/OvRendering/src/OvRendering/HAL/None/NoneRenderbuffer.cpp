/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include <OvDebug/Assertion.h>
#include <OvRendering/HAL/None/NoneRenderbuffer.h>

template<>
OvRendering::HAL::NoneRenderbuffer::TRenderbuffer()
{
}

template<>
OvRendering::HAL::NoneRenderbuffer::~TRenderbuffer()
{
}

template<>
void OvRendering::HAL::NoneRenderbuffer::Bind() const
{
}

template<>
void OvRendering::HAL::NoneRenderbuffer::Unbind() const
{
}

template<>
uint32_t OvRendering::HAL::NoneRenderbuffer::GetID() const
{
	return 0;
}

template<>
void OvRendering::HAL::NoneRenderbuffer::Allocate(uint16_t p_width, uint16_t p_height, Settings::EInternalFormat p_format)
{
	m_context.width = p_width;
	m_context.height = p_height;
	m_context.format = p_format;
	m_context.allocated = true;
}

template<>
bool OvRendering::HAL::NoneRenderbuffer::IsValid() const
{
	return m_context.allocated;
}

template<>
void OvRendering::HAL::NoneRenderbuffer::Resize(uint16_t p_width, uint16_t p_height)
{
	OVASSERT(IsValid(), "Cannot resize a renderbuffer that has not been allocated");
}

template<>
uint16_t OvRendering::HAL::NoneRenderbuffer::GetWidth() const
{
	OVASSERT(IsValid(), "Cannot get the height of a renderbuffer that has not been allocated");
	return m_context.width;
}

template<>
uint16_t OvRendering::HAL::NoneRenderbuffer::GetHeight() const
{
	OVASSERT(IsValid(), "Cannot get the height of a renderbuffer that has not been allocated");
	return m_context.height;
}
