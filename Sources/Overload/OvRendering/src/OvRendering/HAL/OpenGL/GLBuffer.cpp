/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include <GL/glew.h>

#include <OvDebug/Assertion.h>
#include <OvRendering/HAL/OpenGL/GLBuffer.h>
#include <OvRendering/HAL/OpenGL/GLTypes.h>

template<>
OvRendering::HAL::GLBuffer::TBuffer(Settings::EBufferType p_type) : m_buffer{
	.type = p_type
}
{
	glGenBuffers(1, &m_buffer.id);
}

template<>
OvRendering::HAL::GLBuffer::~TBuffer()
{
	glDeleteBuffers(1, &m_buffer.id);
}

template<>
void OvRendering::HAL::GLBuffer::Allocate(uint64_t p_size, Settings::EAccessSpecifier p_usage)
{
	glNamedBufferDataEXT(m_buffer.id, p_size, nullptr, EnumToValue<GLenum>(p_usage));
	m_buffer.allocatedBytes = p_size;
}

template<>
void OvRendering::HAL::GLBuffer::Upload(const void* p_data, std::optional<BufferMemoryRange> p_range)
{
	OVASSERT(IsValid(), "Trying to upload data to an invalid buffer");

	glNamedBufferSubData(
		m_buffer.id,
		p_range ? p_range->offset : 0,
		p_range ? p_range->size : m_buffer.allocatedBytes,
		p_data
	);
}

template<>
void OvRendering::HAL::GLBuffer::Bind(std::optional<uint32_t> p_index) const
{
	OVASSERT(IsValid(), "Cannot bind an invalid buffer");

	if (p_index.has_value())
	{
		glBindBufferBase(EnumToValue<GLenum>(m_buffer.type), p_index.value(), m_buffer.id);
	}
	else
	{
		glBindBuffer(EnumToValue<GLenum>(m_buffer.type), m_buffer.id);
	}
}

template<>
void OvRendering::HAL::GLBuffer::Unbind() const
{
	OVASSERT(IsValid(), "Cannot unbind an invalid buffer");

	glBindBuffer(EnumToValue<GLenum>(m_buffer.type), 0);
}

template<>
bool OvRendering::HAL::GLBuffer::IsValid() const
{
	return
		m_buffer.id != 0 &&
		m_buffer.type != Settings::EBufferType::UNKNOWN &&
		m_buffer.allocatedBytes > 0;
}

template<>
uint32_t OvRendering::HAL::GLBuffer::GetID() const
{
	return m_buffer.id;
}
