/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include <algorithm>
#include <cstdint>
#include <limits>
#include <memory>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include <OvCore/ECS/Components/UI/TextMeshBuilder.h>
#include <OvRendering/Geometry/Vertex.h>

namespace
{
	OvMaths::FVector2 ResolveTextSize(const OvMaths::FVector2& p_contentSize, const OvMaths::FVector2& p_uiSize)
	{
		return {
			p_uiSize.x > 0.0f ? p_uiSize.x : p_contentSize.x,
			p_uiSize.y > 0.0f ? p_uiSize.y : p_contentSize.y
		};
	}

	bool IsSoftWrapWhitespace(char p_character)
	{
		return p_character == ' ' || p_character == '\t';
	}

	float GetGlyphAdvance(
		const OvRendering::Resources::Font& p_font,
		const OvRendering::Resources::Font::Glyph* p_fallbackGlyph,
		float p_fontSize,
		float p_scale,
		char p_character
	)
	{
		const auto* glyph = p_font.GetGlyph(p_character, p_fontSize);
		if (!glyph)
		{
			glyph = p_fallbackGlyph;
		}

		return glyph ? glyph->xAdvance * p_scale : 0.0f;
	}

	float MeasureAdvance(
		const OvRendering::Resources::Font& p_font,
		const OvRendering::Resources::Font::Glyph* p_fallbackGlyph,
		float p_fontSize,
		float p_scale,
		std::string_view p_text,
		size_t p_begin,
		size_t p_end
	)
	{
		float width = 0.0f;
		for (size_t index = p_begin; index < p_end; ++index)
		{
			width += GetGlyphAdvance(p_font, p_fallbackGlyph, p_fontSize, p_scale, p_text[index]);
		}

		return width;
	}

	void AppendWrappedRun(
		std::string& p_output,
		const OvRendering::Resources::Font& p_font,
		const OvRendering::Resources::Font::Glyph* p_fallbackGlyph,
		float p_fontSize,
		float p_scale,
		float p_maxWidth,
		std::string_view p_text,
		size_t p_begin,
		size_t p_end,
		float p_width,
		float& p_lineWidth,
		bool& p_lineHasContent
	)
	{
		if (p_width <= p_maxWidth)
		{
			p_output.append(p_text.data() + p_begin, p_end - p_begin);
			p_lineWidth += p_width;
			p_lineHasContent = p_end > p_begin;
			return;
		}

		for (size_t index = p_begin; index < p_end; ++index)
		{
			const float characterWidth = GetGlyphAdvance(p_font, p_fallbackGlyph, p_fontSize, p_scale, p_text[index]);
			if (p_lineHasContent && p_lineWidth + characterWidth > p_maxWidth)
			{
				p_output += '\n';
				p_lineWidth = 0.0f;
				p_lineHasContent = false;
			}

			p_output += p_text[index];
			p_lineWidth += characterWidth;
			p_lineHasContent = true;
		}
	}

