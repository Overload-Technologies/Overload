/**
* @project: baregl
* @author: Adrien Givry
* @licence: MIT
*/

#pragma once

#include <baregl/types/EFormat.h>
#include <baregl/types/EInternalFormat.h>
#include <baregl/types/EPixelDataType.h>
#include <baregl/types/ETextureFilteringMode.h>
#include <baregl/types/ETextureWrapMode.h>

#include <optional>

namespace baregl::data
{
	/**
	* Structure that holds additional data for mutable textures
	*/
	struct MutableTextureDesc
	{
		types::EFormat format = types::EFormat::RGBA;
		types::EPixelDataType type = types::EPixelDataType::UNSIGNED_BYTE;
		const void* data = nullptr;
	};

	/**
	* Structure that holds the description of a texture
	*/
	struct TextureDesc
	{
		uint32_t width = 0;
		uint32_t height = 0;
		types::ETextureFilteringMode minFilter = types::ETextureFilteringMode::LINEAR_MIPMAP_LINEAR;
		types::ETextureFilteringMode magFilter = types::ETextureFilteringMode::LINEAR;
		types::ETextureWrapMode horizontalWrap = types::ETextureWrapMode::REPEAT;
		types::ETextureWrapMode verticalWrap = types::ETextureWrapMode::REPEAT;
		types::EInternalFormat internalFormat = types::EInternalFormat::RGBA;
		bool useMipMaps = true;
		std::optional<MutableTextureDesc> mutableDesc = std::nullopt;
	};
}
