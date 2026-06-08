/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include <functional>
#include <ranges>
#include <string>
#include <vector>
#include <tracy/Tracy.hpp>

#include <OvCore/ECS/Components/CModelRenderer.h>
#include <OvCore/ECS/Components/CMaterialRenderer.h>
#include <OvCore/ECS/Components/CSkinnedMeshRenderer.h>
#include <OvCore/ECS/Components/UI/CCanvas.h>
#include <OvCore/ECS/Components/UI/CImage.h>
#include <OvCore/ECS/Components/UI/CText.h>
#include <OvCore/Global/ServiceLocator.h>
#include <OvCore/Rendering/EngineBufferRenderFeature.h>
#include <OvCore/Rendering/EngineDrawableDescriptor.h>
#include <OvCore/Rendering/PostProcessRenderPass.h>
#include <OvCore/Rendering/ReflectionRenderFeature.h>
#include <OvCore/Rendering/ReflectionRenderPass.h>
#include <OvCore/Rendering/SceneRenderer.h>
#include <OvCore/Rendering/ShadowRenderFeature.h>
#include <OvCore/Rendering/ShadowRenderPass.h>
#include <OvCore/Rendering/SkinningDrawableDescriptor.h>
#include <OvCore/Rendering/SkinningRenderFeature.h>
#include <OvCore/Rendering/SkinningUtils.h>
#include <OvCore/Rendering/UIRenderingUtils.h>
#include <OvCore/ResourceManagement/ShaderManager.h>
#include <OvRendering/Data/Frustum.h>
#include <OvRendering/Features/LightingRenderFeature.h>
#include <OvRendering/HAL/Profiling.h>
#include <OvRendering/Resources/Loaders/ShaderLoader.h>

namespace
{
	using namespace OvCore::Rendering;
	const std::string kSkinningFeatureName{ SkinningUtils::kFeatureName };

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

	EngineDrawableDescriptor CreateUIDrawableDescriptor(
		OvCore::ECS::Actor& p_owner,
		const OvCore::Rendering::UIRenderingUtils::UIFrameResolver& p_uiFrameResolver,
		const OvMaths::FMatrix4& p_uiProjectionMatrix,
		const OvMaths::FVector2& p_elementSize
	)
	{
		EngineDrawableDescriptor descriptor{
			.modelMatrix = p_owner.transform.GetFTransform().GetWorldMatrix(),
			.userMatrix = OvMaths::FMatrix4::Identity
		};

		OvCore::Rendering::UIRenderingUtils::ResolvedUIElement resolvedElement;
		if (p_uiFrameResolver.ResolveElement(
			p_owner,
			p_elementSize,
			resolvedElement
		))
		{
			descriptor.modelMatrix = resolvedElement.modelMatrix;

			if (p_uiFrameResolver.IsScreenSpace())
			{
				descriptor.viewMatrixOverride = OvMaths::FMatrix4::Identity;
				descriptor.projectionMatrixOverride = p_uiProjectionMatrix;
			}
		}

		return descriptor;
	}

	void AppendImageDrawable(
		SceneRenderer::SceneDrawablesDescriptor& p_result,
		OvCore::ECS::Components::UI::CImage& p_image,
		const OvCore::Rendering::UIRenderingUtils::UIFrameResolver& p_uiFrameResolver,
		const OvMaths::FMatrix4& p_uiProjectionMatrix,
		int p_drawOrder
	)
	{
		auto& owner = p_image.owner;
		auto* material = p_image.GetMaterial();
		if (!material) return;

		OvRendering::Entities::Drawable drawable{
			.mesh = p_image.GetMesh(),
			.material = *material,
			.stateMask = material->GenerateStateMask()
		};

		drawable.AddDescriptor<SceneRenderer::SceneDrawableDescriptor>({
			.actor = owner,
			.visibilityFlags = EVisibilityFlags::GEOMETRY,
			.bounds = std::nullopt,
			.drawOrderOverride = p_drawOrder,
			.isUserInterface = true
		});

		drawable.AddDescriptor<EngineDrawableDescriptor>(
			CreateUIDrawableDescriptor(
				owner,
				p_uiFrameResolver,
				p_uiProjectionMatrix,
				p_image.GetIntrinsicSize()
			)
		);

		p_result.drawables.push_back(drawable);
	}

