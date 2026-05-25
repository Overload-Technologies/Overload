/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include <algorithm>
#include <array>
#include <cmath>
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
	constexpr uint32_t kMinimumAtlasSize = 512;
	constexpr uint32_t kMaximumAtlasSize = 4096;
	constexpr float kDefaultPixelSize = 32.0f;
	constexpr float kMinimumPixelSize = 1.0f;
	constexpr float kMaximumPixelSize = 256.0f;
	constexpr uint32_t kGlyphPadding = 1;
	constexpr const char* kFontAtlasUniform = "u_FontAtlas";

	uint32_t ToPixelSizeKey(float p_pixelSize)
	{
		if (!std::isfinite(p_pixelSize))
		{
			return static_cast<uint32_t>(kDefaultPixelSize);
		}

		const auto clamped = std::clamp(p_pixelSize, kMinimumPixelSize, kMaximumPixelSize);
		return std::max<uint32_t>(1, static_cast<uint32_t>(std::lround(clamped)));
	}

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
		float pixelSize = kDefaultPixelSize;
		float lineHeight = kDefaultPixelSize;
		uint32_t atlasWidth = 0;
		uint32_t atlasHeight = 0;
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
		uint32_t p_atlasWidth,
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
			const size_t destinationOffset = (static_cast<size_t>(p_y) + row) * p_atlasWidth + p_x;
			std::copy(source, source + glyphWidth, p_alphaAtlas.begin() + destinationOffset);
		}

		return true;
	}

	BakedFont BakeFont(const std::filesystem::path& p_realPath, float p_pixelSize)
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

		if (FT_Set_Pixel_Sizes(face.handle, 0, static_cast<FT_UInt>(ToPixelSizeKey(p_pixelSize))) != 0)
		{
			OVLOG_WARNING("Unable to set font pixel size: " + p_realPath.string());
			return result;
		}

		result.pixelSize = static_cast<float>(ToPixelSizeKey(p_pixelSize));

		if (face.handle->size)
		{
			result.lineHeight = static_cast<float>(face.handle->size->metrics.height) / 64.0f;
		}

		for (uint32_t atlasSize = kMinimumAtlasSize; atlasSize <= kMaximumAtlasSize; atlasSize *= 2)
		{
			std::vector<uint8_t> alphaAtlas(static_cast<size_t>(atlasSize) * atlasSize);
			uint32_t cursorX = 0;
			uint32_t cursorY = 0;
			uint32_t rowHeight = 0;
			bool fits = true;

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

				if (cursorX + glyphWidth + kGlyphPadding > atlasSize)
				{
					cursorX = 0;
					cursorY += rowHeight + kGlyphPadding;
					rowHeight = 0;
				}

				if (cursorY + glyphHeight + kGlyphPadding > atlasSize)
				{
					fits = false;
					break;
				}

				if (!CopyGlyphBitmap(bitmap, cursorX, cursorY, atlasSize, alphaAtlas))
				{
					OVLOG_WARNING("Unsupported glyph bitmap format in font: " + p_realPath.string());
					return result;
				}

				glyph.uMin = static_cast<float>(cursorX) / static_cast<float>(atlasSize);
				glyph.vMin = static_cast<float>(cursorY) / static_cast<float>(atlasSize);
				glyph.uMax = static_cast<float>(cursorX + glyphWidth) / static_cast<float>(atlasSize);
				glyph.vMax = static_cast<float>(cursorY + glyphHeight) / static_cast<float>(atlasSize);

				cursorX += glyphWidth + kGlyphPadding;
				rowHeight = std::max(rowHeight, glyphHeight);
			}

			if (!fits)
			{
				continue;
			}

			result.atlasWidth = atlasSize;
			result.atlasHeight = atlasSize;
			result.atlasData.resize(static_cast<size_t>(atlasSize) * atlasSize * 4);

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

		OVLOG_WARNING("Font atlas is too small for: " + p_realPath.string());
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
	DestroyAtlasVariants();
}

bool OvRendering::Resources::Font::Reload(const std::filesystem::path& p_realPath)
{
	m_realPath = p_realPath;
	m_activePixelSize = ToPixelSizeKey(kDefaultPixelSize);
	m_valid = false;
	DestroyAtlasVariants();
	m_atlasVariants.clear();

	return SetActivePixelSize(static_cast<float>(m_activePixelSize));
}

bool OvRendering::Resources::Font::SetActivePixelSize(float p_pixelSize)
{
	const auto pixelSize = ToPixelSizeKey(p_pixelSize);
	auto* variant = GetOrCreateVariant(pixelSize);
	if (!variant)
	{
		m_valid = false;
		return false;
	}

	m_activePixelSize = pixelSize;
	m_valid = variant->valid && variant->atlasTexture;
	return m_valid;
}

