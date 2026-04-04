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
	struct StaticVertex
	{
		float position[3];
		float texCoords[2];
		float normals[3];
		float tangent[3];
		float bitangent[3];
	};

	StaticVertex ToStaticVertex(const OvRendering::Geometry::Vertex& p_vertex)
	{
		StaticVertex result{};

		result.position[0] = p_vertex.position[0];
		result.position[1] = p_vertex.position[1];
		result.position[2] = p_vertex.position[2];

		result.texCoords[0] = p_vertex.texCoords[0];
		result.texCoords[1] = p_vertex.texCoords[1];

		result.normals[0] = p_vertex.normals[0];
		result.normals[1] = p_vertex.normals[1];
		result.normals[2] = p_vertex.normals[2];

		result.tangent[0] = p_vertex.tangent[0];
		result.tangent[1] = p_vertex.tangent[1];
		result.tangent[2] = p_vertex.tangent[2];

		result.bitangent[0] = p_vertex.bitangent[0];
		result.bitangent[1] = p_vertex.bitangent[1];
		result.bitangent[2] = p_vertex.bitangent[2];

		return result;
	}
}

OvRendering::Resources::Mesh::Mesh(
	std::span<const Geometry::Vertex> p_vertices,
	std::span<const uint32_t> p_indices,
	uint32_t p_materialIndex,
	bool p_hasSkinningData
) :
	m_vertexCount(static_cast<uint32_t>(p_vertices.size())),
	m_indicesCount(static_cast<uint32_t>(p_indices.size())),
	m_materialIndex(p_materialIndex),
	m_hasSkinningData(p_hasSkinningData)
{
	Upload(p_vertices, p_indices);
	ComputeBoundingSphere(p_vertices);
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
	const auto staticLayout = std::to_array<Settings::VertexAttribute>({
		{ Settings::EDataType::FLOAT, 3 }, // position
		{ Settings::EDataType::FLOAT, 2 }, // texCoords
		{ Settings::EDataType::FLOAT, 3 }, // normal
		{ Settings::EDataType::FLOAT, 3 }, // tangent
		{ Settings::EDataType::FLOAT, 3 }  // bitangent
	});

	const auto skinnedLayout = std::to_array<Settings::VertexAttribute>({
		{ Settings::EDataType::FLOAT, 3 }, // position
		{ Settings::EDataType::FLOAT, 2 }, // texCoords
		{ Settings::EDataType::FLOAT, 3 }, // normal
		{ Settings::EDataType::FLOAT, 3 }, // tangent
		{ Settings::EDataType::FLOAT, 3 }, // bitangent
		{ Settings::EDataType::FLOAT, 4 }, // bone IDs
		{ Settings::EDataType::FLOAT, 4 }  // bone weights
	});

	std::vector<StaticVertex> staticVertices;
	const void* vertexData = nullptr;
	uint64_t vertexBufferSize = 0;
	Settings::VertexAttributeLayout layout = staticLayout;

	if (m_hasSkinningData)
	{
		vertexData = p_vertices.data();
		vertexBufferSize = p_vertices.size_bytes();
		layout = skinnedLayout;
	}
	else
	{
		staticVertices.reserve(p_vertices.size());

		for (const auto& vertex : p_vertices)
		{
			staticVertices.push_back(ToStaticVertex(vertex));
		}

		vertexData = staticVertices.data();
		vertexBufferSize = static_cast<uint64_t>(staticVertices.size()) * sizeof(StaticVertex);
	}

	if (m_vertexBuffer.Allocate(vertexBufferSize))
	{
		m_vertexBuffer.Upload(vertexData);

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

void OvRendering::Resources::Mesh::ComputeBoundingSphere(std::span<const Geometry::Vertex> p_vertices)
{
	m_boundingSphere.position = OvMaths::FVector3::Zero;
	m_boundingSphere.radius = 0.0f;

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

		m_boundingSphere.position = OvMaths::FVector3{ minX + maxX, minY + maxY, minZ + maxZ } / 2.0f;

		for (const auto& vertex : p_vertices)
		{
			const auto& position = reinterpret_cast<const OvMaths::FVector3&>(vertex.position);
			m_boundingSphere.radius = std::max(m_boundingSphere.radius, OvMaths::FVector3::Distance(m_boundingSphere.position, position));
		}
	}
}
