/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include <string>
#include <vector>

#include "OvRendering/Resources/Texture.h"


namespace OvRendering::Resources::Loaders
{
	/**
	* Handle the Texture creation and destruction
	*/
	class TextureLoader
	{
	public:
		/**
		* Disabled constructor
		*/
		TextureLoader() = delete;

		/**
		* Create a texture from file
		* @param p_filePath
		* @param p_minFilter
		* @param p_magFilter
		* @param p_generateMipmap
		* @param p_hdr
		*/
		static Texture* Create(
			const std::string& p_filepath,
			OvRendering::Settings::ETextureFilteringMode p_minFilter,
			OvRendering::Settings::ETextureFilteringMode p_magFilter,
			bool p_generateMipmap,
			bool p_hdr = false
		);

		/**
		* Create a texture from a single SDR pixel color
		* @param p_r
		* @param p_g
		* @param p_b
		* @param p_a
		*/
		static Texture* CreatePixel(uint8_t r, uint8_t g, uint8_t b, uint8_t a);

		/**
		* Create a texture from memory
		* @param p_data
		* @param p_width
		* @param p_height
		* @param p_minFilter
		* @param p_magFilter
		* @param p_generateMipmap
		* @param p_hdr
		*/
		static Texture* CreateFromMemory(
			uint8_t* p_data,
			uint32_t p_width,
			uint32_t p_height,
			OvRendering::Settings::ETextureFilteringMode p_minFilter,
			OvRendering::Settings::ETextureFilteringMode p_magFilter,
			bool p_generateMipmap,
			bool p_hdr = false
		);

		/**
		* Reload a texture from file
		* @param p_texture
		* @param p_filePath
		* @param p_minFilter
		* @param p_magFilter
		* @param p_generateMipmap
		* @param p_hdr
		*/
		static void Reload(
			Texture& p_texture,
			const std::string& p_filePath,
			OvRendering::Settings::ETextureFilteringMode p_minFilter,
			OvRendering::Settings::ETextureFilteringMode p_magFilter,
			bool p_generateMipmap,
			bool p_hdr
		);

		/**
		* Destroy a texture
		* @param p_textureInstance
		*/
		static bool Destroy(Texture*& p_textureInstance);
	};
}