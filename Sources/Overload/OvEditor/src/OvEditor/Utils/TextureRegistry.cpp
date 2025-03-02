/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include <OvEditor/Utils/TextureRegistry.h>

namespace OvEditor::Utils
{
	TextureRegistry::TextureRegistry()
	{
		using namespace OvRendering::HAL;

		m_creationListenerID = Texture::CreationEvent += [this](Texture& p_texture)
		{
			const auto id = p_texture.GetID();
			if (id != 0)
			{
				m_quickAccessTextureIDs.push_back(id);
				m_textures[id] = &p_texture;
				textureAddedEvent.Invoke({ id, &p_texture });
			}
		};

		m_destructionListenerID = Texture::DestructionEvent += [this](Texture& p_texture)
		{
			const auto id = p_texture.GetID();
			if (id != 0)
			{
				m_quickAccessTextureIDs.erase(std::ranges::find(m_quickAccessTextureIDs, id));
				m_textures.erase(id);
				textureRemovedEvent.Invoke({ id, &p_texture });
			}
		};
	}

	TextureRegistry::~TextureRegistry()
	{
		OvRendering::HAL::Texture::CreationEvent -= m_creationListenerID;
		OvRendering::HAL::Texture::DestructionEvent -= m_destructionListenerID;
	}

	OvTools::Utils::OptRef<OvRendering::HAL::Texture> TextureRegistry::GetTexture(uint32_t p_id) const
	{
		if (m_textures.contains(p_id))
		{
			return OvTools::Utils::OptRef<OvRendering::HAL::Texture>(*m_textures.at(p_id));
		}

		return std::nullopt;
	}

	std::span<const uint32_t> TextureRegistry::GetTextureIDs() const
	{
		return m_quickAccessTextureIDs;
	}
}
