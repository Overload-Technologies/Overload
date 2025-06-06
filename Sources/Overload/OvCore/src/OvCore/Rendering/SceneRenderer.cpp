/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include <ranges>
#include <tracy/Tracy.hpp>

#include <OvCore/ECS/Components/CModelRenderer.h>
#include <OvCore/ECS/Components/CMaterialRenderer.h>
#include <OvCore/Global/ServiceLocator.h>
#include <OvCore/Rendering/EngineBufferRenderFeature.h>
#include <OvCore/Rendering/EngineDrawableDescriptor.h>
#include <OvCore/Rendering/PostProcessRenderPass.h>
#include <OvCore/Rendering/ReflectionRenderFeature.h>
#include <OvCore/Rendering/ReflectionRenderPass.h>
#include <OvCore/Rendering/SceneRenderer.h>
#include <OvCore/Rendering/ShadowRenderFeature.h>
#include <OvCore/Rendering/ShadowRenderPass.h>
#include <OvCore/ResourceManagement/ShaderManager.h>
#include <OvRendering/Data/Frustum.h>
#include <OvRendering/Features/LightingRenderFeature.h>
#include <OvRendering/HAL/Profiling.h>
#include <OvRendering/Resources/Loaders/ShaderLoader.h>

namespace
{
	using namespace OvCore::Rendering;

	class SceneRenderPass : public OvRendering::Core::ARenderPass
	{
	public:
		SceneRenderPass(OvRendering::Core::CompositeRenderer& p_renderer, bool stencilWrite = false) :
			OvRendering::Core::ARenderPass(p_renderer),
			m_stencilWrite(stencilWrite)
		{
		}

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
		{
		}

	protected:
		virtual void Draw(OvRendering::Data::PipelineState p_pso) override
		{
			ZoneScoped;
			TracyGpuZone("OpaqueRenderPass");

			PrepareStencilBuffer(p_pso);

			const auto& drawables = m_renderer.GetDescriptor<SceneRenderer::SceneFilteredDrawablesDescriptor>();

			for (const auto& drawable : drawables.opaques | std::views::values)
			{
				m_renderer.DrawEntity(p_pso, drawable);
			}
		}
	};

	class TransparentRenderPass : public SceneRenderPass
	{
	public:
		TransparentRenderPass(OvRendering::Core::CompositeRenderer& p_renderer, bool p_stencilWrite = false) :
			SceneRenderPass(p_renderer, p_stencilWrite) {
		}

	protected:
		virtual void Draw(OvRendering::Data::PipelineState p_pso) override
		{
			ZoneScoped;
			TracyGpuZone("TransparentRenderPass");

			PrepareStencilBuffer(p_pso);

			const auto& drawables = m_renderer.GetDescriptor<SceneRenderer::SceneFilteredDrawablesDescriptor>();

			for (const auto& drawable : drawables.transparents | std::views::values)
			{
				m_renderer.DrawEntity(p_pso, drawable);
			}
		}
	};

	class UIRenderPass : public SceneRenderPass
	{
	public:
		UIRenderPass(OvRendering::Core::CompositeRenderer& p_renderer, bool p_stencilWrite = false) :
			SceneRenderPass(p_renderer, p_stencilWrite) {
		}

	protected:
		virtual void Draw(OvRendering::Data::PipelineState p_pso) override
		{
			ZoneScoped;
			TracyGpuZone("UIRenderPass");

			PrepareStencilBuffer(p_pso);

			const auto& drawables = m_renderer.GetDescriptor<SceneRenderer::SceneFilteredDrawablesDescriptor>();

			for (const auto& drawable : drawables.ui | std::views::values)
			{
				m_renderer.DrawEntity(p_pso, drawable);
			}
		}
	};

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

	std::vector<std::reference_wrapper<OvCore::ECS::Components::CReflectionProbe>> FindActiveReflectionProbes(const OvCore::SceneSystem::Scene& p_scene)
	{
		std::vector<std::reference_wrapper<OvCore::ECS::Components::CReflectionProbe>> probes;
		const auto& facs = p_scene.GetFastAccessComponents();
		for (auto probe : facs.reflectionProbes)
		{
			if (probe->owner.IsActive())
			{
				probes.push_back(*probe);
			}
		}
		return probes;
	}
}

