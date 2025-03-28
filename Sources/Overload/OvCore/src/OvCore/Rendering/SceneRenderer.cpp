/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include <OvAnalytics/Profiling/ProfilerSpy.h>

#include <OvRendering/Data/Frustum.h>
#include <OvRendering/Features/LightingRenderFeature.h>
#include <OvRendering/Resources/Loaders/ShaderLoader.h>

#include "OvCore/Rendering/SceneRenderer.h"
#include "OvCore/Rendering/EngineBufferRenderFeature.h"
#include "OvCore/Rendering/EngineDrawableDescriptor.h"
#include "OvCore/Rendering/ShadowRenderFeature.h"
#include "OvCore/Rendering/ShadowRenderPass.h"
#include "OvCore/Rendering/PostProcessRenderPass.h"
#include "OvCore/ECS/Components/CModelRenderer.h"
#include "OvCore/ECS/Components/CMaterialRenderer.h"
#include "OvCore/Global/ServiceLocator.h"
#include "OvCore/ResourceManagement/ShaderManager.h"

struct SceneRenderPassDescriptor
{
	OvCore::Rendering::SceneRenderer::AllDrawables drawables;
};

class SceneRenderPass : public OvRendering::Core::ARenderPass
{
public:
	SceneRenderPass(OvRendering::Core::CompositeRenderer& p_renderer, bool stencilWrite = false) :
		OvRendering::Core::ARenderPass(p_renderer),
		m_stencilWrite(stencilWrite)
	{}

protected:
	void PrepareStencilBuffer(OvRendering::Data::PipelineState& p_pso)
	{
		p_pso.stencilTest = true;
		p_pso.stencilWriteMask = 0xFF;
		p_pso.stencilFuncRef = 1;
		p_pso.stencilFuncMask = 0xFF;
		p_pso.stencilOpFail = OvRendering::Settings::EOperation::REPLACE;
		p_pso.depthOpFail = OvRendering::Settings::EOperation::REPLACE;
		p_pso.bothOpFail = OvRendering::Settings::EOperation::REPLACE;
		p_pso.colorWriting.mask = 0x00;
	}

private:
	bool m_stencilWrite;
};

class OpaqueRenderPass : public SceneRenderPass
{
public:
	OpaqueRenderPass(OvRendering::Core::CompositeRenderer& p_renderer, bool p_stencilWrite = false) :
		SceneRenderPass(p_renderer, p_stencilWrite)
	{}

protected:
	virtual void Draw(OvRendering::Data::PipelineState p_pso) override
	{
		PrepareStencilBuffer(p_pso);

		auto& sceneContent = m_renderer.GetDescriptor<SceneRenderPassDescriptor>();

		for (const auto& [distance, drawable] : sceneContent.drawables.opaques)
		{
			m_renderer.DrawEntity(p_pso, drawable);
		}
	}
};

class TransparentRenderPass : public SceneRenderPass
{
public:
	TransparentRenderPass(OvRendering::Core::CompositeRenderer& p_renderer, bool p_stencilWrite = false) :
		SceneRenderPass(p_renderer, p_stencilWrite) {}

protected:
	virtual void Draw(OvRendering::Data::PipelineState p_pso) override
	{
		PrepareStencilBuffer(p_pso);

		auto& sceneContent = m_renderer.GetDescriptor<SceneRenderPassDescriptor>();

		for (const auto& [distance, drawable] : sceneContent.drawables.transparents)
		{
			m_renderer.DrawEntity(p_pso, drawable);
		}
	}
};

class UIRenderPass : public SceneRenderPass
{
public:
	UIRenderPass(OvRendering::Core::CompositeRenderer& p_renderer, bool p_stencilWrite = false) :
		SceneRenderPass(p_renderer, p_stencilWrite) {}

protected:
	virtual void Draw(OvRendering::Data::PipelineState p_pso) override
	{
		PrepareStencilBuffer(p_pso);

		auto& sceneContent = m_renderer.GetDescriptor<SceneRenderPassDescriptor>();

		for (const auto& [distance, drawable] : sceneContent.drawables.ui)
		{
			m_renderer.DrawEntity(p_pso, drawable);
		}
	}
};


