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

namespace
{
	constexpr size_t kUBOSize =
		sizeof(OvMaths::FVector4) +	// Position (vec3)
		sizeof(OvMaths::FMatrix4) +	// Rotation (mat3)
		sizeof(OvMaths::FVector4) +	// Box Center (vec3)
		sizeof(OvMaths::FVector4) +	// Box Extents (vec3)
		sizeof(int);				// Box Projection (bool)

	constexpr uint32_t kBackBufferIndex = 0; // Cubemap that is being rendered
	constexpr uint32_t kCompleteBufferIndex = 1; // Cubemap that is used

	bool RequiresDoubleBuffering(OvCore::ECS::Components::CReflectionProbe::ECaptureSpeed p_speed)
	{
		return p_speed != OvCore::ECS::Components::CReflectionProbe::ECaptureSpeed::SIX_FACES;
	}
}

OvCore::ECS::Components::CReflectionProbe::CReflectionProbe(ECS::Actor& p_owner) :
	AComponent(p_owner),
	m_framebuffers{ "ReflectionProbeFramebuffer" },
	m_cubemapIterator{ m_cubemaps }
{
	m_uniformBuffer = std::make_unique<OvRendering::HAL::UniformBuffer>();
	m_uniformBuffer->Allocate(kUBOSize, OvRendering::Settings::EAccessSpecifier::STREAM_DRAW);

	_AllocateResources();

	if (m_refreshMode == ERefreshMode::ONCE)
	{
		RequestCapture();
	}
}

std::string OvCore::ECS::Components::CReflectionProbe::GetName()
{
	return "Reflection Probe";
}

void OvCore::ECS::Components::CReflectionProbe::SetCaptureSpeed(ECaptureSpeed p_speed)
{
	const bool requiredDoubleBuffering = _IsDoubleBuffered();
	const bool willRequireDoubleBuffering = RequiresDoubleBuffering(p_speed);

	m_captureSpeed = p_speed;

	// Progressive uses double buffering, while immediate (6 face per frame) doesn't.
	// This makes sure that the proper resources are allocated for
	// the given refresh mode.
	if (requiredDoubleBuffering != willRequireDoubleBuffering)
	{
		_AllocateResources();
	}
}

OvCore::ECS::Components::CReflectionProbe::ECaptureSpeed OvCore::ECS::Components::CReflectionProbe::GetCaptureSpeed() const
{
	return m_captureSpeed;
}

void OvCore::ECS::Components::CReflectionProbe::SetRefreshMode(ERefreshMode p_mode)
{
	if (p_mode != m_refreshMode)
	{
		m_refreshMode = p_mode;

		// If the refresh mode is set to ONCE, we request a capture.
		if (m_refreshMode == ERefreshMode::ONCE)
		{
			RequestCapture();
		}

		_AllocateResources();
	}
}

OvCore::ECS::Components::CReflectionProbe::ERefreshMode OvCore::ECS::Components::CReflectionProbe::GetRefreshMode() const
{
	return m_refreshMode;
}

void OvCore::ECS::Components::CReflectionProbe::SetInfluencePolicy(EInfluencePolicy p_policy)
{
	m_influencePolicy = p_policy;
}

