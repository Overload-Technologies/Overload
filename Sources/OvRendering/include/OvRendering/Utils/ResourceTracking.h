/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include <span>

#include <baregl/Texture.h>
#include <OvTools/Utils/OptRef.h>
#include <OvTools/Eventing/Event.h>

namespace OvRendering::Utils::ResourceTracking
{
	/**
	* Structure describing a texture registry entry.
	*/
	struct TextureRegistryEntryDesc
	{
		const uint32_t id;
		const baregl::Texture* texture;
	};

	static OvTools::Eventing::Event<const TextureRegistryEntryDesc&> TextureAddedEvent;
	static OvTools::Eventing::Event<const TextureRegistryEntryDesc&> TextureRemovedEvent;

	/**
	* Start tracking resource events
	*/
	void TrackResources();

	/**
	* Returns the texture associated with the given id
	* @param p_id
	*/
	OvTools::Utils::OptRef<const baregl::Texture> GetTexture(uint32_t p_id);

	/**
	* Returns all the texture ids
	*/
	std::span<const uint32_t> GetTextureIDs();
}

