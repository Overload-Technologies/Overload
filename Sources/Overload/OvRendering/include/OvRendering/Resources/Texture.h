/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include <cstdint>
#include <string>
#include <memory>

#include <OvRendering/Settings/ETextureFilteringMode.h>
#include <OvRendering/HAL/Texture.h>

namespace OvRendering::Resources
{
	namespace Loaders { class TextureLoader; }

	/**
	* Texture saved on the disk
	*/
	class Texture
	{
		friend class Loaders::TextureLoader;

	public:
		/**
		* Returns the associated HAL::Texture instance
		*/
		HAL::Texture& GetTexture();

	private:
		Texture(const std::string p_path, std::unique_ptr<HAL::Texture>&& p_texture);
		~Texture() = default;
		void SetTexture(std::unique_ptr<HAL::Texture>&& p_texture);

	public:
		const std::string path;

	private:
		std::unique_ptr<HAL::Texture> m_texture;
	};
}