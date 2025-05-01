/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include <OvCore/ECS/Components/CMaterialRenderer.h>
#include <OvCore/Global/ServiceLocator.h>
#include <OvCore/Rendering/EngineBufferRenderFeature.h>
#include <OvCore/Rendering/EngineDrawableDescriptor.h>
#include <OvCore/Rendering/ShadowRenderPass.h>
#include <OvCore/ResourceManagement/ShaderManager.h>

#include <OvRendering/Features/LightingRenderFeature.h>
#include <OvRendering/HAL/Profiling.h>

constexpr uint8_t kMaxShadowMaps = 1;

OvCore::Rendering::ShadowRenderPass::ShadowRenderPass(OvRendering::Core::CompositeRenderer& p_renderer) :
	OvRendering::Core::ARenderPass(p_renderer)
{
	const auto shadowShader = OVSERVICE(OvCore::ResourceManagement::ShaderManager).GetResource(":Shaders\\Shadow.ovfx");
	OVASSERT(shadowShader, "Cannot find the shadow shader");

	m_shadowMaterial.SetShader(shadowShader);
	m_shadowMaterial.SetFrontfaceCulling(false);
	m_shadowMaterial.SetBackfaceCulling(false);
}

void OvCore::Rendering::ShadowRenderPass::Draw(OvRendering::Data::PipelineState p_pso)
{
	TracyGpuZone("ShadowRenderPass");

	using namespace OvCore::Rendering;

	OVASSERT(m_renderer.HasDescriptor<SceneRenderer::SceneDescriptor>(), "Cannot find SceneDescriptor attached to this renderer");
	OVASSERT(m_renderer.HasFeature<OvCore::Rendering::EngineBufferRenderFeature>(), "Cannot find EngineBufferRenderFeature attached to this renderer");
	OVASSERT(m_renderer.HasDescriptor<OvRendering::Features::LightingRenderFeature::LightingDescriptor>(), "Cannot find LightingDescriptor attached to this renderer");

	auto& engineBufferRenderFeature = m_renderer.GetFeature<OvCore::Rendering::EngineBufferRenderFeature>();
	auto& lightingDescriptor = m_renderer.GetDescriptor<OvRendering::Features::LightingRenderFeature::LightingDescriptor>();

	auto& sceneDescriptor = m_renderer.GetDescriptor<SceneRenderer::SceneDescriptor>();
	auto& frameDescriptor = m_renderer.GetFrameDescriptor();
	auto& scene = sceneDescriptor.scene;

	auto pso = m_renderer.CreatePipelineState();

	uint8_t lightIndex = 0;

	for (auto lightReference : lightingDescriptor.lights)
	{
		auto& light = lightReference.get();

		if (light.castShadows)
		{
			if (lightIndex < kMaxShadowMaps)
			{
				if (light.type == OvRendering::Settings::ELightType::DIRECTIONAL)
				{
					light.UpdateShadowData(frameDescriptor.camera.value());
					const auto& lightSpaceMatrix = light.GetLightSpaceMatrix();
					const auto& shadowBuffer = light.GetShadowBuffer();
					shadowBuffer.Bind();
					m_renderer.SetViewport(0, 0, light.shadowMapResolution, light.shadowMapResolution);
					m_renderer.Clear(true, true, true);
					DrawShadows(pso, scene, lightSpaceMatrix);
					shadowBuffer.Unbind();
				}
				else
				{
					// Other light types not supported!
				}
			}
		}
	}

	if (auto output = frameDescriptor.outputBuffer)
	{
		output.value().Bind();
	}

	m_renderer.SetViewport(0, 0, frameDescriptor.renderWidth, frameDescriptor.renderHeight);
}

void OvCore::Rendering::ShadowRenderPass::DrawShadows(
	OvRendering::Data::PipelineState p_pso,
	OvCore::SceneSystem::Scene& p_scene,
	const OvMaths::FMatrix4& p_lightSpaceMatrix
)
{
	for (auto modelRenderer : p_scene.GetFastAccessComponents().modelRenderers)
	{
		auto& actor = modelRenderer->owner;

		if (actor.IsActive())
		{
			if (auto model = modelRenderer->GetModel())
			{
				if (auto materialRenderer = modelRenderer->owner.GetComponent<OvCore::ECS::Components::CMaterialRenderer>())
				{
					const auto& materials = materialRenderer->GetMaterials();
					const auto& modelMatrix = actor.transform.GetWorldMatrix();

					for (auto mesh : model->GetMeshes())
					{
						if (auto material = materials.at(mesh->GetMaterialIndex()); material && material->IsValid() && material->IsShadowCaster())
						{
							const std::string shadowPassName = "SHADOW_PASS";

							// If the material has shadow pass, use it, otherwise use the shadow fallback material
							auto& targetMaterial =
								material->SupportsFeature(shadowPassName) ?
								*material :
								m_shadowMaterial;

							OvRendering::Entities::Drawable drawable;
							drawable.mesh = *mesh;
							drawable.material = targetMaterial;
							// TODO: Use a custom state mask.
							// For instance, the shadow pass should never use blending.
							drawable.stateMask = targetMaterial.GenerateStateMask();
							drawable.featureSetOverride = { shadowPassName };
							drawable.AddDescriptor<EngineDrawableDescriptor>({
								modelMatrix,
								materialRenderer->GetUserMatrix()
							});

							targetMaterial.SetProperty("_LightSpaceMatrix", p_lightSpaceMatrix, true);

							m_renderer.DrawEntity(p_pso, drawable);
						}
					}
				}
			}
		}
	}
}
