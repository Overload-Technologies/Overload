/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include <algorithm>
#include <array>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <vector>

#include <OvDebug/Logger.h>
#include <OvRendering/Resources/Font.h>
#include <OvRendering/Resources/Loaders/TextureLoader.h>
#include <OvRendering/Settings/ETextureFilteringMode.h>
#include <OvRendering/Settings/ETextureWrapMode.h>

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable: 4244 4267 4456 4505 4996 5262 6385)
#endif
#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-function"
#pragma clang diagnostic ignored "-Wimplicit-fallthrough"
#endif
#if defined(__GNUC__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wimplicit-fallthrough"
#endif
#define STBTT_STATIC
#define STB_TRUETYPE_IMPLEMENTATION
#include <imstb_truetype.h>
#if defined(__GNUC__)
#pragma GCC diagnostic pop
#endif
#if defined(__clang__)
#pragma clang diagnostic pop
#endif
#ifdef _MSC_VER
#pragma warning(pop)
#endif

namespace
{
	constexpr uint32_t kAtlasWidth = 512;
	constexpr uint32_t kAtlasHeight = 512;
	constexpr float kDefaultPixelSize = 32.0f;

	struct BakedFont
	{
		bool valid = false;
		float lineHeight = kDefaultPixelSize;
		std::array<OvRendering::Resources::Font::Glyph, OvRendering::Resources::Font::kGlyphCount> glyphs = {};
		std::vector<uint8_t> atlasData;
	};

	std::vector<uint8_t> ReadFile(const std::filesystem::path& p_path)
	{
		std::ifstream file{ p_path, std::ios::binary | std::ios::ate };
		if (!file)
		{
			return {};
		}

		const auto size = file.tellg();
		if (size <= std::streampos{ 0 })
		{
			return {};
		}

		std::vector<uint8_t> data(static_cast<size_t>(size));
		file.seekg(0, std::ios::beg);
		file.read(reinterpret_cast<char*>(data.data()), static_cast<std::streamsize>(data.size()));

		return file ? data : std::vector<uint8_t>{};
	}

	BakedFont BakeFont(const std::filesystem::path& p_realPath)
	{
		using namespace OvRendering::Resources;

		BakedFont result;

		const auto fontData = ReadFile(p_realPath);
		if (fontData.empty())
		{
			OVLOG_WARNING("Unable to read font: " + p_realPath.string());
			return result;
		}

		stbtt_fontinfo fontInfo;
		const int fontOffset = stbtt_GetFontOffsetForIndex(fontData.data(), 0);
		if (fontOffset < 0 || !stbtt_InitFont(&fontInfo, fontData.data(), fontOffset))
		{
			OVLOG_WARNING("Unable to initialize font: " + p_realPath.string());
			return result;
		}

		int ascent = 0;
		int descent = 0;
		int lineGap = 0;
		stbtt_GetFontVMetrics(&fontInfo, &ascent, &descent, &lineGap);
		result.lineHeight = static_cast<float>(ascent - descent + lineGap) * stbtt_ScaleForPixelHeight(&fontInfo, kDefaultPixelSize);

		std::array<stbtt_bakedchar, Font::kGlyphCount> bakedChars = {};
		std::vector<uint8_t> alphaAtlas(kAtlasWidth * kAtlasHeight);

		const int bakeResult = stbtt_BakeFontBitmap(
			fontData.data(),
			fontOffset,
			kDefaultPixelSize,
			alphaAtlas.data(),
			kAtlasWidth,
			kAtlasHeight,
			Font::kFirstGlyph,
			Font::kGlyphCount,
			bakedChars.data()
		);

		if (bakeResult <= 0)
		{
			OVLOG_WARNING("Unable to bake font atlas: " + p_realPath.string());
			return result;
		}

		result.atlasData.resize(kAtlasWidth * kAtlasHeight * 4);
		for (size_t i = 0; i < alphaAtlas.size(); ++i)
		{
			const size_t offset = i * 4;
			result.atlasData[offset + 0] = 255;
			result.atlasData[offset + 1] = 255;
			result.atlasData[offset + 2] = 255;
			result.atlasData[offset + 3] = alphaAtlas[i];
		}

		for (uint32_t i = 0; i < Font::kGlyphCount; ++i)
		{
			const auto& baked = bakedChars[i];
			auto& glyph = result.glyphs[i];
			glyph.xOffset = baked.xoff;
			glyph.yOffset = baked.yoff;
			glyph.xAdvance = baked.xadvance;
			glyph.width = static_cast<float>(baked.x1 - baked.x0);
			glyph.height = static_cast<float>(baked.y1 - baked.y0);
			glyph.uMin = static_cast<float>(baked.x0) / static_cast<float>(kAtlasWidth);
			glyph.vMin = static_cast<float>(baked.y0) / static_cast<float>(kAtlasHeight);
			glyph.uMax = static_cast<float>(baked.x1) / static_cast<float>(kAtlasWidth);
			glyph.vMax = static_cast<float>(baked.y1) / static_cast<float>(kAtlasHeight);
		}

		result.valid = true;
		return result;
	}
}

