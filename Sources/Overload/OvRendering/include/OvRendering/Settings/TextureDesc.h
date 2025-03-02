/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include <OvRendering/Settings/ETextureFilteringMode.h>
#include <OvRendering/Settings/ETextureWrapMode.h>
#include <OvRendering/Settings/EInternalFormat.h>
#include <OvRendering/Settings/EFormat.h>
#include <OvRendering/Settings/EPixelDataType.h>

namespace OvRendering::Settings
{
	/**
	* Structure that holds additional data for mutable textures
	*/
	struct MutableTextureDesc
	{
		EFormat format = EFormat::RGBA;
		EPixelDataType type = EPixelDataType::UNSIGNED_BYTE;
		const void* data = nullptr;
	};

	/**
	* Structure that holds the description of a texture
	*/
	struct TextureDesc
	{
		uint32_t width = 0;
		uint32_t height = 0;
		ETextureFilteringMode minFilter = ETextureFilteringMode::LINEAR_MIPMAP_LINEAR;
		ETextureFilteringMode magFilter = ETextureFilteringMode::LINEAR;
		ETextureWrapMode horizontalWrap = ETextureWrapMode::REPEAT;
		ETextureWrapMode verticalWrap = ETextureWrapMode::REPEAT;
		EInternalFormat internalFormat = EInternalFormat::RGBA;
		bool useMipMaps = true;
		std::optional<MutableTextureDesc> mutableDesc = std::nullopt;
	};
}
