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

#include <ft2build.h>
#include FT_FREETYPE_H

namespace
{
	constexpr uint32_t kAtlasWidth = 512;
	constexpr uint32_t kAtlasHeight = 512;
	constexpr float kDefaultPixelSize = 32.0f;
	constexpr uint32_t kGlyphPadding = 1;
	constexpr const char* kFontAtlasUniform = "u_FontAtlas";

	void ConfigureEmbeddedMaterial(OvRendering::Data::Material& p_material)
	{
		p_material.SetOrthographicSupport(true);
		p_material.SetPerspectiveSupport(true);
		p_material.SetBlendable(true);
		p_material.SetUserInterface(true);
		p_material.SetBackfaceCulling(false);
		p_material.SetFrontfaceCulling(false);
		p_material.SetDepthTest(false);
		p_material.SetDepthWriting(false);
		p_material.SetColorWriting(true);
		p_material.SetCastShadows(false);
		p_material.SetReceiveShadows(false);
		p_material.SetCapturedByReflectionProbes(false);
		p_material.SetReceiveReflections(false);
		p_material.SetGPUInstances(1);
	}

	struct BakedFont
	{
		bool valid = false;
		float lineHeight = kDefaultPixelSize;
		std::array<OvRendering::Resources::Font::Glyph, OvRendering::Resources::Font::kGlyphCount> glyphs = {};
		std::vector<uint8_t> atlasData;
	};

	struct FreeTypeLibrary
	{
		FT_Library handle = nullptr;

		FreeTypeLibrary()
		{
			if (FT_Init_FreeType(&handle) != 0)
			{
				handle = nullptr;
			}
		}

		~FreeTypeLibrary()
		{
			if (handle)
			{
				FT_Done_FreeType(handle);
			}
		}
	};

	struct FreeTypeFace
	{
		FT_Face handle = nullptr;

		~FreeTypeFace()
		{
			if (handle)
			{
				FT_Done_Face(handle);
			}
		}
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

	bool CopyGlyphBitmap(
		const FT_Bitmap& p_bitmap,
		uint32_t p_x,
		uint32_t p_y,
		std::vector<uint8_t>& p_alphaAtlas
	)
	{
		if (p_bitmap.pixel_mode != FT_PIXEL_MODE_GRAY)
		{
			return false;
		}

		const uint32_t glyphWidth = static_cast<uint32_t>(p_bitmap.width);
		const uint32_t glyphHeight = static_cast<uint32_t>(p_bitmap.rows);
		const int32_t pitch = static_cast<int32_t>(p_bitmap.pitch);
		const uint32_t pitchAbs = static_cast<uint32_t>(pitch >= 0 ? pitch : -pitch);

		for (uint32_t row = 0; row < glyphHeight; ++row)
		{
			const uint32_t sourceRow = pitch >= 0 ? row : glyphHeight - 1 - row;
			const uint8_t* source = p_bitmap.buffer + static_cast<size_t>(sourceRow) * pitchAbs;
			const size_t destinationOffset = (static_cast<size_t>(p_y) + row) * kAtlasWidth + p_x;
			std::copy(source, source + glyphWidth, p_alphaAtlas.begin() + destinationOffset);
		}

		return true;
	}