	std::string WrapTextToWidth(
		std::string_view p_text,
		const OvRendering::Resources::Font& p_font,
		const OvRendering::Resources::Font::Glyph* p_fallbackGlyph,
		float p_fontSize,
		float p_scale,
		float p_maxWidth
	)
	{
		if (p_maxWidth <= 0.0f)
		{
			return std::string{ p_text };
		}

		std::string output;
		output.reserve(p_text.size());

		float lineWidth = 0.0f;
		float pendingWhitespaceWidth = 0.0f;
		std::string pendingWhitespace;
		bool lineHasContent = false;

		for (size_t index = 0; index < p_text.size();)
		{
			const char character = p_text[index];
			if (character == '\r')
			{
				++index;
				continue;
			}

			if (character == '\n')
			{
				output += '\n';
				lineWidth = 0.0f;
				pendingWhitespaceWidth = 0.0f;
				pendingWhitespace.clear();
				lineHasContent = false;
				++index;
				continue;
			}

			if (IsSoftWrapWhitespace(character))
			{
				const size_t whitespaceBegin = index;
				while (index < p_text.size() && IsSoftWrapWhitespace(p_text[index]))
				{
					++index;
				}

				if (lineHasContent)
				{
					pendingWhitespace.append(p_text.data() + whitespaceBegin, index - whitespaceBegin);
					pendingWhitespaceWidth += MeasureAdvance(
						p_font,
						p_fallbackGlyph,
						p_fontSize,
						p_scale,
						p_text,
						whitespaceBegin,
						index
					);
				}
				continue;
			}

			const size_t wordBegin = index;
			while (
				index < p_text.size() &&
				p_text[index] != '\r' &&
				p_text[index] != '\n' &&
				!IsSoftWrapWhitespace(p_text[index])
			)
			{
				++index;
			}

			const float wordWidth = MeasureAdvance(
				p_font,
				p_fallbackGlyph,
				p_fontSize,
				p_scale,
				p_text,
				wordBegin,
				index
			);

			if (lineHasContent && lineWidth + pendingWhitespaceWidth + wordWidth > p_maxWidth)
			{
				output += '\n';
				lineWidth = 0.0f;
				lineHasContent = false;
			}
			else if (lineHasContent && !pendingWhitespace.empty())
			{
				output += pendingWhitespace;
				lineWidth += pendingWhitespaceWidth;
			}

			pendingWhitespace.clear();
			pendingWhitespaceWidth = 0.0f;

			AppendWrappedRun(
				output,
				p_font,
				p_fallbackGlyph,
				p_fontSize,
				p_scale,
				p_maxWidth,
				p_text,
				wordBegin,
				index,
				wordWidth,
				lineWidth,
				lineHasContent
			);
		}

		return output;
	}

	float GetAlignedCenterX(
		float p_textWidth,
		float p_contentWidth,
		OvCore::ECS::Components::UI::TextMeshBuilder::EHorizontalAlignment p_alignment
	)
	{
		using EHorizontalAlignment = OvCore::ECS::Components::UI::TextMeshBuilder::EHorizontalAlignment;

		switch (p_alignment)
		{
		case EHorizontalAlignment::CENTER:
			return 0.0f;
		case EHorizontalAlignment::RIGHT:
			return p_textWidth * 0.5f - p_contentWidth * 0.5f;
		case EHorizontalAlignment::LEFT:
		default:
			return -p_textWidth * 0.5f + p_contentWidth * 0.5f;
		}
	}

	float GetAlignedCenterY(
		float p_textHeight,
		float p_contentHeight,
		OvCore::ECS::Components::UI::TextMeshBuilder::EVerticalAlignment p_alignment
	)
	{
		using EVerticalAlignment = OvCore::ECS::Components::UI::TextMeshBuilder::EVerticalAlignment;

		switch (p_alignment)
		{
		case EVerticalAlignment::CENTER:
			return 0.0f;
		case EVerticalAlignment::BOTTOM:
			return -p_textHeight * 0.5f + p_contentHeight * 0.5f;
		case EVerticalAlignment::TOP:
		default:
			return p_textHeight * 0.5f - p_contentHeight * 0.5f;
		}
	}

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
	Output output;
	output.size = ResolveTextSize(OvMaths::FVector2::Zero, p_input.uiSize);

	if (!p_input.font || p_input.text.empty() || !p_input.font->EnsurePixelSize(p_input.fontSize))
	{
		return output;
	}

	const float bakedPixelSize = p_input.font->GetPixelSize(p_input.fontSize);
	if (bakedPixelSize <= 0.0f)
	{
		return output;
	}

	const float scale = p_input.fontSize / bakedPixelSize;
	const float lineHeight = p_input.font->GetLineHeight(p_input.fontSize);
	std::vector<OvRendering::Geometry::Vertex> vertices;
	std::vector<uint32_t> indices;
	vertices.reserve(p_input.text.size() * 4);
	indices.reserve(p_input.text.size() * 6);

	struct LineInfo
	{
		size_t firstVertex = 0;
		size_t lastVertex = 0;
		float minX = std::numeric_limits<float>::max();
		float maxX = std::numeric_limits<float>::lowest();
		bool hasGeometry = false;
	};

	std::vector<LineInfo> lines;
	lines.push_back({});
	lines.back().firstVertex = 0;

	float cursorX = 0.0f;
	float baselineY = 0.0f;
	float minX = std::numeric_limits<float>::max();
	float minY = std::numeric_limits<float>::max();
	float maxX = std::numeric_limits<float>::lowest();
	float maxY = std::numeric_limits<float>::lowest();

