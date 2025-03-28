/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include "OvRendering/Features/LightingRenderFeature.h"
#include "OvRendering/Core/CompositeRenderer.h"

OvRendering::Features::LightingRenderFeature::LightingRenderFeature(Core::CompositeRenderer& p_renderer, uint32_t p_bufferBindingPoint)
	: ARenderFeature(p_renderer), m_bufferBindingPoint(p_bufferBindingPoint)
{
	m_lightBuffer = std::make_unique<HAL::ShaderStorageBuffer>();
}

bool IsLightInFrustum(const OvRendering::Entities::Light& p_light, const OvRendering::Data::Frustum& p_frustum)
{
	const auto& position = p_light.transform->GetWorldPosition();
	const auto effectRange = p_light.GetEffectRange();

	// We always consider lights that have an +inf range (Not necessary to test if they are in frustum)
	const bool isOmniscientLight = std::isinf(effectRange);

	return
		isOmniscientLight ||
		p_frustum.SphereInFrustum(position.x, position.y, position.z, p_light.GetEffectRange());
}

void OvRendering::Features::LightingRenderFeature::OnBeginFrame(const Data::FrameDescriptor& p_frameDescriptor)
{
	OVASSERT(m_renderer.HasDescriptor<LightingDescriptor>(), "Cannot find LightingDescriptor attached to this renderer");

	auto& lightDescriptor = m_renderer.GetDescriptor<LightingDescriptor>();
	auto& frameDescriptor = m_renderer.GetFrameDescriptor();

	std::vector<OvMaths::FMatrix4> lightMatrices;
	lightMatrices.reserve(lightDescriptor.lights.size());

	auto frustum = lightDescriptor.frustumOverride ?
		lightDescriptor.frustumOverride :
		frameDescriptor.camera->GetLightFrustum();

	for (auto light : lightDescriptor.lights)
	{
		if (!frustum || IsLightInFrustum(light.get(), frustum.value()))
		{
			lightMatrices.push_back(light.get().GenerateMatrix());
		}
	}

	const auto lightMatricesView = std::span{ lightMatrices };

	if (m_lightBuffer->Allocate(lightMatricesView.size_bytes(), Settings::EAccessSpecifier::STREAM_DRAW))
	{
		m_lightBuffer->Upload(lightMatricesView.data());
	}

	m_lightBuffer->Bind(m_bufferBindingPoint);
}

void OvRendering::Features::LightingRenderFeature::OnEndFrame()
{
	m_lightBuffer->Unbind();
}
