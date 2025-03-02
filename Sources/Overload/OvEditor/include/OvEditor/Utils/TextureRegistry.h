/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include <span>
#include <unordered_map>

#include <OvRendering/HAL/Texture.h>
#include <OvTools/Utils/OptRef.h>

namespace OvEditor::Utils
{
	/**
	* Structure describing a texture registry entry.
	*/
	struct TextureRegistryEntryDesc
	{
		const uint32_t id;
		const OvRendering::HAL::Texture* texture;
	};

	/**
	* Class exposing tools to manage textures.
	*/
	class TextureRegistry
	{
	public:
		OvTools::Eventing::Event<const TextureRegistryEntryDesc&> textureAddedEvent;
		OvTools::Eventing::Event<const TextureRegistryEntryDesc&> textureRemovedEvent;

	public:
		/**
		* Creates the TextureRegistry.
		*/
		TextureRegistry();

		/**
		* Destroys the TextureRegistry.
		*/
		~TextureRegistry();

		/**
		* Returns the texture associated with the given id
		* @param p_id
		*/
		OvTools::Utils::OptRef<OvRendering::HAL::Texture> GetTexture(uint32_t p_id) const;

		/**
		* Returns all the texture ids
		*/
		std::span<const uint32_t> GetTextureIDs() const;

	private:
		OvTools::Eventing::ListenerID m_creationListenerID;
		OvTools::Eventing::ListenerID m_destructionListenerID;
		std::unordered_map<uint32_t, OvRendering::HAL::Texture*> m_textures;
		std::vector<uint32_t> m_quickAccessTextureIDs;
	};
}
