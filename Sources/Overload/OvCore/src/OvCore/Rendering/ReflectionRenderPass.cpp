/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include <OvCore/ECS/Components/CMaterialRenderer.h>
#include <OvCore/Global/ServiceLocator.h>
#include <OvCore/Rendering/EngineBufferRenderFeature.h>
#include <OvCore/Rendering/EngineDrawableDescriptor.h>
#include <OvCore/Rendering/ReflectionRenderFeature.h>
#include <OvCore/Rendering/ReflectionRenderPass.h>
#include <OvCore/Rendering/SceneRenderer.h>
#include <OvCore/ResourceManagement/ShaderManager.h>

#include <OvRendering/HAL/Profiling.h>

namespace
{
	const OvMaths::FVector3 kCubeFaceRotations[6] = {
		{ 0.0f, -90.0f, 180.0f },    // (Right)
		{ 0.0f, 90.0f, 180.0f },   // (Left)
		{ 90.0f, 0.0f, 180.0f },   // (Top)
		{ -90.0f, 0.0f, 180.0f },    // (Bottom)
		{ 0.0f, 0.0f, 180.0f },     // (Front)
		{ 0.0f, -180.0f, 180.0f }    // (Back)
	};
}

OvCore::Rendering::ReflectionRenderPass::ReflectionRenderPass(OvRendering::Core::CompositeRenderer& p_renderer) :
	OvRendering::Core::ARenderPass(p_renderer)
{
}

void OvCore::Rendering::ReflectionRenderPass::Draw(OvRendering::Data::PipelineState p_pso)
{
	ZoneScoped;
	TracyGpuZone("ReflectionRenderPass");

	using namespace OvCore::Rendering;

	OVASSERT(m_renderer.HasDescriptor<SceneRenderer::SceneDescriptor>(), "Cannot find SceneDescriptor attached to this renderer");
	OVASSERT(m_renderer.HasFeature<OvCore::Rendering::EngineBufferRenderFeature>(), "Cannot find EngineBufferRenderFeature attached to this renderer");
	OVASSERT(m_renderer.HasDescriptor<OvCore::Rendering::ReflectionRenderFeature::ReflectionDescriptor>(), "Cannot find ReflectionDescriptor attached to this renderer");

	auto& engineBufferRenderFeature = m_renderer.GetFeature<OvCore::Rendering::EngineBufferRenderFeature>();
	auto& reflectionDescriptor = m_renderer.GetDescriptor<OvCore::Rendering::ReflectionRenderFeature::ReflectionDescriptor>();

	auto& sceneDescriptor = m_renderer.GetDescriptor<SceneRenderer::SceneDescriptor>();
	auto& frameDescriptor = m_renderer.GetFrameDescriptor();
	auto& scene = sceneDescriptor.scene;

	auto pso = m_renderer.CreatePipelineState();

	uint8_t lightIndex = 0;

	for (auto reflectionProbeReference : reflectionDescriptor.reflectionProbes)
	{
		auto& reflectionProbe = reflectionProbeReference.get();

		OvRendering::Entities::Camera reflectionCamera;
		reflectionCamera.SetPosition(reflectionProbe.owner.transform.GetWorldPosition());
		reflectionCamera.SetFov(90.0f);

		// For each face
		for (uint32_t faceIndex = 0U; faceIndex < 6U; ++faceIndex)
		{
			reflectionCamera.SetRotation(OvMaths::FQuaternion{ kCubeFaceRotations[faceIndex] });
			const auto [width, height] = reflectionProbe.GetFramebuffer().GetSize();

			reflectionCamera.CacheMatrices(
				width,
				height
			);

			engineBufferRenderFeature.SetCamera(reflectionCamera);

			reflectionProbe.GetFramebuffer().SetTargetDrawBuffer(faceIndex);

			reflectionProbe.GetFramebuffer().Bind();
			m_renderer.SetViewport(0, 0, width, height);
			m_renderer.Clear(true, true, true);
			_DrawReflections(pso, scene);
			reflectionProbe.GetCubemap()->GenerateMipmaps();
			reflectionProbe.GetFramebuffer().Unbind();
		}

		engineBufferRenderFeature.SetCamera(frameDescriptor.camera.value());
	}

	if (auto output = frameDescriptor.outputBuffer)
	{
		output.value().Bind();
	}

	m_renderer.SetViewport(0, 0, frameDescriptor.renderWidth, frameDescriptor.renderHeight);
}

void OvCore::Rendering::ReflectionRenderPass::_DrawReflections(
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
				// TODO: Filter dynamic objects (only static objects should be rendered in the reflection pass)
				if (auto materialRenderer = modelRenderer->owner.GetComponent<OvCore::ECS::Components::CMaterialRenderer>())
				{
					const auto& materials = materialRenderer->GetMaterials();
					const auto& modelMatrix = actor.transform.GetWorldMatrix();

					for (auto mesh : model->GetMeshes())
					{
						if (auto material = materials.at(mesh->GetMaterialIndex()); material && material->IsValid())
						{
							OvRendering::Entities::Drawable drawable;
							drawable.mesh = *mesh;
							drawable.material = material;
							drawable.stateMask = material->GenerateStateMask();

							// If not found, the default pass will be used.
							// This is only if a shader wants to override the default pass.
							drawable.pass = "REFLECTION_PASS"; 

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
