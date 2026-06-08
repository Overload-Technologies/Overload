/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include <algorithm>
#include <cstddef>
#include <limits>
#include <string>
#include <string_view>
#include <vector>

#include <OvCore/ECS/Components/UI/TextLayoutEngine.h>

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
		OvCore::ECS::Components::UI::TextLayoutEngine::EHorizontalAlignment p_alignment
	)
	{
		using EHorizontalAlignment = OvCore::ECS::Components::UI::TextLayoutEngine::EHorizontalAlignment;

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
		OvCore::ECS::Components::UI::TextLayoutEngine::EVerticalAlignment p_alignment
	)
	{
		using EVerticalAlignment = OvCore::ECS::Components::UI::TextLayoutEngine::EVerticalAlignment;

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
}

OvCore::ECS::Components::UI::TextLayoutEngine::Output OvCore::ECS::Components::UI::TextLayoutEngine::Layout(const Input& p_input)
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

	struct LineInfo
	{
		size_t firstGlyph = 0;
		size_t lastGlyph = 0;
		float minX = std::numeric_limits<float>::max();
		float maxX = std::numeric_limits<float>::lowest();
		bool hasGeometry = false;
	};

	std::vector<LineInfo> lines;
	lines.push_back({});
	lines.back().firstGlyph = 0;

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

	output.glyphs.reserve(wrappedText.size());

	for (const char character : wrappedText)
	{
		if (character == '\r')
		{
			continue;
		}

		if (character == '\n')
		{
			lines.back().lastGlyph = output.glyphs.size();
			lines.push_back({});
			lines.back().firstGlyph = output.glyphs.size();
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

		output.glyphs.push_back({
			.left = x0,
			.right = x1,
			.bottom = bottomY,
			.top = topY,
			.uMin = glyph->uMin,
			.uMax = glyph->uMax,
			.vMin = glyph->vMin,
			.vMax = glyph->vMax
		});

		minX = std::min(minX, x0);
		minY = std::min(minY, bottomY);
		maxX = std::max(maxX, x1);
		maxY = std::max(maxY, topY);

		auto& line = lines.back();
		line.hasGeometry = true;
		line.minX = std::min(line.minX, x0);
		line.maxX = std::max(line.maxX, x1);
		line.lastGlyph = output.glyphs.size();

		cursorX += glyph->xAdvance * scale;
	}

	lines.back().lastGlyph = output.glyphs.size();

	if (output.glyphs.empty())
	{
		return output;
	}

	output.contentSize = {
		std::max(maxX - minX, 0.0f),
		std::max(maxY - minY, 0.0f)
	};
	output.size = ResolveTextSize(output.contentSize, p_input.uiSize);

	for (const auto& line : lines)
	{
		if (!line.hasGeometry || line.lastGlyph <= line.firstGlyph)
		{
			continue;
		}

		const float lineWidth = std::max(line.maxX - line.minX, 0.0f);
		const float lineCenterX = line.minX + lineWidth * 0.5f;
		const float alignedLineCenterX = GetAlignedCenterX(output.size.x, lineWidth, p_input.horizontalAlignment);
		const float lineOffsetX = alignedLineCenterX - lineCenterX;

		for (size_t glyphIndex = line.firstGlyph; glyphIndex < line.lastGlyph; ++glyphIndex)
		{
			output.glyphs[glyphIndex].left += lineOffsetX;
			output.glyphs[glyphIndex].right += lineOffsetX;
		}
	}

	const float contentCenterY = minY + output.contentSize.y * 0.5f;
	const float alignedCenterY = GetAlignedCenterY(output.size.y, output.contentSize.y, p_input.verticalAlignment);
	const float globalOffsetY = alignedCenterY - contentCenterY;

	for (auto& glyph : output.glyphs)
	{
		glyph.bottom += globalOffsetY;
		glyph.top += globalOffsetY;
	}

	return output;
}
