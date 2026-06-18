/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include <OvRendering/Utils/ResourceTracking.h>

#include <unordered_map>

#include <baregl/debug/Debug.h>

#include <OvDebug/Assertion.h>

namespace OvRendering::Utils::ResourceTracking
{
	OvTools::Eventing::Event<const ResourceTracking::TextureRegistryEntryDesc&> TextureAddedEvent;
	OvTools::Eventing::Event<const ResourceTracking::TextureRegistryEntryDesc&> TextureRemovedEvent;
}

namespace
{
	std::unordered_map<uint32_t, const baregl::Texture*> m_textures;
	std::vector<uint32_t> m_quickAccessTextureIDs;

	class EventHandler : public baregl::debug::IEventHandler
	{
	public:
		virtual void OnBufferCreated([[maybe_unused]] const baregl::Buffer&) override {}
		virtual void OnBufferDestroyed([[maybe_unused]] const baregl::Buffer&) override {}
		virtual void OnFramebufferCreated([[maybe_unused]] const baregl::Framebuffer&) override {}
		virtual void OnFramebufferDestroyed([[maybe_unused]] const baregl::Framebuffer&) override {}
		virtual void OnRenderbufferCreated([[maybe_unused]] const baregl::Renderbuffer&) override {}
		virtual void OnRenderbufferDestroyed([[maybe_unused]] const baregl::Renderbuffer&) override {}
		virtual void OnShaderProgramCreated([[maybe_unused]] const baregl::ShaderProgram&) override {}
		virtual void OnShaderProgramDestroyed([[maybe_unused]] const baregl::ShaderProgram&) override {}
		virtual void OnShaderStageCreated([[maybe_unused]] const baregl::ShaderStage&) override {}
		virtual void OnShaderStageDestroyed([[maybe_unused]] const baregl::ShaderStage&) override {}
		virtual void OnVertexArrayCreated([[maybe_unused]] const baregl::VertexArray&) override {}
		virtual void OnVertexArrayDestroyed([[maybe_unused]] const baregl::VertexArray&) override {}

		virtual void OnTextureCreated(const baregl::Texture& p_texture) override
		{
			const auto id = p_texture.GetID();
			if (id != 0)
			{
				m_quickAccessTextureIDs.push_back(id);
				m_textures[id] = &p_texture;
				OvRendering::Utils::ResourceTracking::TextureAddedEvent.Invoke({ id, &p_texture });
			}
		}

		virtual void OnTextureDestroyed(const baregl::Texture& p_texture) override
		{
			const auto id = p_texture.GetID();
			if (id != 0)
			{
				m_quickAccessTextureIDs.erase(std::ranges::find(m_quickAccessTextureIDs, id));
				m_textures.erase(id);
				OvRendering::Utils::ResourceTracking::TextureRemovedEvent.Invoke({ id, &p_texture });
			}
		}
	};
}

namespace OvRendering::Utils::ResourceTracking
{
	void TrackResources()
	{
		baregl::debug::SetEventHandler(std::make_unique<EventHandler>());
	}

	OvTools::Utils::OptRef<const baregl::Texture> GetTexture(uint32_t p_id)
	{
		if (m_textures.contains(p_id))
		{
			return OvTools::Utils::OptRef<const baregl::Texture>(*m_textures.at(p_id));
		}

		return std::nullopt;
	}

	std::span<const uint32_t> GetTextureIDs()
	{
		return m_quickAccessTextureIDs;
	}
}