OvCore::Rendering::SceneRenderer::SceneRenderer(OvRendering::Context::Driver& p_driver, bool p_stencilWrite)
	: OvRendering::Core::CompositeRenderer(p_driver)
{
	AddFeature<EngineBufferRenderFeature>();
	AddFeature<OvRendering::Features::LightingRenderFeature>();
	AddFeature<ShadowRenderFeature>();

	AddPass<ShadowRenderPass>("Shadows", OvRendering::Settings::ERenderPassOrder::Shadows);
	AddPass<OpaqueRenderPass>("Opaques", OvRendering::Settings::ERenderPassOrder::Opaque, p_stencilWrite);
	AddPass<TransparentRenderPass>("Transparents", OvRendering::Settings::ERenderPassOrder::Transparent, p_stencilWrite);
	AddPass<PostProcessRenderPass>("Post-Process", OvRendering::Settings::ERenderPassOrder::PostProcessing);
	AddPass<UIRenderPass>("UI", OvRendering::Settings::ERenderPassOrder::UI);
}

OvRendering::Features::LightingRenderFeature::LightSet FindActiveLights(const OvCore::SceneSystem::Scene& p_scene)
{
	OvRendering::Features::LightingRenderFeature::LightSet lights;

	const auto& facs = p_scene.GetFastAccessComponents();

	for (auto light : facs.lights)
	{
		if (light->owner.IsActive())
		{
			lights.push_back(std::ref(light->GetData()));
		}
	}

	return lights;
}

void OvCore::Rendering::SceneRenderer::BeginFrame(const OvRendering::Data::FrameDescriptor& p_frameDescriptor)
{
	OVASSERT(HasDescriptor<SceneDescriptor>(), "Cannot find SceneDescriptor attached to this renderer");

	auto& sceneDescriptor = GetDescriptor<SceneDescriptor>();

	const bool frustumLightCulling = p_frameDescriptor.camera.value().HasFrustumLightCulling();

	AddDescriptor<OvRendering::Features::LightingRenderFeature::LightingDescriptor>({
		FindActiveLights(sceneDescriptor.scene),
		frustumLightCulling ? sceneDescriptor.frustumOverride : std::nullopt
	});

	OvRendering::Core::CompositeRenderer::BeginFrame(p_frameDescriptor);

	AddDescriptor<SceneRenderPassDescriptor>({
		ParseScene()
	});
}

void OvCore::Rendering::SceneRenderer::DrawModelWithSingleMaterial(OvRendering::Data::PipelineState p_pso, OvRendering::Resources::Model& p_model, OvRendering::Data::Material& p_material, const OvMaths::FMatrix4& p_modelMatrix)
{
	auto stateMask = p_material.GenerateStateMask();
	auto userMatrix = OvMaths::FMatrix4::Identity;

	auto engineDrawableDescriptor = EngineDrawableDescriptor{
		p_modelMatrix,
		userMatrix
	};

	for (auto mesh : p_model.GetMeshes())
	{
		OvRendering::Entities::Drawable element;
		element.mesh = *mesh;
		element.material = p_material;
		element.stateMask = stateMask;
		element.AddDescriptor(engineDrawableDescriptor);

		DrawEntity(p_pso, element);
	}
}