OvCore::ECS::Components::CReflectionProbe::EInfluencePolicy OvCore::ECS::Components::CReflectionProbe::GetInfluencePolicy() const
{
	return m_influencePolicy;
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

void OvCore::ECS::Components::CReflectionProbe::SetBoxProjection(bool p_enabled)
{
	m_boxProjection = p_enabled;
}

bool OvCore::ECS::Components::CReflectionProbe::IsBoxProjectionEnabled() const
{
	return m_boxProjection;
}

void OvCore::ECS::Components::CReflectionProbe::SetCubemapResolution(uint32_t p_resolution)
{
	OVASSERT(p_resolution > 0, "Cubemap resolution must be greater than 0");
	OVASSERT((p_resolution & (p_resolution - 1)) == 0 > 0, "Cubemap resolution must be a power of 2");

	if (p_resolution != m_resolution)
	{
		m_resolution = p_resolution;
		_AllocateResources();
		RequestCapture();
	}
}

uint32_t OvCore::ECS::Components::CReflectionProbe::GetCubemapResolution() const
{
	return m_resolution;
}

void OvCore::ECS::Components::CReflectionProbe::RequestCapture(bool p_forceImmediate)
{
	m_captureFaceIndex = 0;

	m_captureRequest = {
		.forceImmediate = p_forceImmediate
	};
}

std::shared_ptr<OvRendering::HAL::Texture> OvCore::ECS::Components::CReflectionProbe::GetCubemap() const
{
	const auto& target =
		_IsDoubleBuffered() ?
		m_cubemapIterator[kCompleteBufferIndex] :
		m_cubemaps[0];

	OVASSERT(target != nullptr, "Cubemap is not initialized");
	return target;
}

void OvCore::ECS::Components::CReflectionProbe::OnSerialize(tinyxml2::XMLDocument& p_doc, tinyxml2::XMLNode* p_node)
{
	using namespace OvCore::Helpers;
	Serializer::SerializeInt(p_doc, p_node, "resolution", m_resolution);
	Serializer::SerializeUint32(p_doc, p_node, "influence_policy", static_cast<uint32_t>(m_influencePolicy));
	Serializer::SerializeVec3(p_doc, p_node, "influence_size", m_influenceSize);
	Serializer::SerializeVec3(p_doc, p_node, "influence_offset", m_influenceOffset);
	Serializer::SerializeBoolean(p_doc, p_node, "box_projection", m_boxProjection);
	Serializer::SerializeUint32(p_doc, p_node, "refresh_mode", static_cast<uint32_t>(m_refreshMode));
	Serializer::SerializeUint32(p_doc, p_node, "capture_speed", static_cast<uint32_t>(m_captureSpeed));
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

	Serializer::DeserializeUint32(p_doc, p_node, "influence_policy", reinterpret_cast<uint32_t&>(m_influencePolicy));
	Serializer::DeserializeVec3(p_doc, p_node, "influence_size", m_influenceSize);
	Serializer::DeserializeVec3(p_doc, p_node, "influence_offset", m_influenceOffset);
	Serializer::DeserializeBoolean(p_doc, p_node, "box_projection", m_boxProjection);
	Serializer::DeserializeUint32(p_doc, p_node, "refresh_mode", reinterpret_cast<uint32_t&>(m_refreshMode));
	Serializer::DeserializeUint32(p_doc, p_node, "capture_speed", reinterpret_cast<uint32_t&>(m_captureSpeed));

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
		{ static_cast<int>(ERefreshMode::ONCE), "Once" },
		{ static_cast<int>(ERefreshMode::ON_DEMAND), "On Demand" }
	};

	auto& refreshModeDispatcher = refreshMode.AddPlugin<OvUI::Plugins::DataDispatcher<int>>();
	refreshModeDispatcher.RegisterGatherer([this] { return static_cast<int>(GetRefreshMode()); });
	refreshModeDispatcher.RegisterProvider([this](int mode) { SetRefreshMode(static_cast<ERefreshMode>(mode)); });

	Helpers::GUIDrawer::CreateTitle(p_root, "Capture Speed");

	auto& progressiveFacesPerFrame = p_root.CreateWidget<OvUI::Widgets::Selection::ComboBox>(static_cast<int>(m_captureSpeed));
	progressiveFacesPerFrame.choices = {
		{ 1, "Low (1 face/frame)" },
		{ 2, "Medium (2 faces/frame)" },
		{ 3, "High (3 faces/frame)" },
		{ 6, "Immediate (6 faces/frame)" },
	};

	auto& progressiveFacesPerFrameDispatcher = progressiveFacesPerFrame.AddPlugin<OvUI::Plugins::DataDispatcher<int>>();
	progressiveFacesPerFrameDispatcher.RegisterGatherer([this] { return static_cast<int>(GetCaptureSpeed()); });
	progressiveFacesPerFrameDispatcher.RegisterProvider([this](int mode) { SetCaptureSpeed(static_cast<ECaptureSpeed>(mode)); });

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

	Helpers::GUIDrawer::CreateTitle(p_root, "Influence Policy");

	auto& influencePolicy = p_root.CreateWidget<OvUI::Widgets::Selection::ComboBox>(static_cast<int>(m_influencePolicy));
	influencePolicy.choices = {
		{ static_cast<int>(EInfluencePolicy::GLOBAL), "Global" },
		{ static_cast<int>(EInfluencePolicy::LOCAL), "Local" }
	};

	auto& influencePolicyDispatcher = influencePolicy.AddPlugin<OvUI::Plugins::DataDispatcher<int>>();
	influencePolicyDispatcher.RegisterGatherer([this] { return static_cast<int>(m_influencePolicy); });
	influencePolicyDispatcher.RegisterProvider([this](int policy) { m_influencePolicy = static_cast<EInfluencePolicy>(policy); });

	Helpers::GUIDrawer::DrawVec3(
		p_root,
		"Influence Size",
		m_influenceSize,
		0.05f,
		0.0f
	);

	auto& influenceSize = *p_root.GetWidgets().back().first;

	Helpers::GUIDrawer::DrawVec3(
		p_root,
		"Influence Offset",
		m_influenceOffset,
		0.05f
	);

	auto& influenceOffset = *p_root.GetWidgets().back().first;

	Helpers::GUIDrawer::DrawBoolean(
		p_root,
		"Box Projection",
		m_boxProjection
	);

	auto& boxProjection = *p_root.GetWidgets().back().first;

	auto updateInfluenceWidgets = [](auto& widget, auto policy) {
		widget.disabled = policy == OvCore::ECS::Components::CReflectionProbe::EInfluencePolicy::GLOBAL;
	};

	updateInfluenceWidgets(influenceSize, m_influencePolicy);
	updateInfluenceWidgets(influenceOffset, m_influencePolicy);
	updateInfluenceWidgets(boxProjection, m_influencePolicy);

	influencePolicy.ValueChangedEvent += [&](int p_value) {
		const auto value = static_cast<EInfluencePolicy>(p_value);
		updateInfluenceWidgets(influenceSize, value);
		updateInfluenceWidgets(influenceOffset, value);
		updateInfluenceWidgets(boxProjection, value);
	};

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

void OvCore::ECS::Components::CReflectionProbe::_NotifyCubemapComplete()
{
	if (m_captureRequest)
	{
		m_captureRequest.reset();
	}

	m_isAnyCubemapComplete = true;

	m_cubemapIterator[kBackBufferIndex]->GenerateMipmaps();

	if (_IsDoubleBuffered())
	{
		++m_cubemapIterator;
		++m_framebuffers;
	}
}

void OvCore::ECS::Components::CReflectionProbe::_AllocateResources()
{
	const uint8_t cubemapCount = _IsDoubleBuffered() ? 2 : 1;

	// Reset the ping pong iterators
	m_framebuffers.Reset();
	m_cubemapIterator.Reset();
	m_isAnyCubemapComplete = false;

	for (uint8_t i = 0; i < cubemapCount; ++i)
	{
		auto& cubemap = m_cubemaps[i];
		cubemap = std::make_shared<OvRendering::HAL::Texture>(
			OvRendering::Settings::ETextureType::TEXTURE_CUBE,
			"ReflectionProbeCubemap"
		);

		cubemap->Allocate(
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

		for (uint32_t faceIndex = 0; faceIndex < 6; ++faceIndex)
		{
			m_framebuffers[i].Attach<OvRendering::HAL::Texture>(
				cubemap,
				OvRendering::Settings::EFramebufferAttachment::COLOR,
				faceIndex, // Each color attachment is a face of the cubemap
				faceIndex // Each face of the cubemap is accessed by its layer index
			);
		}

		// Depth buffer
		const auto renderbuffer = std::make_shared<OvRendering::HAL::Renderbuffer>();
		const auto internalFormat = OvRendering::Settings::EInternalFormat::DEPTH_COMPONENT;
		renderbuffer->Allocate(m_resolution, m_resolution, internalFormat);
		m_framebuffers[i].Attach(renderbuffer, OvRendering::Settings::EFramebufferAttachment::DEPTH);

		// Validation
		m_framebuffers[i].Validate();
	}

	// When double buffering is enabled, we want to make sure that at least one
	// complete cubemap is ready to be used at all times.
	if (_IsDoubleBuffered())
	{
		RequestCapture();
	}
}

void OvCore::ECS::Components::CReflectionProbe::_PrepareUBO()
{
	const auto& probePosition = owner.transform.GetWorldPosition();
	const auto& boxPosition = probePosition + m_influenceOffset;
	const auto& probeRotation = owner.transform.GetWorldRotation();
	const auto& probeRotationMatrix = OvMaths::FQuaternion::ToMatrix3(OvMaths::FQuaternion::Normalize(probeRotation));

	// UBO is padding sensitive, so we need to ensure proper alignment.
#pragma pack(push, 1)
	struct
	{
		OvMaths::FVector4 position;
		OvMaths::FMatrix4 rotation;
		OvMaths::FVector4 boxCenter;
		OvMaths::FVector4 boxExtents;
		bool boxProjection;
		std::byte padding[3];
	} uboDataPage{ 
		.position = probePosition,
		.rotation = probeRotationMatrix,
		.boxCenter = boxPosition,
		.boxExtents = m_influenceSize,
		.boxProjection = m_boxProjection && m_influencePolicy == EInfluencePolicy::LOCAL,
	};
#pragma pack(pop)

	static_assert(sizeof(uboDataPage) == kUBOSize, "UBO data size mismatch");

	m_uniformBuffer->Upload(&uboDataPage);
}

std::vector<uint32_t> OvCore::ECS::Components::CReflectionProbe::_GetCaptureFaceIndices()
{
	const bool immediateCaptureRequested =
		(m_captureRequest.has_value() && m_captureRequest->forceImmediate) ||
		// If no cubemap is complete, we need to capture all faces immediately,
		// so we at least have one valid cubemap to present.
		!m_isAnyCubemapComplete;

	auto targetFaceCount = [&]() -> uint32_t {
		const bool captureRequested = m_captureRequest.has_value();
		const bool immediateCaptureRequested =
			(captureRequested && m_captureRequest->forceImmediate) ||
			// If no cubemap is complete, we need to capture all faces immediately,
			// so we at least have one valid cubemap to present.
			!m_isAnyCubemapComplete;

		if (immediateCaptureRequested)
		{
			return 6; // Capture all faces immediately
		}
		
		if (captureRequested || m_refreshMode == ERefreshMode::REALTIME)
		{
			return static_cast<uint32_t>(m_captureSpeed);
		}

		return 0;
	}();

	std::vector<uint32_t> faceIndices;
	faceIndices.reserve(targetFaceCount);
	for (uint32_t i = m_captureFaceIndex; i < m_captureFaceIndex + targetFaceCount; ++i)
	{
		faceIndices.push_back(i % 6); // Cycle through faces 0 to 5
	}
	m_captureFaceIndex = (m_captureFaceIndex + targetFaceCount) % 6; // Update the index for the next capture
	return faceIndices;
}

OvRendering::HAL::Framebuffer& OvCore::ECS::Components::CReflectionProbe::_GetTargetFramebuffer() const
{
	auto& framebuffer = m_framebuffers[kBackBufferIndex];
	OVASSERT(framebuffer.IsValid(), "Framebuffer is invalid");
	return framebuffer;
}

OvRendering::HAL::UniformBuffer& OvCore::ECS::Components::CReflectionProbe::_GetUniformBuffer() const
{
	OVASSERT(m_uniformBuffer != nullptr, "Uniform buffer is not initialized");
	return *m_uniformBuffer;
}

bool OvCore::ECS::Components::CReflectionProbe::_IsDoubleBuffered() const
{
	return RequiresDoubleBuffering(m_captureSpeed);
}
