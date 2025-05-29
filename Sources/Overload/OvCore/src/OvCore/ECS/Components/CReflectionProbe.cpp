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
		sizeof(float) +				// Brightness (float)
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
}

std::string OvCore::ECS::Components::CReflectionProbe::GetName()
{
	return "Reflection Probe";
}

void OvCore::ECS::Components::CReflectionProbe::SetRefreshMode(ERefreshMode p_mode)
{
	if (p_mode != m_refreshMode)
	{
		m_refreshMode = p_mode;
	}
}

OvCore::ECS::Components::CReflectionProbe::ERefreshMode OvCore::ECS::Components::CReflectionProbe::GetRefreshMode() const
{
	return m_refreshMode;
}

void OvCore::ECS::Components::CReflectionProbe::SetCaptureSpeed(ECaptureSpeed p_speed)
{
	const bool requiredDoubleBuffering = _IsDoubleBuffered();
	const bool willRequireDoubleBuffering = RequiresDoubleBuffering(p_speed);

	m_captureSpeed = p_speed;
	m_captureFaceIndex = 0; // Reset the capture face index, to make sure we start from the first face again.

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

void OvCore::ECS::Components::CReflectionProbe::SetCapturePosition(const OvMaths::FVector3& p_position)
{
	m_capturePosition = p_position;
}

const OvMaths::FVector3& OvCore::ECS::Components::CReflectionProbe::GetCapturePosition() const
{
	return m_capturePosition;
}

void OvCore::ECS::Components::CReflectionProbe::SetBrightness(float p_brightness)
{
	m_brightness = p_brightness;
}

float OvCore::ECS::Components::CReflectionProbe::GetBrightness() const
{
	return m_brightness;
}

void OvCore::ECS::Components::CReflectionProbe::SetCubemapResolution(uint32_t p_resolution)
{
	OVASSERT(p_resolution > 0, "Cubemap resolution must be greater than 0");
	OVASSERT((p_resolution & (p_resolution - 1)) == 0 > 0, "Cubemap resolution must be a power of 2");

	if (p_resolution != m_resolution)
	{
		m_resolution = p_resolution;
		_AllocateResources();
	}
}

uint32_t OvCore::ECS::Components::CReflectionProbe::GetCubemapResolution() const
{
	return m_resolution;
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

void OvCore::ECS::Components::CReflectionProbe::SetBoxProjection(bool p_enabled)
{
	m_boxProjection = p_enabled;
}

bool OvCore::ECS::Components::CReflectionProbe::IsBoxProjectionEnabled() const
{
	return m_boxProjection;
}

void OvCore::ECS::Components::CReflectionProbe::RequestCapture(bool p_forceImmediate)
{
	m_captureFaceIndex = 0;

	if (m_captureRequest.has_value() && m_captureRequest->forceImmediate)
	{
		// If a request already exists and is set to force immediate,
		// we ignore the new request.
		return;
	}

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
	Serializer::SerializeUint32(p_doc, p_node, "refresh_mode", static_cast<uint32_t>(m_refreshMode));
	Serializer::SerializeUint32(p_doc, p_node, "capture_speed", static_cast<uint32_t>(m_captureSpeed));
	Serializer::SerializeVec3(p_doc, p_node, "capture_position", m_capturePosition);
	Serializer::SerializeFloat(p_doc, p_node, "brightness", m_brightness);
	Serializer::SerializeUint32(p_doc, p_node, "resolution", m_resolution);
	Serializer::SerializeUint32(p_doc, p_node, "influence_policy", static_cast<uint32_t>(m_influencePolicy));
	Serializer::SerializeVec3(p_doc, p_node, "influence_size", m_influenceSize);
	Serializer::SerializeBoolean(p_doc, p_node, "box_projection", m_boxProjection);
}

void OvCore::ECS::Components::CReflectionProbe::OnDeserialize(tinyxml2::XMLDocument& p_doc, tinyxml2::XMLNode* p_node)
{
	using namespace OvCore::Helpers;

	// Not ideal, but avoids garbage value from overriding the current resolution.
	if (p_node->FirstChildElement("resolution"))
	{
		m_resolution = Serializer::DeserializeInt(p_doc, p_node, "resolution");
	}

	Serializer::DeserializeUint32(p_doc, p_node, "refresh_mode", reinterpret_cast<uint32_t&>(m_refreshMode));
	Serializer::DeserializeUint32(p_doc, p_node, "capture_speed", reinterpret_cast<uint32_t&>(m_captureSpeed));
	Serializer::DeserializeVec3(p_doc, p_node, "capture_position", m_capturePosition);
	Serializer::DeserializeFloat(p_doc, p_node, "brightness", m_brightness);
	Serializer::DeserializeUint32(p_doc, p_node, "resolution", m_resolution);
	Serializer::DeserializeUint32(p_doc, p_node, "influence_policy", reinterpret_cast<uint32_t&>(m_influencePolicy));
	Serializer::DeserializeVec3(p_doc, p_node, "influence_size", m_influenceSize);
	Serializer::DeserializeBoolean(p_doc, p_node, "box_projection", m_boxProjection);

	m_captureFaceIndex = 0;
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

	Helpers::GUIDrawer::DrawVec3(
		p_root,
		"Capture Position",
		m_capturePosition,
		0.05f
	);

	Helpers::GUIDrawer::DrawScalar<float>(
		p_root,
		"Brithgness",
		m_brightness,
		0.01f,
		0.0f
	);

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
	updateInfluenceWidgets(boxProjection, m_influencePolicy);

	influencePolicy.ValueChangedEvent += [&](int p_value) {
		const auto value = static_cast<EInfluencePolicy>(p_value);
		updateInfluenceWidgets(influenceSize, value);
		updateInfluenceWidgets(boxProjection, value);
	};

	auto& captureNowButton = p_root.CreateWidget<OvUI::Widgets::Buttons::Button>("Capture Now");
	captureNowButton.ClickedEvent += [this] {
		RequestCapture();
	};
}

void OvCore::ECS::Components::CReflectionProbe::_NotifyCubemapComplete()
{
	if (m_captureRequest)
	{
		m_captureRequest.reset(); // Discard the capture request after processing it.
	}

	// This is to keep track of whether any cubemap is complete.
	// Useful to ensure that we always have a valid cubemap to present.
	m_isAnyCubemapComplete = true;

	// Once the cubemap is complete, we can generate the mipmaps
	// that will be used to sample the average color of the cubemap (diffuse IBL).
	m_cubemapIterator[kBackBufferIndex]->GenerateMipmaps();

	// When using double buffering (i.e. progressive capture), we need to
	// update which buffer is the back buffer and which one is the complete buffer (presented to shaders).
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

	// This forces a capture on the next frame,
	// as we always want to have at least one valid cubemap.
	m_isAnyCubemapComplete = false;

	m_captureFaceIndex = 0;

	// Allocate 1 or 2 cubemaps (depending on double buffering)
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
}

void OvCore::ECS::Components::CReflectionProbe::_PrepareUBO()
{
	const auto& probePosition = owner.transform.GetWorldPosition() + m_capturePosition;
	const auto& boxPosition = owner.transform.GetWorldPosition();
	const auto& probeRotation = owner.transform.GetWorldRotation();
	const auto& probeRotationMatrix = OvMaths::FQuaternion::ToMatrix3(OvMaths::FQuaternion::Normalize(probeRotation));

	// UBO is padding sensitive, so we need to ensure proper alignment.
#pragma pack(push, 1)
	struct
	{
		OvMaths::FVector4 position;
		OvMaths::FMatrix4 rotation;
		OvMaths::FVector4 boxCenter;
		OvMaths::FVector4 boxHalfExtents;
		float brightness;
		bool boxProjection;
		std::byte padding[3];
	} uboDataPage{ 
		.position = probePosition,
		.rotation = probeRotationMatrix,
		.boxCenter = boxPosition,
		.boxHalfExtents = m_influenceSize,
		.brightness = m_brightness,
		.boxProjection = m_boxProjection && m_influencePolicy == EInfluencePolicy::LOCAL,
	};
#pragma pack(pop)

	static_assert(sizeof(uboDataPage) == kUBOSize, "UBO data size mismatch");

	m_uniformBuffer->Upload(&uboDataPage);
}

std::vector<uint32_t> OvCore::ECS::Components::CReflectionProbe::_GetCaptureFaceIndices()
{
	auto targetFaceCount = [&]() -> uint32_t {
		const bool captureRequested = m_captureRequest.has_value();
		const bool immediateCaptureRequested =
			(captureRequested && m_captureRequest->forceImmediate) ||
			// Always make sure to capture all faces immediately when no cubemap is complete,
			// unless we are in "on demand" mode.
			(m_refreshMode != ERefreshMode::ON_DEMAND && !m_isAnyCubemapComplete);

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

	if (targetFaceCount == 0)
	{
		return {}; // No faces to capture
	}

	const auto lastIndex = m_captureFaceIndex + (targetFaceCount - 1);

	OVASSERT(
		lastIndex < 6,
		"We shouldn't request more than one cubemap to be modified at a time."
		// Expected indices are:
		// [0, 1, 2, 3, 4, 5] for 6 faces per frame,
		// [0, 1, 2], [3, 4, 5] for 3 faces per frame,
		// [0, 1], [2, 3], [4, 5] for 2 faces per frame,
		// [0], [1], [2], [3], [4], [5] for 1 face per frame.
	);

	std::vector<uint32_t> faceIndices;
	faceIndices.reserve(targetFaceCount);

	for (uint32_t i = m_captureFaceIndex; i <= lastIndex; ++i)
	{
		faceIndices.push_back(i);
	}

	// Update the index for the next capture
	m_captureFaceIndex = (lastIndex + 1) % 6;

	return faceIndices;
}

OvRendering::HAL::Framebuffer& OvCore::ECS::Components::CReflectionProbe::_GetTargetFramebuffer() const
{
	auto& framebuffer =
		_IsDoubleBuffered() ?
		m_framebuffers[kBackBufferIndex] :
		m_framebuffers[0];

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