OvCore::Rendering::SceneRenderer::SceneRenderer(OvRendering::Context::Driver& p_driver, bool p_stencilWrite)
	: OvRendering::Core::CompositeRenderer(p_driver)
{
	using namespace OvRendering::Features;
	using namespace OvRendering::Settings;
	using enum OvRendering::Features::EFeatureExecutionPolicy;

	AddFeature<EngineBufferRenderFeature, ALWAYS>();
	AddFeature<LightingRenderFeature, ALWAYS>();

	AddFeature<ReflectionRenderFeature, WHITELIST_ONLY>()
		.Include<OpaqueRenderPass>()
		.Include<TransparentRenderPass>();

	AddFeature<ShadowRenderFeature, WHITELIST_ONLY>()
		.Include<OpaqueRenderPass>()
		.Include<TransparentRenderPass>()
		.Include<UIRenderPass>();

	AddPass<ShadowRenderPass>("Shadows", ERenderPassOrder::Shadows);
	AddPass<ReflectionRenderPass>("ReflectionRenderPass", ERenderPassOrder::Reflections);
	AddPass<OpaqueRenderPass>("Opaques", ERenderPassOrder::Opaque, p_stencilWrite);
	AddPass<TransparentRenderPass>("Transparents", ERenderPassOrder::Transparent, p_stencilWrite);
	AddPass<PostProcessRenderPass>("Post-Process", ERenderPassOrder::PostProcessing);
	AddPass<UIRenderPass>("UI", ERenderPassOrder::UI);
}

