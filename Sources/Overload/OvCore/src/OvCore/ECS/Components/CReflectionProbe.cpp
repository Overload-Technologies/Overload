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
#include <OvRendering/HAL/Renderbuffer.h>
#include <OvUI/Widgets/Selection/ComboBox.h>
#include <OvUI/Widgets/Buttons/Button.h>

OvCore::ECS::Components::CReflectionProbe::CReflectionProbe(ECS::Actor& p_owner) : AComponent(p_owner)
{
	m_framebuffer = std::make_unique<OvRendering::HAL::Framebuffer>(
		"ReflectionProbeFramebuffer"
	);

	_CreateCubemap();

	if (m_refreshMode == ERefreshMode::ONCE)
	{
		RequestCapture();
	}
}

std::string OvCore::ECS::Components::CReflectionProbe::GetName()
{
	return "Reflection Probe";
}

void OvCore::ECS::Components::CReflectionProbe::SetRefreshMode(ERefreshMode p_mode)
{
	m_refreshMode = p_mode;
}

OvCore::ECS::Components::CReflectionProbe::ERefreshMode OvCore::ECS::Components::CReflectionProbe::GetRefreshMode() const
{
	return m_refreshMode;
}

void OvCore::ECS::Components::CReflectionProbe::SetCaptureDynamicObjects(bool p_capture)
{
	m_captureDynamicObjects = p_capture;
}

bool OvCore::ECS::Components::CReflectionProbe::GetCaptureDynamicObjects() const
{
	return m_captureDynamicObjects;
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

void OvCore::ECS::Components::CReflectionProbe::RequestCapture()
{
	m_captureRequested = true;
}

void OvCore::ECS::Components::CReflectionProbe::OnSerialize(tinyxml2::XMLDocument& p_doc, tinyxml2::XMLNode* p_node)
{
	using namespace OvCore::Helpers;
	Serializer::SerializeInt(p_doc, p_node, "resolution", m_resolution);
	Serializer::SerializeVec3(p_doc, p_node, "influence_size", m_influenceSize);
	Serializer::SerializeVec3(p_doc, p_node, "influence_offset", m_influenceOffset);
	Serializer::SerializeBoolean(p_doc, p_node, "capture_dynamic_objects", m_captureDynamicObjects);
	Serializer::SerializeInt(p_doc, p_node, "refresh_mode", static_cast<int>(m_refreshMode));
}

void OvCore::ECS::Components::CReflectionProbe::OnDeserialize(tinyxml2::XMLDocument& p_doc, tinyxml2::XMLNode* p_node)
{
	using namespace OvCore::Helpers;

	// Not ideal, but avoids garbage value from overriding the current resolution.
	if (p_node->FirstChildElement("resolution"))
	{
		m_resolution = Serializer::DeserializeInt(p_doc, p_node, "resolution");
	}

	const auto previousRefreshMode = m_refreshMode;

	Serializer::DeserializeVec3(p_doc, p_node, "influence_size", m_influenceSize);
	Serializer::DeserializeVec3(p_doc, p_node, "influence_offset", m_influenceOffset);
	Serializer::DeserializeBoolean(p_doc, p_node, "capture_dynamic_objects", m_captureDynamicObjects);
	Serializer::DeserializeInt(p_doc, p_node, "refresh_mode", reinterpret_cast<int&>(m_refreshMode));

	// If the refresh mode is set to ONCE, we request a capture.
	if (m_refreshMode == ERefreshMode::ONCE && previousRefreshMode != m_refreshMode)
	{
		RequestCapture();
	}
}

void OvCore::ECS::Components::CReflectionProbe::OnInspector(OvUI::Internal::WidgetContainer& p_root)
{
	using namespace OvCore::Helpers;

	Helpers::GUIDrawer::CreateTitle(p_root, "Refresh Mode");

	auto& refreshMode = p_root.CreateWidget<OvUI::Widgets::Selection::ComboBox>(static_cast<int>(m_refreshMode));
	refreshMode.choices = {
		{ static_cast<int>(ERefreshMode::REALTIME), "Realtime" },
		{ static_cast<int>(ERefreshMode::ONCE), "Once)" },
		{ static_cast<int>(ERefreshMode::MANUAL), "Manual" }
	};

	auto& refreshModeDispatcher = refreshMode.AddPlugin<OvUI::Plugins::DataDispatcher<int>>();
	refreshModeDispatcher.RegisterGatherer([this] { return static_cast<int>(m_refreshMode); });
	refreshModeDispatcher.RegisterProvider([this](int mode) { m_refreshMode = static_cast<ERefreshMode>(mode); });

	Helpers::GUIDrawer::DrawBoolean(
		p_root,
		"Capture Dynamic Objects",
		m_captureDynamicObjects
	);

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

	auto& captureNowButton = p_root.CreateWidget<OvUI::Widgets::Buttons::Button>("Capture Now");
	captureNowButton.ClickedEvent += [this] {
		RequestCapture();
	};
}

void OvCore::ECS::Components::CReflectionProbe::OnEnable()
{
	if (m_refreshMode == ERefreshMode::ONCE)
	{
		RequestCapture();
	}
}

bool OvCore::ECS::Components::CReflectionProbe::_IsCaptureRequested() const
{
	return m_captureRequested;
}

void OvCore::ECS::Components::CReflectionProbe::_MarkCaptureRequestComplete()
{
	m_captureRequested = false;
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

	// Depth buffer
	const auto renderbuffer = std::make_shared<OvRendering::HAL::Renderbuffer>();
	const auto internalFormat = OvRendering::Settings::EInternalFormat::DEPTH_COMPONENT;
	renderbuffer->Allocate(m_resolution, m_resolution, internalFormat);
	m_framebuffer->Attach(renderbuffer, OvRendering::Settings::EFramebufferAttachment::DEPTH);

	m_framebuffer->Validate();
}

std::shared_ptr<OvRendering::HAL::Texture> OvCore::ECS::Components::CReflectionProbe::_GetCubemap() const
{
	OVASSERT(m_cubemap != nullptr, "Cubemap is not initialized");
	return m_cubemap;
}

OvRendering::HAL::Framebuffer& OvCore::ECS::Components::CReflectionProbe::_GetFramebuffer() const
{
	OVASSERT(m_framebuffer != nullptr, "Framebuffer is not initialized");
	return *m_framebuffer;
}
