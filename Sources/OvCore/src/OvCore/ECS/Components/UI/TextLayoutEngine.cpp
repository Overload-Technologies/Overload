/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <string_view>
#include <vector>

#include <OvCore/ECS/Components/UI/TextLayoutEngine.h>

namespace
{
	using CodePoint = uint32_t;

	constexpr CodePoint kReplacementCodePoint = 0xFFFD;

	OvMaths::FVector2 ResolveTextSize(const OvMaths::FVector2& p_contentSize, const OvMaths::FVector2& p_uiSize)
	{
		return {
			p_uiSize.x > 0.0f ? p_uiSize.x : p_contentSize.x,
			p_uiSize.y > 0.0f ? p_uiSize.y : p_contentSize.y
		};
	}

	std::vector<CodePoint> DecodeUTF8(std::string_view p_text)
	{
		std::vector<CodePoint> result;
		result.reserve(p_text.size());

		for (size_t index = 0; index < p_text.size();)
		{
			const auto firstByte = static_cast<uint8_t>(p_text[index]);
			if (firstByte <= 0x7F)
			{
				result.push_back(firstByte);
				++index;
				continue;
			}

			size_t sequenceLength = 0;
			CodePoint codePoint = 0;
			CodePoint minimumCodePoint = 0;
			if (firstByte >= 0xC2 && firstByte <= 0xDF)
			{
				sequenceLength = 2;
				codePoint = firstByte & 0x1F;
				minimumCodePoint = 0x80;
			}
			else if (firstByte >= 0xE0 && firstByte <= 0xEF)
			{
				sequenceLength = 3;
				codePoint = firstByte & 0x0F;
				minimumCodePoint = 0x800;
			}
			else if (firstByte >= 0xF0 && firstByte <= 0xF4)
			{
				sequenceLength = 4;
				codePoint = firstByte & 0x07;
				minimumCodePoint = 0x10000;
			}

			bool valid = sequenceLength > 0 && index + sequenceLength <= p_text.size();
			for (size_t offset = 1; valid && offset < sequenceLength; ++offset)
			{
				const auto continuationByte = static_cast<uint8_t>(p_text[index + offset]);
				valid = (continuationByte & 0xC0) == 0x80;
				codePoint = (codePoint << 6) | (continuationByte & 0x3F);
			}

			valid =
				valid &&
				codePoint >= minimumCodePoint &&
				codePoint <= 0x10FFFF &&
				(codePoint < 0xD800 || codePoint > 0xDFFF);

			if (!valid)
			{
				result.push_back(kReplacementCodePoint);
				++index;
				continue;
			}

			result.push_back(codePoint);
			index += sequenceLength;
		}

		return result;
	}

	bool IsSoftWrapWhitespace(CodePoint p_codePoint)
	{
		return p_codePoint == ' ' || p_codePoint == '\t';
	}

	float GetGlyphAdvance(
		const OvRendering::Resources::Font& p_font,
		const OvRendering::Resources::Font::Glyph* p_fallbackGlyph,
		float p_fontSize,
		float p_scale,
		CodePoint p_codePoint
	)
	{
		const auto* glyph = p_font.GetGlyph(p_codePoint, p_fontSize);
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
		const std::vector<CodePoint>& p_text,
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
		std::vector<CodePoint>& p_output,
		const OvRendering::Resources::Font& p_font,
		const OvRendering::Resources::Font::Glyph* p_fallbackGlyph,
		float p_fontSize,
		float p_scale,
		float p_maxWidth,
		const std::vector<CodePoint>& p_text,
		size_t p_begin,
		size_t p_end,
		float p_width,
		float& p_lineWidth,
		bool& p_lineHasContent
	)
	{
		if (p_width <= p_maxWidth)
		{
			p_output.insert(p_output.end(), p_text.begin() + p_begin, p_text.begin() + p_end);
			p_lineWidth += p_width;
			p_lineHasContent = p_end > p_begin;
			return;
		}

		for (size_t index = p_begin; index < p_end; ++index)
		{
			const float characterWidth = GetGlyphAdvance(p_font, p_fallbackGlyph, p_fontSize, p_scale, p_text[index]);
			if (p_lineHasContent && p_lineWidth + characterWidth > p_maxWidth)
			{
				p_output.push_back('\n');
				p_lineWidth = 0.0f;
				p_lineHasContent = false;
			}

			p_output.push_back(p_text[index]);
			p_lineWidth += characterWidth;
			p_lineHasContent = true;
		}
	}