	const auto* fallbackGlyph = p_input.font->GetGlyph('?', p_input.fontSize);
	const auto wrappedText = WrapTextToWidth(
		p_input.text,
		*p_input.font,
		fallbackGlyph,
		p_input.fontSize,
		scale,
		p_input.uiSize.x
	);

	for (const char character : wrappedText)
	{
		if (character == '\r')
		{
			continue;
		}

		if (character == '\n')
		{
			lines.back().lastVertex = vertices.size();
			lines.push_back({});
			lines.back().firstVertex = vertices.size();
			cursorX = 0.0f;
			baselineY -= lineHeight * scale;
			continue;
		}

		const auto* glyph = p_input.font->GetGlyph(character, p_input.fontSize);
		if (!glyph)
		{
			glyph = fallbackGlyph;
		}

		if (!glyph)
		{
			continue;
		}

		const float x0 = cursorX + glyph->xOffset * scale;
		const float topY = baselineY - glyph->yOffset * scale;
		const float x1 = x0 + glyph->width * scale;
		const float bottomY = topY - glyph->height * scale;

		const uint32_t firstVertex = static_cast<uint32_t>(vertices.size());
		vertices.push_back({ { x0, bottomY, 0.0f }, { glyph->uMin, glyph->vMax }, { 0.0f, 0.0f, 1.0f }, {}, {} });
		vertices.push_back({ { x1, bottomY, 0.0f }, { glyph->uMax, glyph->vMax }, { 0.0f, 0.0f, 1.0f }, {}, {} });
		vertices.push_back({ { x1, topY,    0.0f }, { glyph->uMax, glyph->vMin }, { 0.0f, 0.0f, 1.0f }, {}, {} });
		vertices.push_back({ { x0, topY,    0.0f }, { glyph->uMin, glyph->vMin }, { 0.0f, 0.0f, 1.0f }, {}, {} });

		indices.push_back(firstVertex + 0);
		indices.push_back(firstVertex + 1);
		indices.push_back(firstVertex + 2);
		indices.push_back(firstVertex + 0);
		indices.push_back(firstVertex + 2);
		indices.push_back(firstVertex + 3);

		minX = std::min(minX, x0);
		minY = std::min(minY, bottomY);
		maxX = std::max(maxX, x1);
		maxY = std::max(maxY, topY);

		auto& line = lines.back();
		line.hasGeometry = true;
		line.minX = std::min(line.minX, x0);
		line.maxX = std::max(line.maxX, x1);
		line.lastVertex = vertices.size();

		cursorX += glyph->xAdvance * scale;
	}

	lines.back().lastVertex = vertices.size();

	if (vertices.empty() || indices.empty())
	{
		return output;
	}

	const OvMaths::FVector2 contentSize = {
		std::max(maxX - minX, 0.0f),
		std::max(maxY - minY, 0.0f)
	};

	output.size = ResolveTextSize(contentSize, p_input.uiSize);
	if (!p_input.buildMesh)
	{
		return output;
	}

	for (const auto& line : lines)
	{
		if (!line.hasGeometry || line.lastVertex <= line.firstVertex)
		{
			continue;
		}

		const float lineWidth = std::max(line.maxX - line.minX, 0.0f);
		const float lineCenterX = line.minX + lineWidth * 0.5f;
		const float alignedLineCenterX = GetAlignedCenterX(output.size.x, lineWidth, p_input.horizontalAlignment);
		const float lineOffsetX = alignedLineCenterX - lineCenterX;

		for (size_t vertexIndex = line.firstVertex; vertexIndex < line.lastVertex; ++vertexIndex)
		{
			vertices[vertexIndex].position[0] += lineOffsetX;
		}
	}

	const float contentCenterY = minY + contentSize.y * 0.5f;
	const float alignedCenterY = GetAlignedCenterY(output.size.y, contentSize.y, p_input.verticalAlignment);
	const float globalOffsetY = alignedCenterY - contentCenterY;

	for (auto& vertex : vertices)
	{
		vertex.position[1] += globalOffsetY;
	}

	ClipTextGeometryToSize(vertices, indices, output.size);
	if (vertices.empty() || indices.empty())
	{
		return output;
	}

	output.mesh = std::make_unique<OvRendering::Resources::Mesh>(vertices, indices);
	return output;
}
