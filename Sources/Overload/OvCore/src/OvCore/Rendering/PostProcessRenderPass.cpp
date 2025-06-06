/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include <tracy/Tracy.hpp>

#include <OvCore/ECS/Components/CPostProcessStack.h>
#include <OvCore/Rendering/PostProcessRenderPass.h>
#include <OvCore/Global/ServiceLocator.h>
#include <OvCore/Rendering/FramebufferUtil.h>
#include <OvCore/Rendering/SceneRenderer.h>
#include <OvCore/ResourceManagement/ShaderManager.h>

#include <OvRendering/Core/CompositeRenderer.h>
#include <OvRendering/HAL/Profiling.h>

OvCore::Rendering::PostProcessRenderPass::PostProcessRenderPass(OvRendering::Core::CompositeRenderer& p_renderer) :
	OvRendering::Core::ARenderPass(p_renderer),
	m_pingPongBuffers{ "PostProcessBlit" }
{
	for (auto& buffer : m_pingPongBuffers.GetFramebuffers())
	{
		OvCore::Rendering::FramebufferUtil::SetupFramebuffer(
			buffer, 1, 1, false, false, false
		);
	}

	m_blitMaterial.SetShader(OVSERVICE(OvCore::ResourceManagement::ShaderManager)[":Shaders\\PostProcess\\Blit.ovfx"]);

	// Instantiate available effects
	m_effects.reserve(4);
	m_effects.push_back(std::make_unique<OvCore::Rendering::PostProcess::AutoExposureEffect>(p_renderer));
	m_effects.push_back(std::make_unique<OvCore::Rendering::PostProcess::BloomEffect>(p_renderer));
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
	ZoneScoped;
	TracyGpuZone("PostProcessRenderPass");

	auto& sceneDescriptor = m_renderer.GetDescriptor<OvCore::Rendering::SceneRenderer::SceneDescriptor>();
	auto& scene = sceneDescriptor.scene;

	if (auto stack = FindPostProcessStack(scene))
	{
		auto& framebuffer = m_renderer.GetFrameDescriptor().outputBuffer.value();

		m_renderer.Blit(p_pso, framebuffer, m_pingPongBuffers[0], m_blitMaterial);

		for (auto& effect : m_effects)
		{
			const auto& settings = stack->Get(typeid(*effect));

			if (effect && effect->IsApplicable(settings))
			{
				effect->Draw(
					p_pso,
					m_pingPongBuffers[0],
					m_pingPongBuffers[1],
					settings
				);

				++m_pingPongBuffers;
			}
		}

		m_renderer.Blit(p_pso, m_pingPongBuffers[0], framebuffer, m_blitMaterial);
	}
}