OvRendering::Resources::Font::Font(const std::string& p_path, const std::filesystem::path& p_realPath) :
	path(p_path)
{
	Reload(p_realPath);
}

OvRendering::Resources::Font::~Font()
{
	Loaders::TextureLoader::Destroy(m_atlasTexture);
}

bool OvRendering::Resources::Font::Reload(const std::filesystem::path& p_realPath)
{
	using namespace OvRendering::Settings;

	auto bakedFont = BakeFont(p_realPath);
	if (!bakedFont.valid)
	{
		return false;
	}

	if (m_atlasTexture)
	{
		Loaders::TextureLoader::ReloadFromMemory(
			*m_atlasTexture,
			bakedFont.atlasData.data(),
			kAtlasWidth,
			kAtlasHeight,
			ETextureFilteringMode::LINEAR,
			ETextureFilteringMode::LINEAR,
			ETextureWrapMode::CLAMP_TO_EDGE,
			ETextureWrapMode::CLAMP_TO_EDGE,
			false
		);
	}
	else
	{
		m_atlasTexture = Loaders::TextureLoader::CreateFromMemory(
			bakedFont.atlasData.data(),
			kAtlasWidth,
			kAtlasHeight,
			ETextureFilteringMode::LINEAR,
			ETextureFilteringMode::LINEAR,
			ETextureWrapMode::CLAMP_TO_EDGE,
			ETextureWrapMode::CLAMP_TO_EDGE,
			false
		);

		if (!m_atlasTexture)
		{
			OVLOG_WARNING("Unable to create font atlas texture: " + p_realPath.string());
			return false;
		}

		const_cast<std::string&>(m_atlasTexture->path) = path;
	}

	m_valid = true;
	m_pixelSize = kDefaultPixelSize;
	m_lineHeight = bakedFont.lineHeight;
	m_glyphs = bakedFont.glyphs;

	return true;
}

bool OvRendering::Resources::Font::IsValid() const
{
	return m_valid && m_atlasTexture;
}

float OvRendering::Resources::Font::GetPixelSize() const
{
	return m_pixelSize;
}

float OvRendering::Resources::Font::GetLineHeight() const
{
	return m_lineHeight;
}

const OvRendering::Resources::Font::Glyph* OvRendering::Resources::Font::GetGlyph(char p_character) const
{
	const auto character = static_cast<uint8_t>(p_character);
	if (character < kFirstGlyph || character >= kFirstGlyph + kGlyphCount)
	{
		return nullptr;
	}

	return &m_glyphs[character - kFirstGlyph];
}

OvRendering::Resources::Texture* OvRendering::Resources::Font::GetAtlasTexture() const
{
	return m_atlasTexture;
}
