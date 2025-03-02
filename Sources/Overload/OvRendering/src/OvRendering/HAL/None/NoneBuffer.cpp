/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include <OvDebug/Assertion.h>
#include <OvRendering/HAL/None/NoneBuffer.h>

template<>
OvRendering::HAL::NoneBuffer::TBuffer(Settings::EBufferType p_type) : m_buffer{
	.type = p_type
}
{
}

template<>
OvRendering::HAL::NoneBuffer::~TBuffer()
{
}

template<>
uint64_t OvRendering::HAL::NoneBuffer::Allocate(size_t p_size, Settings::EAccessSpecifier p_usage)
{
	return m_buffer.allocatedBytes = p_size;
}

template<>
void OvRendering::HAL::NoneBuffer::Upload(const void* p_data, std::optional<BufferMemoryRange> p_range)
{
	OVASSERT(IsValid(), "Trying to upload data to an invalid buffer");
	OVASSERT(!IsEmpty(), "Trying to upload data to an empty buffer");
}

template<>
void OvRendering::HAL::NoneBuffer::Bind(std::optional<uint32_t> p_index) const
{
	OVASSERT(IsValid(), "Cannot bind an invalid buffer");
}

template<>
void OvRendering::HAL::NoneBuffer::Unbind() const
{
	OVASSERT(IsValid(), "Cannot unbind an invalid buffer");
}

template<>
bool OvRendering::HAL::NoneBuffer::IsValid() const
{
	return
		m_buffer.type != Settings::EBufferType::UNKNOWN &&
		m_buffer.allocatedBytes > 0;
}

template<>
bool OvRendering::HAL::NoneBuffer::IsEmpty() const
{
	return GetSize() == 0;
}

template<>
uint64_t OvRendering::HAL::NoneBuffer::GetSize() const
{
	OVASSERT(IsValid(), "Cannot get size of an invalid buffer");
	return m_buffer.allocatedBytes;
}

template<>
uint32_t OvRendering::HAL::NoneBuffer::GetID() const
{
	OVASSERT(IsValid(), "Cannot get ID of an invalid buffer");
	return 0;
}
