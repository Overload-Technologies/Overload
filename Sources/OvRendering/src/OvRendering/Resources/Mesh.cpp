/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include <array>
#include <cmath>
#include <cstdint>
#include <limits>
#include <vector>

#include <OvDebug/Logger.h>
#include <OvRendering/Resources/Mesh.h>

namespace
{
	template<typename TVertex>
	OvRendering::Geometry::BoundingSphere ComputeBoundingSphere(std::span<const TVertex> p_vertices)
	{
		OvRendering::Geometry::BoundingSphere sphere{};
		sphere.position = OvMaths::FVector3::Zero;
		sphere.radius = 0.0f;

		if (!p_vertices.empty())
		{
			float minX = std::numeric_limits<float>::max();
			float minY = std::numeric_limits<float>::max();
			float minZ = std::numeric_limits<float>::max();

			float maxX = std::numeric_limits<float>::lowest();
			float maxY = std::numeric_limits<float>::lowest();
			float maxZ = std::numeric_limits<float>::lowest();

			for (const auto& vertex : p_vertices)
			{
				minX = std::min(minX, vertex.position[0]);
				minY = std::min(minY, vertex.position[1]);
				minZ = std::min(minZ, vertex.position[2]);

				maxX = std::max(maxX, vertex.position[0]);
				maxY = std::max(maxY, vertex.position[1]);
				maxZ = std::max(maxZ, vertex.position[2]);
			}

			sphere.position = OvMaths::FVector3{ minX + maxX, minY + maxY, minZ + maxZ } / 2.0f;

			for (const auto& vertex : p_vertices)
			{
				const auto& position = reinterpret_cast<const OvMaths::FVector3&>(vertex.position);
				sphere.radius = std::max(sphere.radius, OvMaths::FVector3::Distance(sphere.position, position));
			}
		}

		return sphere;
	}
}

OvRendering::Resources::Mesh::Mesh(
	std::span<const Geometry::Vertex> p_vertices,
	std::span<const uint32_t> p_indices,
	uint32_t p_materialIndex
) :
	m_vertexCount(static_cast<uint32_t>(p_vertices.size())),
	m_indicesCount(static_cast<uint32_t>(p_indices.size())),
	m_materialIndex(p_materialIndex),
	m_hasSkinningData(false)
{
	Upload(p_vertices, p_indices);
	m_boundingSphere = ComputeBoundingSphere(p_vertices);
}

OvRendering::Resources::Mesh::Mesh(
	std::span<const Geometry::SkinnedVertex> p_vertices,
	std::span<const uint32_t> p_indices,
	uint32_t p_materialIndex
) :
	m_vertexCount(static_cast<uint32_t>(p_vertices.size())),
	m_indicesCount(static_cast<uint32_t>(p_indices.size())),
	m_materialIndex(p_materialIndex),
	m_hasSkinningData(true)
{
	Upload(p_vertices, p_indices);
	m_boundingSphere = ComputeBoundingSphere(p_vertices);
}

void OvRendering::Resources::Mesh::Bind() const
{
	m_vertexArray.Bind();
}

void OvRendering::Resources::Mesh::Unbind() const
{
	m_vertexArray.Unbind();
}

uint32_t OvRendering::Resources::Mesh::GetVertexCount() const
{
	return m_vertexCount;
}

uint32_t OvRendering::Resources::Mesh::GetIndexCount() const
{
	return m_indicesCount;
}

const OvRendering::Geometry::BoundingSphere& OvRendering::Resources::Mesh::GetBoundingSphere() const
{
	return m_boundingSphere;
}

uint32_t OvRendering::Resources::Mesh::GetMaterialIndex() const
{
	return m_materialIndex;
}

bool OvRendering::Resources::Mesh::HasSkinningData() const
{
	return m_hasSkinningData;
}

void OvRendering::Resources::Mesh::Upload(std::span<const Geometry::Vertex> p_vertices, std::span<const uint32_t> p_indices)
{
	const auto layout = std::to_array<Settings::VertexAttribute>({
		{ Settings::EDataType::FLOAT, 3 }, // position
		{ Settings::EDataType::FLOAT, 2 }, // texCoords
		{ Settings::EDataType::FLOAT, 3 }, // normal
		{ Settings::EDataType::FLOAT, 3 }, // tangent
		{ Settings::EDataType::FLOAT, 3 }  // bitangent
	});

	if (m_vertexBuffer.Allocate(p_vertices.size_bytes()))
	{
		m_vertexBuffer.Upload(p_vertices.data());

		if (m_indexBuffer.Allocate(p_indices.size_bytes()))
		{
			m_indexBuffer.Upload(p_indices.data());
			m_vertexArray.SetLayout(layout, m_vertexBuffer, m_indexBuffer);
		}
		else
		{
			OVLOG_WARNING("Empty index buffer!");
		}
	}
	else
	{
		OVLOG_WARNING("Empty vertex buffer!");
	}
}

void OvRendering::Resources::Mesh::Upload(std::span<const Geometry::SkinnedVertex> p_vertices, std::span<const uint32_t> p_indices)
{
	const auto layout = std::to_array<Settings::VertexAttribute>({
		{ Settings::EDataType::FLOAT,        3 }, // position
		{ Settings::EDataType::FLOAT,        2 }, // texCoords
		{ Settings::EDataType::FLOAT,        3 }, // normal
		{ Settings::EDataType::FLOAT,        3 }, // tangent
		{ Settings::EDataType::FLOAT,        3 }, // bitangent
		{ Settings::EDataType::UNSIGNED_INT, 4 }, // boneIDs
		{ Settings::EDataType::FLOAT,        4 }  // boneWeights
	});

	if (m_vertexBuffer.Allocate(p_vertices.size_bytes()))
	{
		m_vertexBuffer.Upload(p_vertices.data());

		if (m_indexBuffer.Allocate(p_indices.size_bytes()))
		{
			m_indexBuffer.Upload(p_indices.data());
			m_vertexArray.SetLayout(layout, m_vertexBuffer, m_indexBuffer);
		}
		else
		{
			OVLOG_WARNING("Empty index buffer!");
		}
	}
	else
	{
		OVLOG_WARNING("Empty vertex buffer!");
	}
}