OvCore::Rendering::SceneRenderer::AllDrawables OvCore::Rendering::SceneRenderer::ParseScene()
{
	using namespace OvCore::ECS::Components;

	OpaqueDrawables opaques;
	TransparentDrawables transparents;
	UIDrawables ui;

	auto& camera = m_frameDescriptor.camera.value();

	auto& sceneDescriptor = GetDescriptor<SceneDescriptor>();
	auto& scene = sceneDescriptor.scene;
	auto overrideMaterial = sceneDescriptor.overrideMaterial;
	auto fallbackMaterial = sceneDescriptor.fallbackMaterial;

	OvTools::Utils::OptRef<const OvRendering::Data::Frustum> frustum;

	if (camera.HasFrustumGeometryCulling())
	{
		auto& frustumOverride = sceneDescriptor.frustumOverride;
		frustum = frustumOverride ? frustumOverride : camera.GetFrustum();
	}

	for (CModelRenderer* modelRenderer : scene.GetFastAccessComponents().modelRenderers)
	{
		auto& owner = modelRenderer->owner;

		if (owner.IsActive())
		{
			if (auto model = modelRenderer->GetModel())
			{
				if (auto materialRenderer = modelRenderer->owner.GetComponent<CMaterialRenderer>())
				{
					auto& transform = owner.transform.GetFTransform();

					auto cullingOptions = OvRendering::Settings::ECullingOptions::NONE;

					if (modelRenderer->GetFrustumBehaviour() != CModelRenderer::EFrustumBehaviour::DISABLED)
					{
						cullingOptions |= OvRendering::Settings::ECullingOptions::FRUSTUM_PER_MODEL;
					}

					if (modelRenderer->GetFrustumBehaviour() == CModelRenderer::EFrustumBehaviour::CULL_MESHES)
					{
						cullingOptions |= OvRendering::Settings::ECullingOptions::FRUSTUM_PER_MESH;
					}

					const auto& modelBoundingSphere = modelRenderer->GetFrustumBehaviour() == CModelRenderer::EFrustumBehaviour::CULL_CUSTOM ? modelRenderer->GetCustomBoundingSphere() : model->GetBoundingSphere();

					std::vector<OvRendering::Resources::Mesh*> meshes;

					if (frustum)
					{
						PROFILER_SPY("Frustum Culling");
						meshes = frustum.value().GetMeshesInFrustum(*model, modelBoundingSphere, transform, cullingOptions);
					}
					else
					{
						meshes = model->GetMeshes();
					}

					if (!meshes.empty())
					{
						float distanceToActor = OvMaths::FVector3::Distance(transform.GetWorldPosition(), camera.GetPosition());
						const OvCore::ECS::Components::CMaterialRenderer::MaterialList& materials = materialRenderer->GetMaterials();

						for (const auto& mesh : meshes)
						{
							OvTools::Utils::OptRef<OvRendering::Data::Material> material;

							if (mesh->GetMaterialIndex() < kMaxMaterialCount)
							{
								if (overrideMaterial && overrideMaterial->IsValid())
								{
									material = overrideMaterial.value();
								}
								else
								{
									material = materials.at(mesh->GetMaterialIndex());
								}

								const bool isMaterialValid = material && material->IsValid();
								const bool hasValidFallbackMaterial = fallbackMaterial && fallbackMaterial->IsValid();

								if (!isMaterialValid && hasValidFallbackMaterial)
								{
									material = fallbackMaterial;
								}
							}

							if (material && material->IsValid())
							{
								OvRendering::Entities::Drawable drawable;
								drawable.mesh = *mesh;
								drawable.material = material;
								drawable.stateMask = material->GenerateStateMask();

								drawable.AddDescriptor<EngineDrawableDescriptor>({
									transform.GetWorldMatrix(),
									materialRenderer->GetUserMatrix()
								});

								if (material->IsUserInterface())
								{
									ui.emplace(distanceToActor, drawable);
								}
								else
								{
									if (material->IsBlendable())
									{
										transparents.emplace(distanceToActor, drawable);
									}
									else
									{
										opaques.emplace(distanceToActor, drawable);
									}
								}
							}
						}
					}
				}
			}
		}
	}

	return { opaques, transparents, ui };
}