	BakedFont BakeFont(const std::filesystem::path& p_realPath)
	{
		using namespace OvRendering::Resources;

		BakedFont result;

		FreeTypeLibrary library;
		if (!library.handle)
		{
			OVLOG_WARNING("Unable to initialize FreeType");
			return result;
		}

		const auto fontData = ReadFile(p_realPath);
		if (fontData.empty())
		{
			OVLOG_WARNING("Unable to read font: " + p_realPath.string());
			return result;
		}

		FreeTypeFace face;
		if (FT_New_Memory_Face(library.handle, fontData.data(), static_cast<FT_Long>(fontData.size()), 0, &face.handle) != 0)
		{
			OVLOG_WARNING("Unable to initialize font: " + p_realPath.string());
			return result;
		}

		if (FT_Select_Charmap(face.handle, FT_ENCODING_UNICODE) != 0)
		{
			OVLOG_WARNING("Font does not provide a Unicode charmap: " + p_realPath.string());
			return result;
		}

		if (FT_Set_Pixel_Sizes(face.handle, 0, static_cast<FT_UInt>(kDefaultPixelSize)) != 0)
		{
			OVLOG_WARNING("Unable to set font pixel size: " + p_realPath.string());
			return result;
		}

		if (face.handle->size)
		{
			result.lineHeight = static_cast<float>(face.handle->size->metrics.height) / 64.0f;
		}

		std::vector<uint8_t> alphaAtlas(kAtlasWidth * kAtlasHeight);
		uint32_t cursorX = 0;
		uint32_t cursorY = 0;
		uint32_t rowHeight = 0;

		for (uint32_t i = 0; i < Font::kGlyphCount; ++i)
		{
			const uint32_t character = Font::kFirstGlyph + i;
			if (FT_Load_Char(face.handle, character, FT_LOAD_RENDER | FT_LOAD_TARGET_NORMAL) != 0)
			{
				OVLOG_WARNING("Unable to load glyph from font atlas: " + p_realPath.string());
				return result;
			}

			const FT_GlyphSlot glyphSlot = face.handle->glyph;
			const FT_Bitmap& bitmap = glyphSlot->bitmap;
			const uint32_t glyphWidth = static_cast<uint32_t>(bitmap.width);
			const uint32_t glyphHeight = static_cast<uint32_t>(bitmap.rows);
			auto& glyph = result.glyphs[i];

			glyph.xOffset = static_cast<float>(glyphSlot->bitmap_left);
			glyph.yOffset = -static_cast<float>(glyphSlot->bitmap_top);
			glyph.xAdvance = static_cast<float>(glyphSlot->advance.x) / 64.0f;
			glyph.width = static_cast<float>(glyphWidth);
			glyph.height = static_cast<float>(glyphHeight);

			if (glyphWidth == 0 || glyphHeight == 0)
			{
				continue;
			}

			if (cursorX + glyphWidth + kGlyphPadding > kAtlasWidth)
			{
				cursorX = 0;
				cursorY += rowHeight + kGlyphPadding;
				rowHeight = 0;
			}

			if (cursorY + glyphHeight + kGlyphPadding > kAtlasHeight)
			{
				OVLOG_WARNING("Font atlas is too small for: " + p_realPath.string());
				return result;
			}

			if (!CopyGlyphBitmap(bitmap, cursorX, cursorY, alphaAtlas))
			{
				OVLOG_WARNING("Unsupported glyph bitmap format in font: " + p_realPath.string());
				return result;
			}

			glyph.uMin = static_cast<float>(cursorX) / static_cast<float>(kAtlasWidth);
			glyph.vMin = static_cast<float>(cursorY) / static_cast<float>(kAtlasHeight);
			glyph.uMax = static_cast<float>(cursorX + glyphWidth) / static_cast<float>(kAtlasWidth);
			glyph.vMax = static_cast<float>(cursorY + glyphHeight) / static_cast<float>(kAtlasHeight);

			cursorX += glyphWidth + kGlyphPadding;
			rowHeight = std::max(rowHeight, glyphHeight);
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
	if (m_embeddedMaterial && m_embeddedMaterial->IsValid())
	{
		m_embeddedMaterial->TrySetProperty(kFontAtlasUniform, m_atlasTexture);
	}

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

bool OvRendering::Resources::Font::EnsureEmbeddedMaterial(Shader* p_shader)
{
	if (!p_shader || !m_atlasTexture)
	{
		m_embeddedMaterial.reset();
		return false;
	}

	if (!m_embeddedMaterial)
	{
		m_embeddedMaterial = std::make_unique<Data::Material>(p_shader);
		ConfigureEmbeddedMaterial(*m_embeddedMaterial);
	}
	else if (m_embeddedMaterial->GetShader() != p_shader)
	{
		m_embeddedMaterial->SetShader(p_shader);
		ConfigureEmbeddedMaterial(*m_embeddedMaterial);
	}

	if (!m_embeddedMaterial->IsValid())
	{
		return false;
	}

	m_embeddedMaterial->TrySetProperty(kFontAtlasUniform, m_atlasTexture);
	return true;
}

OvRendering::Data::Material* OvRendering::Resources::Font::GetEmbeddedMaterial() const
{
	return m_embeddedMaterial.get();
}
