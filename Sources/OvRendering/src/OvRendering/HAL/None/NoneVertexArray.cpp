/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include <OvDebug/Assertion.h>
#include <OvRendering/HAL/None/NoneVertexArray.h>

template<>
OvRendering::HAL::NoneVertexArray::TVertexArray()
{
}

template<>
OvRendering::HAL::NoneVertexArray::~TVertexArray()
{
}

template<>
void OvRendering::HAL::NoneVertexArray::Bind() const
{
}

template<>
void OvRendering::HAL::NoneVertexArray::Unbind() const
{
}

template<>
bool OvRendering::HAL::NoneVertexArray::IsValid() const
{
	return m_context.attributeCount > 0;
}

template<>
void OvRendering::HAL::NoneVertexArray::SetLayout(
	Settings::VertexAttributeLayout p_attributes,
	VertexBuffer& p_vertexBuffer,
	IndexBuffer& p_indexBuffer
)
{
	OVASSERT(!IsValid(), "Vertex array layout already set");

	for (const auto& attribute : p_attributes)
	{
		OVASSERT(attribute.count >= 1 && attribute.count <= 4, "Attribute count must be between 1 and 4");
		++m_context.attributeCount;
	}
}

template<>
void OvRendering::HAL::NoneVertexArray::ResetLayout()
{
	OVASSERT(IsValid(), "Vertex array layout not already set");
	m_context.attributeCount = 0;
}

template<>
uint32_t OvRendering::HAL::NoneVertexArray::GetID() const
{
	return 0;
}
