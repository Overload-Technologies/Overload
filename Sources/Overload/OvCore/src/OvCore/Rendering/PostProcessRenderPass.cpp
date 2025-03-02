/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include <OvCore/ECS/Components/CPostProcessStack.h>
#include <OvCore/Rendering/PostProcessRenderPass.h>
#include <OvCore/Global/ServiceLocator.h>
#include <OvCore/Rendering/FramebufferUtil.h>
#include <OvCore/Rendering/SceneRenderer.h>
#include <OvCore/ResourceManagement/ShaderManager.h>
#include <OvRendering/Core/CompositeRenderer.h>

OvCore::Rendering::PostProcessRenderPass::PostProcessRenderPass(OvRendering::Core::CompositeRenderer& p_renderer) :
	OvRendering::Core::ARenderPass(p_renderer),
	m_pingPongBuffers{
		OvRendering::HAL::Framebuffer{"PostProcessBlitPingPong0"},
		OvRendering::HAL::Framebuffer{"PostProcessBlitPingPong1"}
	}
{
	for (auto& buffer : m_pingPongBuffers)
	{
		OvCore::Rendering::FramebufferUtil::SetupFramebuffer(
			buffer, 1, 1, false, false, false
		);
	}

	m_blitMaterial.SetShader(OVSERVICE(OvCore::ResourceManagement::ShaderManager)[":Shaders\\PostProcess\\Blit.ovfx"]);

	// Instantiate available effects
	m_effects.reserve(4);
	m_effects.push_back(std::make_unique<OvCore::Rendering::PostProcess::BloomEffect>(p_renderer));
	m_effects.push_back(std::make_unique<OvCore::Rendering::PostProcess::AutoExposureEffect>(p_renderer));
	m_effects.push_back(std::make_unique<OvCore::Rendering::PostProcess::TonemappingEffect>(p_renderer));
	m_effects.push_back(std::make_unique<OvCore::Rendering::PostProcess::FXAAEffect>(p_renderer));
}

OvTools::Utils::OptRef<const OvCore::Rendering::PostProcess::PostProcessStack> FindPostProcessStack(OvCore::SceneSystem::Scene& p_scene)
{
	auto& postProcessStacks = p_scene.GetFastAccessComponents().postProcessStacks;

	for (auto postProcessStack : postProcessStacks)
	{
		if (postProcessStack && postProcessStack->owner.IsActive())
		{
			return postProcessStack->GetStack();
		}
	}

	return std::nullopt;
}

void OvCore::Rendering::PostProcessRenderPass::Draw(OvRendering::Data::PipelineState p_pso)
{
	auto& sceneDescriptor = m_renderer.GetDescriptor<OvCore::Rendering::SceneRenderer::SceneDescriptor>();
	auto& scene = sceneDescriptor.scene;

	if (auto stack = FindPostProcessStack(scene))
	{
		uint32_t passIndex = 0;

		auto& framebuffer = m_renderer.GetFrameDescriptor().outputBuffer.value();

		const uint64_t kPingPongBufferSize = m_pingPongBuffers.size();
		
		m_renderer.Blit(p_pso, framebuffer, m_pingPongBuffers[0], m_blitMaterial);

		for (auto& effect : m_effects)
		{
			const auto& settings = stack->Get(typeid(*effect));

			if (effect && effect->IsApplicable(settings))
			{
				effect->Draw(
					p_pso,
					m_pingPongBuffers[passIndex % kPingPongBufferSize],
					m_pingPongBuffers[(passIndex + 1) % kPingPongBufferSize],
					settings
				);

				++passIndex;
			}
		}

		const uint64_t lastIndex = passIndex % kPingPongBufferSize;

		m_renderer.Blit(p_pso, m_pingPongBuffers[lastIndex], framebuffer, m_blitMaterial);
	}
}
