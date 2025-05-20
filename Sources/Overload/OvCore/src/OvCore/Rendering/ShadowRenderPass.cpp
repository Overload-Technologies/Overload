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
	const auto shadowShader = OVSERVICE(OvCore::ResourceManagement::ShaderManager).GetResource(":Shaders\\ShadowFallback.ovfx");
	OVASSERT(shadowShader, "Cannot find the shadow shader");

	m_shadowMaterial.SetShader(shadowShader);
	// No need to update the material settings, as its generated state mask will be overridden anyway.
}

void OvCore::Rendering::ShadowRenderPass::Draw(OvRendering::Data::PipelineState p_pso)
{
	ZoneScoped;
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
					light.PrepareForShadowRendering(frameDescriptor);

					engineBufferRenderFeature.SetCamera(light.shadowCamera.value());

					light.shadowBuffer->Bind();
					m_renderer.SetViewport(0, 0, light.shadowMapResolution, light.shadowMapResolution);
					m_renderer.Clear(true, true, true);
					DrawShadows(pso, scene);
					light.shadowBuffer->Unbind();

					engineBufferRenderFeature.SetCamera(frameDescriptor.camera.value());
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
	OvCore::SceneSystem::Scene& p_scene
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

							// Generate the state mask for the target material, and override
							// its properties to ensure the shadow pass is rendered correctly.
							drawable.stateMask = targetMaterial.GenerateStateMask();
							drawable.stateMask.blendable = false; // The shadow pass should never use blending.
							drawable.stateMask.depthTest = true; // The shadow pass should always use depth test.
							drawable.stateMask.colorWriting = false; // The shadow pass should never write color.
							drawable.stateMask.depthWriting = true; // The shadow pass should always write depth.

							// No front/backface culling for shadow pass (aka: two-sided shadow pass).
							// A "two-sided" shadow pass setting could be added in the future to change this behavior.
							drawable.stateMask.frontfaceCulling = false;
							drawable.stateMask.backfaceCulling = false;

							drawable.featureSetOverride = targetMaterial.GetFeatures() + shadowPassName;

							drawable.AddDescriptor<EngineDrawableDescriptor>({
								modelMatrix,
								materialRenderer->GetUserMatrix()
							});

							m_renderer.DrawEntity(p_pso, drawable);
						}
					}
				}
			}
		}
	}
}
