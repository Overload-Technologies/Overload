/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include <format>
#include <tinyxml2.h>

#include <OvCore/ECS/Actor.h>
#include <OvCore/ECS/Components/CReflectionProbe.h>
#include <OvDebug/Assertion.h>
#include <OvUI/Widgets/Selection/ComboBox.h>

namespace
{
	constexpr uint32_t kProbeDefaultResolution = 512;
}

OvCore::ECS::Components::CReflectionProbe::CReflectionProbe(ECS::Actor& p_owner) :
	AComponent(p_owner),
	m_resolution{ kProbeDefaultResolution },
	m_influenceSize{ 10.0f, 10.0f, 10.0f },
	m_influenceOffset{ 0.0f, 0.0f, 0.0f }
{
	m_framebuffer = std::make_unique<OvRendering::HAL::Framebuffer>(
		"ReflectionProbeFramebuffer"
	);

	_CreateCubemap();
}

std::string OvCore::ECS::Components::CReflectionProbe::GetName()
{
	return "Reflection Probe";
}

void OvCore::ECS::Components::CReflectionProbe::SetInfluenceSize(const OvMaths::FVector3& p_size)
{
	m_influenceSize = p_size;
}

const OvMaths::FVector3& OvCore::ECS::Components::CReflectionProbe::GetInfluenceSize() const
{
	return m_influenceSize;
}

void OvCore::ECS::Components::CReflectionProbe::SetInfluenceOffset(const OvMaths::FVector3& p_offset)
{
	m_influenceOffset = p_offset;
}

const OvMaths::FVector3& OvCore::ECS::Components::CReflectionProbe::GetInfluenceOffset() const
{
	return m_influenceOffset;
}

void OvCore::ECS::Components::CReflectionProbe::SetCubemapResolution(uint32_t p_resolution)
{
	OVASSERT(p_resolution > 0, "Cubemap resolution must be greater than 0");
	OVASSERT((p_resolution & (p_resolution - 1)) == 0 > 0, "Cubemap resolution must be a power of 2");

	if (p_resolution != m_resolution)
	{
		m_resolution = p_resolution;
		_CreateCubemap();
	}
}

uint32_t OvCore::ECS::Components::CReflectionProbe::GetCubemapResolution() const
{
	return m_resolution;
}

std::shared_ptr<OvRendering::HAL::Texture> OvCore::ECS::Components::CReflectionProbe::GetCubemap() const
{
	OVASSERT(m_cubemap != nullptr, "Cubemap is not initialized");
	return m_cubemap;
}

OvRendering::HAL::Framebuffer& OvCore::ECS::Components::CReflectionProbe::GetFramebuffer() const
{
	OVASSERT(m_framebuffer != nullptr, "Framebuffer is not initialized");
	return *m_framebuffer;
}

void OvCore::ECS::Components::CReflectionProbe::OnSerialize(tinyxml2::XMLDocument& p_doc, tinyxml2::XMLNode* p_node)
{
	using namespace OvCore::Helpers;
	Serializer::SerializeInt(p_doc, p_node, "resolution", m_resolution);
	Serializer::DeserializeVec3(p_doc, p_node, "influence_size", m_influenceSize);
	Serializer::DeserializeVec3(p_doc, p_node, "influence_offset", m_influenceOffset);
}

void OvCore::ECS::Components::CReflectionProbe::OnDeserialize(tinyxml2::XMLDocument& p_doc, tinyxml2::XMLNode* p_node)
{
	using namespace OvCore::Helpers;

	// Not ideal, but avoids garbage value from overriding the current resolution.
	if (p_node->FirstChildElement("resolution"))
	{
		m_resolution = Serializer::DeserializeInt(p_doc, p_node, "resolution");
	}

	Serializer::DeserializeVec3(p_doc, p_node, "influence_size", m_influenceSize);
	Serializer::DeserializeVec3(p_doc, p_node, "influence_offset", m_influenceOffset);
}

void OvCore::ECS::Components::CReflectionProbe::OnInspector(OvUI::Internal::WidgetContainer& p_root)
{
	using namespace OvCore::Helpers;
	Helpers::GUIDrawer::CreateTitle(p_root, "Cubemap Resolution");

	auto& cubemapResolution = p_root.CreateWidget<OvUI::Widgets::Selection::ComboBox>(m_resolution);
	cubemapResolution.choices = {
		{ 16, "16" },
		{ 32, "32" },
		{ 64, "64" },
		{ 128, "128" },
		{ 256, "256" },
		{ 512, "512" },
		{ 1024, "1024" },
		{ 2048, "2048" },
		{ 4096, "4096 (You're crazy!)" },
	};

	auto& cubemapResolutionDispatcher = cubemapResolution.AddPlugin<OvUI::Plugins::DataDispatcher<int>>();
	cubemapResolutionDispatcher.RegisterGatherer(std::bind(&CReflectionProbe::GetCubemapResolution, this));
	cubemapResolutionDispatcher.RegisterProvider(std::bind(&CReflectionProbe::SetCubemapResolution, this, std::placeholders::_1));

	Helpers::GUIDrawer::DrawVec3(
		p_root,
		"Influence Size",
		m_influenceSize
	);

	Helpers::GUIDrawer::DrawVec3(
		p_root,
		"Influence Offset",
		m_influenceOffset
	);
}

void OvCore::ECS::Components::CReflectionProbe::_CreateCubemap()
{
	m_cubemap = std::make_shared<OvRendering::HAL::Texture>(
		OvRendering::Settings::ETextureType::TEXTURE_CUBE,
		"ReflectionProbeCubemap"
	);

	m_cubemap->Allocate(
		OvRendering::Settings::TextureDesc{
			.width = m_resolution,
			.height = m_resolution,
			.minFilter = OvRendering::Settings::ETextureFilteringMode::LINEAR_MIPMAP_LINEAR,
			.magFilter = OvRendering::Settings::ETextureFilteringMode::LINEAR,
			.horizontalWrap = OvRendering::Settings::ETextureWrapMode::CLAMP_TO_EDGE,
			.verticalWrap = OvRendering::Settings::ETextureWrapMode::CLAMP_TO_EDGE,
			.internalFormat = OvRendering::Settings::EInternalFormat::RGBA32F,
			.useMipMaps = true
		}
	);

	for (uint32_t i = 0; i < 6; ++i)
	{
		m_framebuffer->Attach<OvRendering::HAL::Texture>(
			m_cubemap,
			OvRendering::Settings::EFramebufferAttachment::COLOR,
			i, // Each color attachment is a face of the cubemap
			i // Each face of the cubemap is accessed by its layer index
		);
	}

	m_framebuffer->Validate();
}
