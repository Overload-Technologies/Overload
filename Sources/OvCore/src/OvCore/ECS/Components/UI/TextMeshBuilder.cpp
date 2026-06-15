/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <utility>
#include <vector>

#include <OvCore/ECS/Components/UI/TextMeshBuilder.h>
#include <OvRendering/Geometry/Vertex.h>

namespace
{
	float Interpolate(float p_start, float p_end, float p_ratio)
	{
		return p_start + (p_end - p_start) * p_ratio;
	}

	void ClipTextGeometryToSize(
		std::vector<OvRendering::Geometry::Vertex>& p_vertices,
		std::vector<uint32_t>& p_indices,
		const OvMaths::FVector2& p_size
	)
	{
		if (p_size.x <= 0.0f || p_size.y <= 0.0f)
		{
			p_vertices.clear();
			p_indices.clear();
			return;
		}

		const float rectLeft = -p_size.x * 0.5f;
		const float rectRight = p_size.x * 0.5f;
		const float rectBottom = -p_size.y * 0.5f;
		const float rectTop = p_size.y * 0.5f;

		std::vector<OvRendering::Geometry::Vertex> clippedVertices;
		std::vector<uint32_t> clippedIndices;
		clippedVertices.reserve(p_vertices.size());
		clippedIndices.reserve(p_indices.size());

		for (size_t quadStart = 0; quadStart + 3 < p_vertices.size(); quadStart += 4)
		{
			const auto& bottomLeft = p_vertices[quadStart + 0];
			const auto& bottomRight = p_vertices[quadStart + 1];
			const auto& topRight = p_vertices[quadStart + 2];

			const float left = bottomLeft.position[0];
			const float right = bottomRight.position[0];
			const float bottom = bottomLeft.position[1];
			const float top = topRight.position[1];

			if (
				left >= rectRight ||
				right <= rectLeft ||
				bottom >= rectTop ||
				top <= rectBottom ||
				right <= left ||
				top <= bottom
			)
			{
				continue;
			}

			const float clippedLeft = std::max(left, rectLeft);
			const float clippedRight = std::min(right, rectRight);
			const float clippedBottom = std::max(bottom, rectBottom);
			const float clippedTop = std::min(top, rectTop);

			const float horizontalRatioLeft = (clippedLeft - left) / (right - left);
			const float horizontalRatioRight = (clippedRight - left) / (right - left);
			const float verticalRatioBottom = (clippedBottom - bottom) / (top - bottom);
			const float verticalRatioTop = (clippedTop - bottom) / (top - bottom);

			const float uMin = bottomLeft.texCoords[0];
			const float uMax = bottomRight.texCoords[0];
			const float vBottom = bottomLeft.texCoords[1];
			const float vTop = topRight.texCoords[1];

			const float clippedUMin = Interpolate(uMin, uMax, horizontalRatioLeft);
			const float clippedUMax = Interpolate(uMin, uMax, horizontalRatioRight);
			const float clippedVBottom = Interpolate(vBottom, vTop, verticalRatioBottom);
			const float clippedVTop = Interpolate(vBottom, vTop, verticalRatioTop);

			const uint32_t firstVertex = static_cast<uint32_t>(clippedVertices.size());
			auto clippedBottomLeft = p_vertices[quadStart + 0];
			auto clippedBottomRight = p_vertices[quadStart + 1];
			auto clippedTopRight = p_vertices[quadStart + 2];
			auto clippedTopLeft = p_vertices[quadStart + 3];

			clippedBottomLeft.position[0] = clippedLeft;
			clippedBottomLeft.position[1] = clippedBottom;
			clippedBottomLeft.texCoords[0] = clippedUMin;
			clippedBottomLeft.texCoords[1] = clippedVBottom;

			clippedBottomRight.position[0] = clippedRight;
			clippedBottomRight.position[1] = clippedBottom;
			clippedBottomRight.texCoords[0] = clippedUMax;
			clippedBottomRight.texCoords[1] = clippedVBottom;

			clippedTopRight.position[0] = clippedRight;
			clippedTopRight.position[1] = clippedTop;
			clippedTopRight.texCoords[0] = clippedUMax;
			clippedTopRight.texCoords[1] = clippedVTop;

			clippedTopLeft.position[0] = clippedLeft;
			clippedTopLeft.position[1] = clippedTop;
			clippedTopLeft.texCoords[0] = clippedUMin;
			clippedTopLeft.texCoords[1] = clippedVTop;

			clippedVertices.push_back(clippedBottomLeft);
			clippedVertices.push_back(clippedBottomRight);
			clippedVertices.push_back(clippedTopRight);
			clippedVertices.push_back(clippedTopLeft);

			clippedIndices.push_back(firstVertex + 0);
			clippedIndices.push_back(firstVertex + 1);
			clippedIndices.push_back(firstVertex + 2);
			clippedIndices.push_back(firstVertex + 0);
			clippedIndices.push_back(firstVertex + 2);
			clippedIndices.push_back(firstVertex + 3);
		}

		p_vertices = std::move(clippedVertices);
		p_indices = std::move(clippedIndices);
	}
}

OvCore::ECS::Components::UI::TextMeshBuilder::Output OvCore::ECS::Components::UI::TextMeshBuilder::Build(const Input& p_input)
{
	return Build(TextLayoutEngine::Layout(p_input));
}

OvCore::ECS::Components::UI::TextMeshBuilder::Output OvCore::ECS::Components::UI::TextMeshBuilder::Build(const TextLayoutEngine::Output& p_layout)
{
	Output output;
	output.size = p_layout.size;

	if (p_layout.glyphs.empty())
	{
		return output;
	}

	std::vector<OvRendering::Geometry::Vertex> vertices;
	std::vector<uint32_t> indices;
	vertices.reserve(p_layout.glyphs.size() * 4);
	indices.reserve(p_layout.glyphs.size() * 6);

	for (const auto& glyph : p_layout.glyphs)
	{
		const uint32_t firstVertex = static_cast<uint32_t>(vertices.size());

		vertices.push_back({ { glyph.left,  glyph.bottom, 0.0f }, { glyph.uMin, glyph.vMax }, { 0.0f, 0.0f, 1.0f }, {}, {} });
		vertices.push_back({ { glyph.right, glyph.bottom, 0.0f }, { glyph.uMax, glyph.vMax }, { 0.0f, 0.0f, 1.0f }, {}, {} });
		vertices.push_back({ { glyph.right, glyph.top,    0.0f }, { glyph.uMax, glyph.vMin }, { 0.0f, 0.0f, 1.0f }, {}, {} });
		vertices.push_back({ { glyph.left,  glyph.top,    0.0f }, { glyph.uMin, glyph.vMin }, { 0.0f, 0.0f, 1.0f }, {}, {} });

		indices.push_back(firstVertex + 0);
		indices.push_back(firstVertex + 1);
		indices.push_back(firstVertex + 2);
		indices.push_back(firstVertex + 0);
		indices.push_back(firstVertex + 2);
		indices.push_back(firstVertex + 3);
	}

	ClipTextGeometryToSize(vertices, indices, output.size);
	if (vertices.empty() || indices.empty())
	{
		return output;
	}

	output.mesh = std::make_unique<OvRendering::Resources::Mesh>(vertices, indices);
	return output;
}
