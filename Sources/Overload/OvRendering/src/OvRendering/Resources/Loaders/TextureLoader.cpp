/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#define STB_IMAGE_IMPLEMENTATION

#include <array>
#include <format>
#include <memory>
#include <stb_image/stb_image.h>

#include <OvDebug/Logger.h>
#include <OvRendering/Resources/Loaders/TextureLoader.h>
#include <OvTools/Utils/PathParser.h>

namespace
{
	/**
	* Simple wrapper for stb_image. Handles SDR and HDR image loading,
	* and enforces RAII for the loaded data.
	*/
	struct Image
	{
		int width = 0;
		int height = 0;
		int bpp = 0;
		bool isHDR = false;
		void* data = nullptr;

		Image(const std::string& p_filepath)
		{
			stbi_set_flip_vertically_on_load(true);

			isHDR = stbi_is_hdr(p_filepath.c_str());

			data = isHDR ?
				static_cast<void*>(stbi_loadf(p_filepath.c_str(), &width, &height, &bpp, 4)) :
				static_cast<void*>(stbi_load(p_filepath.c_str(), &width, &height, &bpp, 4));
		}

		virtual ~Image() { stbi_image_free(data); }
		bool IsValid() const { return data; }
		operator bool() const { return IsValid(); }
	};

	void PrepareTexture(
		OvRendering::HAL::Texture& p_texture,
		void* p_data,
		OvRendering::Settings::ETextureFilteringMode p_minFilter,
		OvRendering::Settings::ETextureFilteringMode p_magFilter,
		OvRendering::Settings::ETextureWrapMode p_horizontalWrapMode,
		OvRendering::Settings::ETextureWrapMode p_verticalWrapMode,
		uint32_t p_width,
		uint32_t p_height,
		bool p_generateMipmap,
		bool p_hdr
	)
	{
		using namespace OvRendering::Settings;

		p_texture.Allocate({
			.width = p_width,
			.height = p_height,
			.minFilter = p_minFilter,
			.magFilter = p_magFilter,
			.horizontalWrap = p_horizontalWrapMode,
			.verticalWrap = p_verticalWrapMode,
			.internalFormat = p_hdr ? EInternalFormat::RGBA32F : EInternalFormat::RGBA8,
			.useMipMaps = p_generateMipmap
		});

		p_texture.Upload(p_data, EFormat::RGBA, p_hdr ? EPixelDataType::FLOAT : EPixelDataType::UNSIGNED_BYTE);

		if (p_generateMipmap)
		{
			p_texture.GenerateMipmaps();
		}
	}
}

OvRendering::Resources::Texture* OvRendering::Resources::Loaders::TextureLoader::Create(
	const std::string& p_filepath,
	OvRendering::Settings::ETextureFilteringMode p_minFilter,
	OvRendering::Settings::ETextureFilteringMode p_magFilter,
	OvRendering::Settings::ETextureWrapMode p_horizontalWrapMode,
	OvRendering::Settings::ETextureWrapMode p_verticalWrapMode,
	bool p_generateMipmap
)
{
	if (Image image{ p_filepath })
	{
		auto texture = std::make_unique<HAL::Texture>(
			Settings::ETextureType::TEXTURE_2D,
			OvTools::Utils::PathParser::GetElementName(p_filepath)
		);

		PrepareTexture(
			*texture,
			image.data,
			p_minFilter,
			p_magFilter,
			p_horizontalWrapMode,
			p_verticalWrapMode,
			image.width,
			image.height,
			p_generateMipmap,
			image.isHDR
		);

		return new Texture{ p_filepath, std::move(texture) };
	}

	return nullptr;
}

OvRendering::Resources::Texture* OvRendering::Resources::Loaders::TextureLoader::CreatePixel(
	uint8_t r,
	uint8_t g,
	uint8_t b,
	uint8_t a
)
{
	std::array<uint8_t, 4> colorData = { r, g, b, a };

	return OvRendering::Resources::Loaders::TextureLoader::CreateFromMemory(
		colorData.data(), 1, 1,
		OvRendering::Settings::ETextureFilteringMode::NEAREST,
		OvRendering::Settings::ETextureFilteringMode::NEAREST,
		OvRendering::Settings::ETextureWrapMode::REPEAT,
		OvRendering::Settings::ETextureWrapMode::REPEAT,
		false
	);
}

OvRendering::Resources::Texture* OvRendering::Resources::Loaders::TextureLoader::CreateFromMemory(
	uint8_t* p_data,
	uint32_t p_width,
	uint32_t p_height,
	OvRendering::Settings::ETextureFilteringMode p_minFilter,
	OvRendering::Settings::ETextureFilteringMode p_magFilter,
	OvRendering::Settings::ETextureWrapMode p_horizontalWrapMode,
	OvRendering::Settings::ETextureWrapMode p_verticalWrapMode,
	bool p_generateMipmap
)
{
	auto texture = std::make_unique<HAL::Texture>(Settings::ETextureType::TEXTURE_2D, "FromMemory");

	PrepareTexture(
		*texture,
		p_data,
		p_minFilter,
		p_magFilter,
		p_horizontalWrapMode,
		p_verticalWrapMode,
		p_width,
		p_height,
		p_generateMipmap,
		false
	);

	return new Texture("", std::move(texture));
}

void OvRendering::Resources::Loaders::TextureLoader::Reload(
	Texture& p_texture,
	const std::string& p_filePath,
	OvRendering::Settings::ETextureFilteringMode p_minFilter,
	OvRendering::Settings::ETextureFilteringMode p_magFilter,
	OvRendering::Settings::ETextureWrapMode p_horizontalWrapMode,
	OvRendering::Settings::ETextureWrapMode p_verticalWrapMode,
	bool p_generateMipmap
)
{
	if (Image image{ p_filePath })
	{
		auto texture = std::make_unique<HAL::Texture>(
			Settings::ETextureType::TEXTURE_2D,
			OvTools::Utils::PathParser::GetElementName(p_filePath)
		);

		PrepareTexture(
			*texture,
			image.data,
			p_minFilter,
			p_magFilter,
			p_horizontalWrapMode,
			p_verticalWrapMode,
			image.width,
			image.height,
			p_generateMipmap,
			image.isHDR
		);

		p_texture.SetTexture(std::move(texture));
	}
}

bool OvRendering::Resources::Loaders::TextureLoader::Destroy(Texture*& p_textureInstance)
{
	if (p_textureInstance)
	{
		delete p_textureInstance;
		p_textureInstance = nullptr;
		return true;
	}

	return false;
}
