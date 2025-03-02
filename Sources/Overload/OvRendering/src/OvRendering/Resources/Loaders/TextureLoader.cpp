/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#define STB_IMAGE_IMPLEMENTATION

#include <stb_image/stb_image.h>
#include <array>
#include <memory>

#include "OvRendering/Resources/Loaders/TextureLoader.h"

namespace
{
	struct Image
	{
		uint8_t* data;
		int width;
		int height;
		int bpp;

		Image(const std::string& p_filepath)
		{
			stbi_set_flip_vertically_on_load(true);
			data = stbi_load(p_filepath.c_str(), &width, &height, &bpp, 4);
		}

		~Image()
		{
			stbi_image_free(data);
		}

		bool IsValid() const
		{
			return data;
		}

		operator bool() const
		{
			return IsValid();
		}
	};

	void PrepareTexture(
		OvRendering::HAL::Texture& p_texture,
		uint8_t* p_data,
		OvRendering::Settings::ETextureFilteringMode p_minFilter,
		OvRendering::Settings::ETextureFilteringMode p_magFilter,
		uint32_t p_width,
		uint32_t p_height,
		bool p_generateMipmap
	)
	{
		using namespace OvRendering::Settings;

		p_texture.Allocate({
			.width = p_width,
			.height = p_height,
			.minFilter = p_minFilter,
			.magFilter = p_magFilter,
			.internalFormat = EInternalFormat::RGBA8,
			.useMipMaps = p_generateMipmap
		});

		p_texture.Upload(p_data, EFormat::RGBA, EPixelDataType::UNSIGNED_BYTE);

		if (p_generateMipmap)
		{
			p_texture.GenerateMipmaps();
		}
	}
}

OvRendering::Resources::Texture* OvRendering::Resources::Loaders::TextureLoader::Create(const std::string& p_filepath, OvRendering::Settings::ETextureFilteringMode p_minFilter, OvRendering::Settings::ETextureFilteringMode p_magFilter, bool p_generateMipmap)
{
	if (Image image{ p_filepath })
	{
		auto texture = std::make_unique<HAL::Texture>();
		PrepareTexture(*texture, image.data, p_minFilter, p_magFilter, image.width, image.height, p_generateMipmap);
		return new Texture{ p_filepath, std::move(texture) };
	}

	return nullptr;
}

OvRendering::Resources::Texture* OvRendering::Resources::Loaders::TextureLoader::CreatePixel(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
{
	std::array<uint8_t, 4> colorData = { r, g, b, a };

	return OvRendering::Resources::Loaders::TextureLoader::CreateFromMemory(
		colorData.data(), 1, 1,
		OvRendering::Settings::ETextureFilteringMode::NEAREST,
		OvRendering::Settings::ETextureFilteringMode::NEAREST,
		false
	);
}

OvRendering::Resources::Texture* OvRendering::Resources::Loaders::TextureLoader::CreateFromMemory(uint8_t* p_data, uint32_t p_width, uint32_t p_height, OvRendering::Settings::ETextureFilteringMode p_minFilter, OvRendering::Settings::ETextureFilteringMode p_magFilter, bool p_generateMipmap)
{
	auto texture = std::make_unique<HAL::Texture>();
	PrepareTexture(*texture, p_data, p_minFilter, p_magFilter, p_width, p_height, p_generateMipmap);
	return new Texture("", std::move(texture));
}

void OvRendering::Resources::Loaders::TextureLoader::Reload(Texture& p_texture, const std::string& p_filePath, OvRendering::Settings::ETextureFilteringMode p_minFilter, OvRendering::Settings::ETextureFilteringMode p_magFilter, bool p_generateMipmap)
{
	if (Image image{ p_filePath })
	{
		auto texture = std::make_unique<HAL::Texture>();
		PrepareTexture(*texture, image.data, p_minFilter, p_magFilter, image.width, image.height, p_generateMipmap);
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
