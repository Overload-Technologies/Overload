/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include <gl/glew.h>

#include <OvDebug/Assertion.h>
#include <OvRendering/HAL/OpenGL/GLVertexArray.h>
#include <OvRendering/HAL/OpenGL/GLTypes.h>

namespace
{
	uint32_t GetDataTypeSizeInBytes(OvRendering::Settings::EDataType p_type)
	{
		switch (p_type)
		{
		case OvRendering::Settings::EDataType::BYTE: return sizeof(GLbyte);
		case OvRendering::Settings::EDataType::UNSIGNED_BYTE: return sizeof(GLubyte);
		case OvRendering::Settings::EDataType::SHORT: return sizeof(GLshort);
		case OvRendering::Settings::EDataType::UNSIGNED_SHORT: return sizeof(GLushort);
		case OvRendering::Settings::EDataType::INT: return sizeof(GLint);
		case OvRendering::Settings::EDataType::UNSIGNED_INT: return sizeof(GLuint);
		case OvRendering::Settings::EDataType::FLOAT: return sizeof(GLfloat);
		case OvRendering::Settings::EDataType::DOUBLE: return sizeof(GLdouble);
		default: return 0;
		}
	}

	uint32_t CalculateTotalVertexSize(std::span<const OvRendering::Settings::VertexAttribute> p_attributes)
	{
		uint32_t result = 0;

		for (const auto& attribute : p_attributes)
		{
			result += GetDataTypeSizeInBytes(attribute.type) * attribute.count;
		}

		return result;
	}
}

template<>
OvRendering::HAL::GLVertexArray::TVertexArray()
{
	glGenVertexArrays(1, &m_context.id);
}

template<>
OvRendering::HAL::GLVertexArray::~TVertexArray()
{
	glDeleteVertexArrays(1, &m_context.id);
}

template<>
bool OvRendering::HAL::GLVertexArray::IsValid() const
{
	return m_context.attributeCount > 0;
}

template<>
void OvRendering::HAL::GLVertexArray::SetLayout(
	Settings::VertexAttributeLayout p_attributes,
	VertexBuffer& p_vertexBuffer,
	IndexBuffer& p_indexBuffer
)
{
	OVASSERT(!IsValid(), "Vertex array layout already set");

	Bind();
	p_indexBuffer.Bind();
	p_vertexBuffer.Bind();

	uint32_t attributeIndex = 0;

	const uint32_t totalSize = CalculateTotalVertexSize(p_attributes);
	intptr_t currentOffset = 0;

	for (const auto& attribute : p_attributes)
	{
		OVASSERT(attribute.count >= 1 && attribute.count <= 4, "Attribute count must be between 1 and 4");

		glEnableVertexAttribArray(attributeIndex);

		glVertexAttribPointer(
			static_cast<GLuint>(attributeIndex),
			static_cast<GLint>(attribute.count),
			EnumToValue<GLenum>(attribute.type),
			static_cast<GLboolean>(attribute.normalized),
			static_cast<GLsizei>(totalSize),
			reinterpret_cast<const GLvoid*>(currentOffset)
		);

		const uint64_t typeSize = GetDataTypeSizeInBytes(attribute.type);
		const uint64_t attributeSize = typeSize * attribute.count;
		currentOffset += attributeSize;
		++attributeIndex;
		++m_context.attributeCount;
	}

	Unbind();
	p_indexBuffer.Unbind();
	p_vertexBuffer.Unbind();
}

template<>
void OvRendering::HAL::GLVertexArray::ResetLayout()
{
	OVASSERT(IsValid(), "Vertex array layout not already set");

	Bind();
	for (uint32_t i = 0; i < m_context.attributeCount; ++i)
	{
		glDisableVertexAttribArray(i);
	}
	m_context.attributeCount = 0;
	Unbind();
}

template<>
void OvRendering::HAL::GLVertexArray::Bind() const
{
	glBindVertexArray(m_context.id);
}

template<>
void OvRendering::HAL::GLVertexArray::Unbind() const
{
	glBindVertexArray(0);
}

template<>
uint32_t OvRendering::HAL::GLVertexArray::GetID() const
{
	return m_context.id;
}