	void AppendTextDrawable(
		SceneRenderer::SceneDrawablesDescriptor& p_result,
		OvCore::ECS::Components::UI::CText& p_text,
		const OvCore::Rendering::UIRenderingUtils::UIFrameResolver& p_uiFrameResolver,
		const OvMaths::FMatrix4& p_uiProjectionMatrix,
		int p_drawOrder
	)
	{
		auto& owner = p_text.owner;
		auto* material = p_text.GetMaterial();
		if (!material) return;

		const auto baseTextSize = p_text.GetSize();
		OvCore::Rendering::UIRenderingUtils::ResolvedUIElement resolvedElement;
		const bool hasResolvedElement = p_uiFrameResolver.ResolveElement(
			owner,
			baseTextSize,
			resolvedElement
		);
		const auto resolvedTextSize = hasResolvedElement ? resolvedElement.effectiveSize : baseTextSize;

		auto* mesh = p_text.GetMesh(resolvedTextSize);
		if (!mesh) return;
		const auto renderedTextSize = p_text.GetSize(resolvedTextSize);

		OvRendering::Entities::Drawable drawable{
			.mesh = *mesh,
			.material = *material,
			.stateMask = material->GenerateStateMask()
		};

		drawable.AddDescriptor<SceneRenderer::SceneDrawableDescriptor>({
			.actor = owner,
			.visibilityFlags = EVisibilityFlags::GEOMETRY,
			.bounds = std::nullopt,
			.drawOrderOverride = p_drawOrder,
			.isUserInterface = true
		});

		drawable.AddDescriptor<EngineDrawableDescriptor>(
			CreateUIDrawableDescriptor(
				owner,
				p_uiFrameResolver,
				p_uiProjectionMatrix,
				renderedTextSize
			)
		);

		p_result.drawables.push_back(drawable);
	}

	void AppendHierarchyUIDrawables(
		SceneRenderer::SceneDrawablesDescriptor& p_result,
		OvCore::ECS::Actor& p_actor,
		const OvCore::Rendering::UIRenderingUtils::UIFrameResolver& p_uiFrameResolver,
		const OvCore::ECS::Components::UI::CCanvas* p_canvas,
		const OvMaths::FMatrix4& p_uiProjectionMatrix,
		int& p_drawOrder
	)
	{
		if (!p_actor.IsActive())
		{
			return;
		}

		if (auto* canvas = p_actor.GetComponent<OvCore::ECS::Components::UI::CCanvas>())
		{
			p_canvas = canvas;
		}

		if (p_canvas)
		{
			if (auto* image = p_actor.GetComponent<OvCore::ECS::Components::UI::CImage>())
			{
				AppendImageDrawable(
					p_result,
					*image,
					p_uiFrameResolver,
					p_uiProjectionMatrix,
					p_drawOrder++
				);
			}

			if (auto* text = p_actor.GetComponent<OvCore::ECS::Components::UI::CText>())
			{
				AppendTextDrawable(
					p_result,
					*text,
					p_uiFrameResolver,
					p_uiProjectionMatrix,
					p_drawOrder++
				);
			}
		}

		for (auto* child : p_actor.GetChildren())
		{
			if (child)
			{
				AppendHierarchyUIDrawables(
					p_result,
					*child,
					p_uiFrameResolver,
					p_canvas,
					p_uiProjectionMatrix,
					p_drawOrder
				);
			}
		}
	}