void OvCore::Rendering::SceneRenderer::BeginFrame(const OvRendering::Data::FrameDescriptor& p_frameDescriptor)
{
	ZoneScoped;

	OVASSERT(HasDescriptor<SceneDescriptor>(), "Cannot find SceneDescriptor attached to this renderer");

	auto& sceneDescriptor = GetDescriptor<SceneDescriptor>();

	const bool frustumLightCulling = p_frameDescriptor.camera.value().HasFrustumLightCulling();

	AddDescriptor<OvRendering::Features::LightingRenderFeature::LightingDescriptor>({
		FindActiveLights(sceneDescriptor.scene),
		frustumLightCulling ? sceneDescriptor.frustumOverride : std::nullopt
	});

	AddDescriptor<OvCore::Rendering::ReflectionRenderFeature::ReflectionDescriptor>({
		FindActiveReflectionProbes(sceneDescriptor.scene)
	});

	OvRendering::Core::CompositeRenderer::BeginFrame(p_frameDescriptor);

	AddDescriptor<SceneDrawablesDescriptor>({
		ParseScene(SceneParsingInput{
			.scene = sceneDescriptor.scene
		})
	});

	// Default filtered drawables descriptor using the main camera (used by most render passes).
	// Some other render passes can decide to filter the drawables themselves, using the 
	// SceneDrawablesDescriptor instead of the SceneFilteredDrawablesDescriptor one.
	AddDescriptor<SceneFilteredDrawablesDescriptor>({
		FilterDrawables(
			GetDescriptor<SceneDrawablesDescriptor>(),
			SceneDrawablesFilteringInput{
				.camera = p_frameDescriptor.camera.value(),
				.frustumOverride = sceneDescriptor.frustumOverride,
				.overrideMaterial = sceneDescriptor.overrideMaterial,
				.fallbackMaterial = sceneDescriptor.fallbackMaterial,
				.requiredVisibilityFlags = EVisibilityFlags::GEOMETRY
			}
		)
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

SceneRenderer::SceneDrawablesDescriptor OvCore::Rendering::SceneRenderer::ParseScene(const SceneParsingInput& p_input)
{
	ZoneScoped;

	using namespace OvCore::ECS::Components;

	// Containers for the parsed drawables.
	SceneRenderer::SceneDrawablesDescriptor result;

	const auto& scene = p_input.scene;

	for (const auto modelRenderer : scene.GetFastAccessComponents().modelRenderers)
	{
		auto& owner = modelRenderer->owner;
		if (!owner.IsActive()) continue;
		const auto model = modelRenderer->GetModel();
		if (!model) continue;
		const auto materialRenderer = modelRenderer->owner.GetComponent<CMaterialRenderer>();
		if (!materialRenderer) continue;
				
		const auto& transform = owner.transform.GetFTransform();
		const auto& materials = materialRenderer->GetMaterials();

		for (auto& mesh : model->GetMeshes())
		{
			OvTools::Utils::OptRef<OvRendering::Data::Material> material;

			if (mesh->GetMaterialIndex() < kMaxMaterialCount)
			{
				material = materials.at(mesh->GetMaterialIndex());
			}

			OvRendering::Entities::Drawable drawable{
				.mesh = *mesh,
				.material = material,
				.stateMask = material.has_value() ? material->GenerateStateMask() : OvRendering::Data::StateMask{},
			};

			auto bounds = [&]() -> std::optional<OvRendering::Geometry::BoundingSphere> {
				using enum CModelRenderer::EFrustumBehaviour;
				switch (modelRenderer->GetFrustumBehaviour())
				{
				case MESH_BOUNDS: return mesh->GetBoundingSphere();
				case DEPRECATED_MODEL_BOUNDS: return model->GetBoundingSphere();
				case CUSTOM_BOUNDS: return modelRenderer->GetCustomBoundingSphere();
				}
				return std::nullopt;
			}();

			drawable.AddDescriptor<SceneDrawableDescriptor>({
				.actor = modelRenderer->owner,
				.visibilityFlags = materialRenderer->GetVisibilityFlags(),
				.bounds = bounds,
			});
			
			drawable.AddDescriptor<EngineDrawableDescriptor>({
				transform.GetWorldMatrix(),
				materialRenderer->GetUserMatrix()
			});

			result.drawables.push_back(drawable);
		}
	}

	return result;
}

SceneRenderer::SceneFilteredDrawablesDescriptor OvCore::Rendering::SceneRenderer::FilterDrawables(
	const SceneDrawablesDescriptor& p_drawables,
	const SceneDrawablesFilteringInput& p_filteringInput
)
{
	ZoneScoped;

	using namespace OvCore::ECS::Components;

	SceneFilteredDrawablesDescriptor output;

	const auto& camera = p_filteringInput.camera;
	const auto& frustumOverride = p_filteringInput.frustumOverride;

	// Determine if we should use frustum culling
	OvTools::Utils::OptRef<const OvRendering::Data::Frustum> frustum;
	if (camera.HasFrustumGeometryCulling())
	{
		frustum = frustumOverride ? frustumOverride : camera.GetFrustum();
	}

	// Process each drawable
	for (const auto& drawable : p_drawables.drawables)
	{
		const auto& desc = drawable.GetDescriptor<SceneDrawableDescriptor>();

		// Skip drawables that do not satisfy the required visibility flags
		if (!SatisfiesVisibility(desc.visibilityFlags, p_filteringInput.requiredVisibilityFlags))
		{
			continue;
		}

		const auto targetMaterial = 
			p_filteringInput.overrideMaterial.has_value() ?
			p_filteringInput.overrideMaterial.value() :
			(drawable.material.has_value() ? drawable.material.value() : p_filteringInput.fallbackMaterial);

		// Skip if material is invalid
		if (!targetMaterial || !targetMaterial->IsValid()) continue;

		// Filter drawables based on the type (UI, opaque, transparent)
		// Except for the fallback material, which is always included.
		if (!p_filteringInput.fallbackMaterial || &p_filteringInput.fallbackMaterial.value() != &targetMaterial.value())
		{
			const bool isUI = targetMaterial->IsUserInterface();
			if (isUI && !p_filteringInput.includeUI) continue;
			if (!isUI && !targetMaterial->IsBlendable() && !p_filteringInput.includeOpaque) continue;
			if (!isUI && targetMaterial->IsBlendable() && !p_filteringInput.includeTransparent) continue;
		}

		// Perform frustum culling if enabled
		if (frustum && desc.bounds.has_value())
		{
			ZoneScopedN("Frustum Culling");

			// Get the engine drawable descriptor to access transform information
			const auto& engineDesc = drawable.GetDescriptor<EngineDrawableDescriptor>();

			if (!frustum->BoundingSphereInFrustum(desc.bounds.value(), desc.actor.transform.GetFTransform()))
			{
				continue; // Skip this drawable as it's outside the frustum
			}
		}

		// Calculate distance to camera for sorting
		const float distanceToCamera = OvMaths::FVector3::Distance(
			desc.actor.transform.GetWorldPosition(),
			camera.GetPosition()
		);

		// At this point we want to copy the drawable to avoid modifying the original one.
		// The copy will use the updated material.
		// At this point, the filtered drawable should be guaranteed to have a valid material.
		auto drawableCopy = drawable;
		drawableCopy.material = targetMaterial;
		drawableCopy.stateMask = targetMaterial->GenerateStateMask();

		// Categorize drawable based on their type.
		// This is also where sorting happens, using
		// the multimap key.
		if (drawableCopy.material->IsUserInterface())
		{
			output.ui.emplace(decltype(decltype(output.ui)::value_type::first){
				.order = drawableCopy.material->GetDrawOrder(),
				.distance = distanceToCamera
			}, drawableCopy);
		}
		else if (drawableCopy.material->IsBlendable())
		{
			output.transparents.emplace(decltype(decltype(output.transparents)::value_type::first){
				.order = drawableCopy.material->GetDrawOrder(),
				.distance = distanceToCamera
			}, drawableCopy);
		}
		else
		{
			output.opaques.emplace(decltype(decltype(output.opaques)::value_type::first){
				.order = drawableCopy.material->GetDrawOrder(),
				.distance = distanceToCamera
			}, drawableCopy);
		}
	}

	return output;
}