	std::vector<CodePoint> WrapTextToWidth(
		const std::vector<CodePoint>& p_text,
		const OvRendering::Resources::Font& p_font,
		const OvRendering::Resources::Font::Glyph* p_fallbackGlyph,
		float p_fontSize,
		float p_scale,
		float p_maxWidth
	)
	{
		if (p_maxWidth <= 0.0f)
		{
			return p_text;
		}

		std::vector<CodePoint> output;
		output.reserve(p_text.size());

		float lineWidth = 0.0f;
		float pendingWhitespaceWidth = 0.0f;
		std::vector<CodePoint> pendingWhitespace;
		bool lineHasContent = false;

		for (size_t index = 0; index < p_text.size();)
		{
			const auto codePoint = p_text[index];
			if (codePoint == '\r')
			{
				++index;
				continue;
			}

			if (codePoint == '\n')
			{
				output.push_back('\n');
				lineWidth = 0.0f;
				pendingWhitespaceWidth = 0.0f;
				pendingWhitespace.clear();
				lineHasContent = false;
				++index;
				continue;
			}

			if (IsSoftWrapWhitespace(codePoint))
			{
				const size_t whitespaceBegin = index;
				while (index < p_text.size() && IsSoftWrapWhitespace(p_text[index]))
				{
					++index;
				}

				if (lineHasContent)
				{
					pendingWhitespace.insert(
						pendingWhitespace.end(),
						p_text.begin() + whitespaceBegin,
						p_text.begin() + index
					);
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
				output.push_back('\n');
				lineWidth = 0.0f;
				lineHasContent = false;
			}
			else if (lineHasContent && !pendingWhitespace.empty())
			{
				output.insert(output.end(), pendingWhitespace.begin(), pendingWhitespace.end());
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
	const float lineAdvance = p_input.font->GetLineHeight(p_input.fontSize) * scale;
	const float ascender = p_input.font->GetAscender(p_input.fontSize) * scale;
	const float descender = p_input.font->GetDescender(p_input.fontSize) * scale;

	struct LineInfo
	{
		size_t firstGlyph = 0;
		size_t lastGlyph = 0;
		float minX = std::numeric_limits<float>::max();
		float maxX = std::numeric_limits<float>::lowest();
		float advance = 0.0f;
		bool hasGeometry = false;
	};

	std::vector<LineInfo> lines;
	lines.push_back({});
	lines.back().firstGlyph = 0;

	float cursorX = 0.0f;
	float baselineY = 0.0f;
	float minY = std::numeric_limits<float>::max();
	float maxY = std::numeric_limits<float>::lowest();
	bool hasGeometry = false;

	const auto* fallbackGlyph = p_input.font->GetGlyph('?', p_input.fontSize);
	const auto codePoints = DecodeUTF8(p_input.text);
	const auto wrappedText = WrapTextToWidth(
		codePoints,
		*p_input.font,
		fallbackGlyph,
		p_input.fontSize,
		scale,
		p_input.uiSize.x
	);

	output.glyphs.reserve(wrappedText.size());

	for (const auto codePoint : wrappedText)
	{
		if (codePoint == '\r')
		{
			continue;
		}

		if (codePoint == '\n')
		{
			lines.back().lastGlyph = output.glyphs.size();
			lines.back().advance = cursorX;
			lines.push_back({});
			lines.back().firstGlyph = output.glyphs.size();
			cursorX = 0.0f;
			baselineY -= lineAdvance;
			continue;
		}

		const auto* glyph = p_input.font->GetGlyph(codePoint, p_input.fontSize);
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

		auto& line = lines.back();
		if (glyph->width > 0.0f && glyph->height > 0.0f)
		{
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

			hasGeometry = true;
			minY = std::min(minY, bottomY);
			maxY = std::max(maxY, topY);
			line.hasGeometry = true;
			line.minX = std::min(line.minX, x0);
			line.maxX = std::max(line.maxX, x1);
			line.lastGlyph = output.glyphs.size();
		}

		cursorX += glyph->xAdvance * scale;
		line.advance = cursorX;
	}

	lines.back().lastGlyph = output.glyphs.size();
	lines.back().advance = cursorX;

	float contentWidth = 0.0f;
	for (const auto& line : lines)
	{
		const float lineMinX = line.hasGeometry ? std::min(line.minX, 0.0f) : 0.0f;
		const float lineMaxX = line.hasGeometry ? std::max(line.maxX, line.advance) : line.advance;
		contentWidth = std::max(contentWidth, std::max(lineMaxX - lineMinX, 0.0f));
	}

	const float logicalTop = ascender;
	const float logicalBottom = -static_cast<float>(lines.size() - 1) * lineAdvance + descender;
	const float contentTop = hasGeometry ? std::max(logicalTop, maxY) : logicalTop;
	const float contentBottom = hasGeometry ? std::min(logicalBottom, minY) : logicalBottom;
	output.contentSize = {
		contentWidth,
		std::max(contentTop - contentBottom, 0.0f)
	};
	output.size = ResolveTextSize(output.contentSize, p_input.uiSize);

	for (const auto& line : lines)
	{
		if (!line.hasGeometry || line.lastGlyph <= line.firstGlyph)
		{
			continue;
		}

		const float lineMinX = std::min(line.minX, 0.0f);
		const float lineMaxX = std::max(line.maxX, line.advance);
		const float lineWidth = std::max(lineMaxX - lineMinX, 0.0f);
		const float lineCenterX = lineMinX + lineWidth * 0.5f;
		const float alignedLineCenterX = GetAlignedCenterX(output.size.x, lineWidth, p_input.horizontalAlignment);
		const float lineOffsetX = alignedLineCenterX - lineCenterX;

		for (size_t glyphIndex = line.firstGlyph; glyphIndex < line.lastGlyph; ++glyphIndex)
		{
			output.glyphs[glyphIndex].left += lineOffsetX;
			output.glyphs[glyphIndex].right += lineOffsetX;
		}
	}

	const float contentCenterY = contentBottom + output.contentSize.y * 0.5f;
	const float alignedCenterY = GetAlignedCenterY(output.size.y, output.contentSize.y, p_input.verticalAlignment);
	const float globalOffsetY = alignedCenterY - contentCenterY;

	for (auto& glyph : output.glyphs)
	{
		glyph.bottom += globalOffsetY;
		glyph.top += globalOffsetY;
	}

	return output;
}