bool OvRendering::Resources::Font::EnsurePixelSize(float p_pixelSize)
{
	return GetOrCreateVariant(ToPixelSizeKey(p_pixelSize)) != nullptr;
}

bool OvRendering::Resources::Font::IsValid() const
{
	const auto* variant = GetActiveVariant();
	return m_valid && variant && variant->atlasTexture;
}

float OvRendering::Resources::Font::GetPixelSize() const
{
	if (const auto* variant = GetActiveVariant(); variant)
	{
		return variant->pixelSize;
	}

	return static_cast<float>(m_activePixelSize);
}

float OvRendering::Resources::Font::GetPixelSize(float p_pixelSize) const
{
	if (const auto* variant = GetVariant(ToPixelSizeKey(p_pixelSize)); variant)
	{
		return variant->pixelSize;
	}

	return static_cast<float>(ToPixelSizeKey(p_pixelSize));
}

float OvRendering::Resources::Font::GetLineHeight() const
{
	if (const auto* variant = GetActiveVariant(); variant)
	{
		return variant->lineHeight;
	}

	return static_cast<float>(m_activePixelSize);
}

float OvRendering::Resources::Font::GetLineHeight(float p_pixelSize) const
{
	if (const auto* variant = GetVariant(ToPixelSizeKey(p_pixelSize)); variant)
	{
		return variant->lineHeight;
	}

	return static_cast<float>(ToPixelSizeKey(p_pixelSize));
}

const OvRendering::Resources::Font::Glyph* OvRendering::Resources::Font::GetGlyph(char p_character) const
{
	const auto character = static_cast<uint8_t>(p_character);
	if (character < kFirstGlyph || character >= kFirstGlyph + kGlyphCount)
	{
		return nullptr;
	}

	if (const auto* variant = GetActiveVariant(); variant)
	{
		return &variant->glyphs[character - kFirstGlyph];
	}

	return nullptr;
}

const OvRendering::Resources::Font::Glyph* OvRendering::Resources::Font::GetGlyph(char p_character, float p_pixelSize) const
{
	const auto character = static_cast<uint8_t>(p_character);
	if (character < kFirstGlyph || character >= kFirstGlyph + kGlyphCount)
	{
		return nullptr;
	}

	const auto* variant = GetVariant(ToPixelSizeKey(p_pixelSize));
	return variant ? &variant->glyphs[character - kFirstGlyph] : nullptr;
}

OvRendering::Resources::Texture* OvRendering::Resources::Font::GetAtlasTexture() const
{
	if (const auto* variant = GetActiveVariant(); variant)
	{
		return variant->atlasTexture;
	}

	return nullptr;
}

OvRendering::Resources::Texture* OvRendering::Resources::Font::GetAtlasTexture(float p_pixelSize)
{
	if (auto* variant = GetOrCreateVariant(ToPixelSizeKey(p_pixelSize)); variant)
	{
		return variant->atlasTexture;
	}

	return nullptr;
}

bool OvRendering::Resources::Font::EnsureEmbeddedMaterial(Shader* p_shader)
{
	return EnsureEmbeddedMaterial(p_shader, static_cast<float>(m_activePixelSize));
}

bool OvRendering::Resources::Font::EnsureEmbeddedMaterial(Shader* p_shader, float p_pixelSize)
{
	auto* variant = GetOrCreateVariant(ToPixelSizeKey(p_pixelSize));
	if (!variant)
	{
		return false;
	}

	if (!p_shader || !variant->atlasTexture)
	{
		variant->embeddedMaterial.reset();
		return false;
	}

	if (!variant->embeddedMaterial)
	{
		variant->embeddedMaterial = std::make_unique<Data::Material>(p_shader);
		ConfigureEmbeddedMaterial(*variant->embeddedMaterial);
	}
	else if (variant->embeddedMaterial->GetShader() != p_shader)
	{
		variant->embeddedMaterial->SetShader(p_shader);
		ConfigureEmbeddedMaterial(*variant->embeddedMaterial);
	}

	if (!variant->embeddedMaterial->IsValid())
	{
		return false;
	}

	variant->embeddedMaterial->TrySetProperty(kFontAtlasUniform, variant->atlasTexture);
	return true;
}

OvRendering::Data::Material* OvRendering::Resources::Font::GetEmbeddedMaterial() const
{
	if (const auto* variant = GetActiveVariant(); variant)
	{
		return variant->embeddedMaterial.get();
	}

	return nullptr;
}

OvRendering::Data::Material* OvRendering::Resources::Font::GetEmbeddedMaterial(float p_pixelSize)
{
	if (auto* variant = GetOrCreateVariant(ToPixelSizeKey(p_pixelSize)); variant)
	{
		return variant->embeddedMaterial.get();
	}

	return nullptr;
}

