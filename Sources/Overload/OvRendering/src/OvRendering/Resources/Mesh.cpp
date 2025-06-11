/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include <array>
#include <intrin.h>

#include <OvDebug/Logger.h>
#include <OvRendering/Resources/Mesh.h>

OvRendering::Resources::Mesh::Mesh(
	std::span<const Geometry::Vertex> p_vertices,
	std::span<const uint32_t> p_indices,
	uint32_t p_materialIndex
) :
	m_vertexCount(static_cast<uint32_t>(p_vertices.size())),
	m_indicesCount(static_cast<uint32_t>(p_indices.size())),
	m_materialIndex(p_materialIndex)
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

void OvRendering::Resources::Mesh::Upload(std::span<const Geometry::Vertex> p_vertices, std::span<const uint32_t> p_indices)
{
	if (m_vertexBuffer.Allocate(p_vertices.size_bytes()))
	{
		m_vertexBuffer.Upload(p_vertices.data());

		if (m_indexBuffer.Allocate(p_indices.size_bytes()))
		{
			m_indexBuffer.Upload(p_indices.data());

			m_vertexArray.SetLayout(std::to_array<Settings::VertexAttribute>({
				{ Settings::EDataType::FLOAT, 3 }, // position
				{ Settings::EDataType::FLOAT, 2 }, // texCoords
				{ Settings::EDataType::FLOAT, 3 }, // normal
				{ Settings::EDataType::FLOAT, 3 }, // tangent
				{ Settings::EDataType::FLOAT, 3 }  // bitangent
			}), m_vertexBuffer, m_indexBuffer);
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
	const size_t vertexCount = p_vertices.size();

	if (vertexCount == 0)
	{
		m_boundingSphere =  {
			.position = OvMaths::FVector3::Zero,
			.radius = 0.0f
		};

		return;
	}

	// Initialize SIMD registers for min/max with first vertex values
	__m128 vMinXYZ = _mm_setr_ps(p_vertices[0].position[0], p_vertices[0].position[1], p_vertices[0].position[2], FLT_MAX);
	__m128 vMaxXYZ = _mm_setr_ps(p_vertices[0].position[0], p_vertices[0].position[1], p_vertices[0].position[2], -FLT_MAX);

	// Process all vertices in one loop to find min/max
	for (size_t i = 1; i < vertexCount; ++i)
	{
		// Load vertex position directly - assumes position is aligned properly
		const float* posPtr = p_vertices[i].position;
		__m128 vPos = _mm_loadu_ps(posPtr); // Using loadu in case it's not 16-byte aligned

		// Update min and max in one pass
		vMinXYZ = _mm_min_ps(vMinXYZ, vPos);
		vMaxXYZ = _mm_max_ps(vMaxXYZ, vPos);
	}

	// Calculate center = (min + max) * 0.5
	__m128 vCenter = _mm_mul_ps(_mm_add_ps(vMinXYZ, vMaxXYZ), _mm_set1_ps(0.5f));

	// Store center position
	float centerArr[4];
	_mm_store_ps(centerArr, vCenter);
	auto center = OvMaths::FVector3{ centerArr[0], centerArr[1], centerArr[2] };

	// Calculate radius - use dot product for distance calculation
	__m128 vMaxDistSq = _mm_setzero_ps();

	// Pre-load center vector once outside the loop
	const __m128 vCenterXYZ = _mm_setr_ps(
		center.x,
		center.y,
		center.z,
		0.0f
	);

	// Unroll the loop by 4 for better throughput
	size_t i = 0;
	const size_t unrollCount = vertexCount & ~3ull; // Round down to multiple of 4

	for (; i < unrollCount; i += 4)
	{
		// Load 4 vertices at once
		const float* pos0 = p_vertices[i].position;
		const float* pos1 = p_vertices[i + 1].position;
		const float* pos2 = p_vertices[i + 2].position;
		const float* pos3 = p_vertices[i + 3].position;

		__m128 vPos0 = _mm_loadu_ps(pos0);
		__m128 vDiff0 = _mm_sub_ps(vPos0, vCenterXYZ);
		__m128 vDistSq0 = _mm_dp_ps(vDiff0, vDiff0, 0x77); // Dot product with mask 0x77 (sum xyz, store in all)
		vMaxDistSq = _mm_max_ps(vMaxDistSq, vDistSq0);

		__m128 vPos1 = _mm_loadu_ps(pos1);
		__m128 vDiff1 = _mm_sub_ps(vPos1, vCenterXYZ);
		__m128 vDistSq1 = _mm_dp_ps(vDiff1, vDiff1, 0x77);
		vMaxDistSq = _mm_max_ps(vMaxDistSq, vDistSq1);

		__m128 vPos2 = _mm_loadu_ps(pos2);
		__m128 vDiff2 = _mm_sub_ps(vPos2, vCenterXYZ);
		__m128 vDistSq2 = _mm_dp_ps(vDiff2, vDiff2, 0x77);
		vMaxDistSq = _mm_max_ps(vMaxDistSq, vDistSq2);

		__m128 vPos3 = _mm_loadu_ps(pos3);
		__m128 vDiff3 = _mm_sub_ps(vPos3, vCenterXYZ);
		__m128 vDistSq3 = _mm_dp_ps(vDiff3, vDiff3, 0x77);
		vMaxDistSq = _mm_max_ps(vMaxDistSq, vDistSq3);
	}

	// Handle remaining vertices
	for (; i < vertexCount; ++i)
	{
		const float* pos = p_vertices[i].position;
		__m128 vPos = _mm_loadu_ps(pos);
		__m128 vDiff = _mm_sub_ps(vPos, vCenterXYZ);
		__m128 vDistSq = _mm_dp_ps(vDiff, vDiff, 0x77);
		vMaxDistSq = _mm_max_ps(vMaxDistSq, vDistSq);
	}

	// Extract radius (sqrt of max squared distance)
	float maxDistSq;
	_mm_store_ss(&maxDistSq, vMaxDistSq);

	m_boundingSphere = {
		.position = center,
		.radius = std::sqrt(maxDistSq)
	};
}