	void AppendHierarchyUIDrawables(
		SceneRenderer::SceneDrawablesDescriptor& p_result,
		OvCore::SceneSystem::Scene& p_scene,
		const OvCore::Rendering::UIRenderingUtils::UIFrameResolver& p_uiFrameResolver
	)
	{
		int drawOrder = 0;
		const auto uiProjectionMatrix = p_uiFrameResolver.CreateProjectionMatrix();

		for (auto* actor : p_scene.GetActors())
		{
			if (actor && !actor->HasParent())
			{
				AppendHierarchyUIDrawables(
					p_result,
					*actor,
					p_uiFrameResolver,
					nullptr,
					uiProjectionMatrix,
					drawOrder
				);
			}
		}
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
	AddFeature<SkinningRenderFeature, ALWAYS>();

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
	const auto renderSize = OvMaths::FVector2{
		static_cast<float>(p_frameDescriptor.renderWidth),
		static_cast<float>(p_frameDescriptor.renderHeight)
	};

	const bool frustumLightCulling = p_frameDescriptor.camera.value().HasFrustumLightCulling();

	AddDescriptor<OvRendering::Features::LightingRenderFeature::LightingDescriptor>({
		FindActiveLights(sceneDescriptor.scene),
		frustumLightCulling ? sceneDescriptor.frustumOverride : std::nullopt
	});

	AddDescriptor<OvCore::Rendering::ReflectionRenderFeature::ReflectionDescriptor>({
		FindActiveReflectionProbes(sceneDescriptor.scene)
	});

	SetDescriptor(OvCore::Rendering::UIRenderingUtils::UIFrameResolver{
		renderSize,
		sceneDescriptor.renderUIInScreenSpace
	});

	OvRendering::Core::CompositeRenderer::BeginFrame(p_frameDescriptor);

	AddDescriptor<SceneDrawablesDescriptor>({
		ParseScene(SceneParsingInput{
			.scene = sceneDescriptor.scene,
			.renderSize = renderSize,
			.renderUIInScreenSpace = sceneDescriptor.renderUIInScreenSpace,
			.uiFrameResolver = &GetDescriptor<OvCore::Rendering::UIRenderingUtils::UIFrameResolver>()
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
				.requiredVisibilityFlags = EVisibilityFlags::GEOMETRY,
				.includeUI = sceneDescriptor.includeUI
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

	auto& scene = p_input.scene;
	OvCore::Rendering::UIRenderingUtils::UIFrameResolver fallbackUIFrameResolver{
		p_input.renderSize,
		p_input.renderUIInScreenSpace
	};
	const auto& uiFrameResolver = p_input.uiFrameResolver ?
		*p_input.uiFrameResolver :
		fallbackUIFrameResolver;

	for (const auto modelRenderer : scene.GetFastAccessComponents().modelRenderers)
	{
		auto& owner = modelRenderer->owner;
		if (!owner.IsActive()) continue;
		const auto model = modelRenderer->GetModel();
		if (!model) continue;
		const auto materialRenderer = modelRenderer->owner.GetComponent<CMaterialRenderer>();
		if (!materialRenderer) continue;
		const auto* skinnedRenderer = owner.GetComponent<CSkinnedMeshRenderer>();
		const bool hasSkinning = SkinningUtils::IsSkinningActive(skinnedRenderer);

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
				default: return std::nullopt;
				}
				return std::nullopt;
			}();

			drawable.AddDescriptor<SceneDrawableDescriptor>({
				.actor = modelRenderer->owner,
				.visibilityFlags = materialRenderer->GetVisibilityFlags(),
				.bounds = bounds
			});

			drawable.AddDescriptor<EngineDrawableDescriptor>({
				transform.GetWorldMatrix(),
				materialRenderer->GetUserMatrix()
			});

			if (hasSkinning && mesh->HasSkinningData())
			{
				SkinningUtils::ApplyDescriptor(drawable, *skinnedRenderer);
			}

			result.drawables.push_back(drawable);
		}
	}

	AppendHierarchyUIDrawables(result, scene, uiFrameResolver);

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
		OvTools::Utils::OptRef<const SkinningDrawableDescriptor> skinningDescriptor;
		const bool hasSkinningDescriptor = drawable.TryGetDescriptor<SkinningDrawableDescriptor>(skinningDescriptor);

		// Skip drawables that do not satisfy the required visibility flags
		if (!SatisfiesVisibility(desc.visibilityFlags, p_filteringInput.requiredVisibilityFlags))
		{
			continue;
		}

		if (desc.isUserInterface && !p_filteringInput.includeUI)
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

			auto cullingBounds = desc.bounds.value();
			if (hasSkinningDescriptor)
			{
				cullingBounds.radius *= skinningDescriptor->boundsScale;
			}

			if (!frustum->BoundingSphereInFrustum(cullingBounds, desc.actor.transform.GetFTransform()))
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

		if (
			hasSkinningDescriptor &&
			targetMaterial->HasShader() &&
			targetMaterial->SupportsFeature(kSkinningFeatureName)
		)
		{
			drawableCopy.featureSetOverride = SkinningUtils::BuildFeatureSet(&targetMaterial->GetFeatures());
		}
		else
		{
			drawableCopy.featureSetOverride = std::nullopt;
		}

		const auto drawOrder = desc.drawOrderOverride.value_or(drawableCopy.material->GetDrawOrder());

		// Categorize drawable based on their type.
		// This is also where sorting happens, using
		// the multimap key.
		if (drawableCopy.material->IsUserInterface())
		{
			output.ui.emplace(decltype(decltype(output.ui)::value_type::first){
				.order = drawOrder,
				.materialKey = &drawableCopy.material.value(),
				.distance = distanceToCamera
			}, drawableCopy);
		}
		else if (drawableCopy.material->IsBlendable())
		{
			output.transparents.emplace(decltype(decltype(output.transparents)::value_type::first){
				.order = drawOrder,
				.materialKey = &drawableCopy.material.value(),
				.distance = distanceToCamera
			}, drawableCopy);
		}
		else
		{
			output.opaques.emplace(decltype(decltype(output.opaques)::value_type::first){
				.order = drawOrder,
				.materialKey = &drawableCopy.material.value(),
				.distance = distanceToCamera
			}, drawableCopy);
		}
	}

	return output;
}