OvRendering::Resources::Font::AtlasVariant* OvRendering::Resources::Font::GetActiveVariant()
{
	return GetVariant(m_activePixelSize);
}

const OvRendering::Resources::Font::AtlasVariant* OvRendering::Resources::Font::GetActiveVariant() const
{
	return GetVariant(m_activePixelSize);
}

OvRendering::Resources::Font::AtlasVariant* OvRendering::Resources::Font::GetVariant(uint32_t p_pixelSize)
{
	const auto found = m_atlasVariants.find(p_pixelSize);
	return found != m_atlasVariants.end() ? &found->second : nullptr;
}

const OvRendering::Resources::Font::AtlasVariant* OvRendering::Resources::Font::GetVariant(uint32_t p_pixelSize) const
{
	const auto found = m_atlasVariants.find(p_pixelSize);
	return found != m_atlasVariants.end() ? &found->second : nullptr;
}

OvRendering::Resources::Font::AtlasVariant* OvRendering::Resources::Font::GetOrCreateVariant(uint32_t p_pixelSize)
{
	if (auto* variant = GetVariant(p_pixelSize))
	{
		return variant->valid ? variant : nullptr;
	}

	if (!CreateAtlasVariant(p_pixelSize))
	{
		auto& failedVariant = m_atlasVariants[p_pixelSize];
		failedVariant.valid = false;
		failedVariant.pixelSize = static_cast<float>(p_pixelSize);
		return nullptr;
	}

	if (auto* variant = GetVariant(p_pixelSize))
	{
		return variant->valid ? variant : nullptr;
	}

	return nullptr;
}

void OvRendering::Resources::Font::DestroyAtlasVariants()
{
	for (auto& [_, variant] : m_atlasVariants)
	{
		Loaders::TextureLoader::Destroy(variant.atlasTexture);
		variant.atlasTexture = nullptr;
		variant.atlasWidth = 0;
		variant.atlasHeight = 0;
		variant.embeddedMaterial.reset();
		variant.valid = false;
	}
}

bool OvRendering::Resources::Font::CreateAtlasVariant(uint32_t p_pixelSize)
{
	using namespace OvRendering::Settings;

	if (m_realPath.empty())
	{
		return false;
	}

	auto bakedFont = BakeFont(m_realPath, static_cast<float>(p_pixelSize));
	if (!bakedFont.valid)
	{
		return false;
	}

	auto it = m_atlasVariants.find(p_pixelSize);
	if (it == m_atlasVariants.end())
	{
		it = m_atlasVariants.emplace(p_pixelSize, AtlasVariant{}).first;
	}

	auto& variant = it->second;

	if (variant.atlasTexture)
	{
		if (variant.atlasWidth == bakedFont.atlasWidth && variant.atlasHeight == bakedFont.atlasHeight)
		{
			Loaders::TextureLoader::ReloadFromMemory(
				*variant.atlasTexture,
				bakedFont.atlasData.data(),
				bakedFont.atlasWidth,
				bakedFont.atlasHeight,
				ETextureFilteringMode::LINEAR,
				ETextureFilteringMode::LINEAR,
				ETextureWrapMode::CLAMP_TO_EDGE,
				ETextureWrapMode::CLAMP_TO_EDGE,
				false
			);
		}
		else
		{
			Loaders::TextureLoader::Destroy(variant.atlasTexture);
			variant.atlasTexture = nullptr;
		}
	}

	if (!variant.atlasTexture)
	{
		variant.atlasTexture = Loaders::TextureLoader::CreateFromMemory(
			bakedFont.atlasData.data(),
			bakedFont.atlasWidth,
			bakedFont.atlasHeight,
			ETextureFilteringMode::LINEAR,
			ETextureFilteringMode::LINEAR,
			ETextureWrapMode::CLAMP_TO_EDGE,
			ETextureWrapMode::CLAMP_TO_EDGE,
			false
		);

		if (!variant.atlasTexture)
		{
			OVLOG_WARNING("Unable to create font atlas texture: " + m_realPath.string());
			return false;
		}
	}

	variant.valid = true;
	variant.pixelSize = bakedFont.pixelSize;
	variant.lineHeight = bakedFont.lineHeight;
	variant.atlasWidth = bakedFont.atlasWidth;
	variant.atlasHeight = bakedFont.atlasHeight;
	variant.glyphs = bakedFont.glyphs;
	if (variant.embeddedMaterial && variant.embeddedMaterial->IsValid())
	{
		variant.embeddedMaterial->TrySetProperty(kFontAtlasUniform, variant.atlasTexture);
	}

	return true;
}
