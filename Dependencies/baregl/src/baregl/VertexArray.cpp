/**
* @project: baregl
* @author: Adrien Givry
* @licence: MIT
*/

#include <baregl/VertexArray.h>

#include <baregl/debug/Assert.h>
#include <baregl/detail/glad/glad.h>
#include <baregl/detail/Types.h>

namespace
{
	uint32_t GetDataTypeSizeInBytes(baregl::types::EDataType p_type)
	{
		switch (p_type)
		{
		case baregl::types::EDataType::BYTE: return sizeof(GLbyte);
		case baregl::types::EDataType::UNSIGNED_BYTE: return sizeof(GLubyte);
		case baregl::types::EDataType::SHORT: return sizeof(GLshort);
		case baregl::types::EDataType::UNSIGNED_SHORT: return sizeof(GLushort);
		case baregl::types::EDataType::INT: return sizeof(GLint);
		case baregl::types::EDataType::UNSIGNED_INT: return sizeof(GLuint);
		case baregl::types::EDataType::FLOAT: return sizeof(GLfloat);
		case baregl::types::EDataType::DOUBLE: return sizeof(GLdouble);
		default: return 0;
		}
	}

	uint32_t GetAttributeSizeInBytes(const baregl::data::VertexAttribute& p_attribute)
	{
		return std::visit([](const auto& attr) -> uint32_t {
			using T = std::decay_t<decltype(attr)>;
			if constexpr (std::is_same_v<T, baregl::data::DoubleVertexAttribute>)
			{
				return sizeof(GLdouble) * attr.count;
			}
			else
			{
				return GetDataTypeSizeInBytes(attr.type) * attr.count;
			}
		}, p_attribute);
	}

	uint32_t CalculateTotalVertexSize(baregl::data::VertexAttributeLayout p_attributes)
	{
		uint32_t result = 0;

		for (const auto& attribute : p_attributes)
		{
			result += GetAttributeSizeInBytes(attribute);
		}

		return result;
	}

	struct VertexAttribSetter
	{
		GLuint index;
		GLsizei stride;
		const GLvoid* offset;

		void operator()(const baregl::data::FloatVertexAttribute& attr) const
		{
			BAREGL_ASSERT(attr.count >= 1 && attr.count <= 4, "Attribute count must be between 1 and 4");
			glVertexAttribPointer(
				index,
				static_cast<GLint>(attr.count),
				baregl::utils::EnumToValue<GLenum>(attr.type),
				static_cast<GLboolean>(attr.normalized),
				stride,
				offset
			);
		}

		void operator()(const baregl::data::IntegerVertexAttribute& attr) const
		{
			BAREGL_ASSERT(attr.count >= 1 && attr.count <= 4, "Attribute count must be between 1 and 4");
			BAREGL_ASSERT(
				attr.type == baregl::types::EDataType::BYTE ||
				attr.type == baregl::types::EDataType::UNSIGNED_BYTE ||
				attr.type == baregl::types::EDataType::SHORT ||
				attr.type == baregl::types::EDataType::UNSIGNED_SHORT ||
				attr.type == baregl::types::EDataType::INT ||
				attr.type == baregl::types::EDataType::UNSIGNED_INT,
				"glVertexAttribIPointer requires an integer data type"
			);
			glVertexAttribIPointer(
				index,
				static_cast<GLint>(attr.count),
				baregl::utils::EnumToValue<GLenum>(attr.type),
				stride,
				offset
			);
		}

		void operator()(const baregl::data::DoubleVertexAttribute& attr) const
		{
			BAREGL_ASSERT(attr.count >= 1 && attr.count <= 4, "Attribute count must be between 1 and 4");
			glVertexAttribLPointer(
				index,
				static_cast<GLint>(attr.count),
				GL_DOUBLE,
				stride,
				offset
			);
		}
	};
}

namespace baregl
{
	VertexArray::VertexArray()
	{
		glCreateVertexArrays(1, &m_id);
	}

	VertexArray::~VertexArray()
	{
		glDeleteVertexArrays(1, &m_id);
	}

	bool VertexArray::IsValid() const
	{
		return m_attributeCount > 0;
	}

	void VertexArray::SetLayout(
		data::VertexAttributeLayout p_attributes,
		Buffer& p_vertexBuffer,
		Buffer& p_indexBuffer
	)
	{
		BAREGL_ASSERT(!IsValid(), "Vertex array layout already set");

		Bind();
		p_indexBuffer.Bind(types::EBufferType::INDEX);
		p_vertexBuffer.Bind(types::EBufferType::VERTEX);

		uint32_t attributeIndex = 0;

		const GLsizei totalSize = static_cast<GLsizei>(CalculateTotalVertexSize(p_attributes));
		intptr_t currentOffset = 0;

		for (const auto& attribute : p_attributes)
		{
			glEnableVertexAttribArray(attributeIndex);

			std::visit(VertexAttribSetter{
				static_cast<GLuint>(attributeIndex),
				totalSize,
				reinterpret_cast<const GLvoid*>(currentOffset)
			}, attribute);

			currentOffset += GetAttributeSizeInBytes(attribute);
			++attributeIndex;
			++m_attributeCount;
		}

		Unbind();
		p_indexBuffer.Unbind();
		p_vertexBuffer.Unbind();
	}

	void VertexArray::ResetLayout()
	{
		BAREGL_ASSERT(IsValid(), "Vertex array layout not already set");

		Bind();
		for (uint32_t i = 0; i < m_attributeCount; ++i)
		{
			glDisableVertexAttribArray(i);
		}
		m_attributeCount = 0;
		Unbind();
	}

	void VertexArray::Bind() const
	{
		glBindVertexArray(m_id);
	}

	void VertexArray::Unbind() const
	{
		glBindVertexArray(0);
	}
}
