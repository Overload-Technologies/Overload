/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include <OvCore/ECS/Components/CMaterialRenderer.h>
#include <OvCore/Rendering/EngineDrawableDescriptor.h>
#include <OvEditor/Core/EditorActions.h>
#include <OvEditor/Rendering/DebugModelRenderFeature.h>
#include <OvEditor/Rendering/OutlineRenderFeature.h>
#include <OvEditor/Settings/EditorSettings.h>
#include <OvRendering/Utils/Conversions.h>

namespace
{
	constexpr uint32_t kStencilMask = 0xFF;
	constexpr int32_t kStencilReference = 1;
	constexpr std::string_view kOutlinePassName = "OUTLINE_PASS";
}

OvEditor::Rendering::OutlineRenderFeature::OutlineRenderFeature(OvRendering::Core::CompositeRenderer& p_renderer) :
	OvRendering::Features::ARenderFeature(p_renderer)
{
	/* Stencil Fill Material */
	m_stencilFillMaterial.SetShader(EDITOR_CONTEXT(editorResources)->GetShader("OutlineFallback"));

	/* Outline Material */
	m_outlineMaterial.SetShader(EDITOR_CONTEXT(editorResources)->GetShader("OutlineFallback"));
}

void OvEditor::Rendering::OutlineRenderFeature::DrawOutline(
	OvCore::ECS::Actor& p_actor,
	const OvMaths::FVector4& p_color,
	float p_thickness
)
{
	DrawStencilPass(p_actor);
	DrawOutlinePass(p_actor, p_color, p_thickness);
}

void OvEditor::Rendering::OutlineRenderFeature::DrawStencilPass(OvCore::ECS::Actor& p_actor)
{
	auto pso = m_renderer.CreatePipelineState();

	pso.stencilTest = true;
	pso.stencilWriteMask = kStencilMask;
	pso.stencilFuncRef = kStencilReference;
	pso.stencilFuncMask = kStencilMask;
	pso.stencilOpFail = OvRendering::Settings::EOperation::REPLACE;
	pso.depthOpFail = OvRendering::Settings::EOperation::REPLACE;
	pso.bothOpFail = OvRendering::Settings::EOperation::REPLACE;
	pso.colorWriting.mask = 0x00;

	DrawActorToStencil(pso, p_actor);
}

void OvEditor::Rendering::OutlineRenderFeature::DrawOutlinePass(OvCore::ECS::Actor& p_actor, const OvMaths::FVector4& p_color, float p_thickness)
{
	auto pso = m_renderer.CreatePipelineState();

	pso.stencilTest = true;
	pso.stencilOpFail = OvRendering::Settings::EOperation::KEEP;
	pso.depthOpFail = OvRendering::Settings::EOperation::KEEP;
	pso.bothOpFail = OvRendering::Settings::EOperation::REPLACE;
	pso.stencilFuncOp = OvRendering::Settings::EComparaisonAlgorithm::NOTEQUAL;
	pso.stencilFuncRef = kStencilReference;
	pso.stencilFuncMask = kStencilMask;
	pso.rasterizationMode = OvRendering::Settings::ERasterizationMode::LINE;
	pso.lineWidthPow2 = OvRendering::Utils::Conversions::FloatToPow2(p_thickness);

	DrawActorOutline(pso, p_actor, p_color);
}

void OvEditor::Rendering::OutlineRenderFeature::DrawActorToStencil(OvRendering::Data::PipelineState p_pso, OvCore::ECS::Actor& p_actor)
{
	if (p_actor.IsActive())
	{
		/* Render static mesh outline and bounding spheres */
		if (auto modelRenderer = p_actor.GetComponent<OvCore::ECS::Components::CModelRenderer>(); modelRenderer && modelRenderer->GetModel())
		{
			if (auto materialRenderer = p_actor.GetComponent<OvCore::ECS::Components::CMaterialRenderer>())
			{
				DrawModelToStencil(
					p_pso,
					p_actor.transform.GetWorldMatrix(),
					*modelRenderer->GetModel(),
					materialRenderer->GetMaterials()
				);
			}
		}

		/* Render camera component outline */
		if (auto cameraComponent = p_actor.GetComponent<OvCore::ECS::Components::CCamera>(); cameraComponent)
		{
			auto translation = OvMaths::FMatrix4::Translation(p_actor.transform.GetWorldPosition());
			auto rotation = OvMaths::FQuaternion::ToMatrix4(p_actor.transform.GetWorldRotation());
			auto model = translation * rotation;
			DrawModelToStencil(p_pso, model, *EDITOR_CONTEXT(editorResources)->GetModel("Camera"));
		}

		if (auto reflectionProbeComponent = p_actor.GetComponent<OvCore::ECS::Components::CReflectionProbe>(); reflectionProbeComponent)
		{
			const auto translation = OvMaths::FMatrix4::Translation(p_actor.transform.GetWorldPosition());
			const auto rotation = OvMaths::FQuaternion::ToMatrix4(p_actor.transform.GetWorldRotation());
			const auto scale = OvMaths::FMatrix4::Scaling({ 0.5f, 0.5f, 0.5f });
			const auto model = translation * rotation * scale;
			DrawModelToStencil(p_pso, model, *EDITOR_CONTEXT(editorResources)->GetModel("Sphere"));
		}

		for (auto& child : p_actor.GetChildren())
		{
			DrawActorToStencil(p_pso, *child);
		}
	}
}

void OvEditor::Rendering::OutlineRenderFeature::DrawActorOutline(
	OvRendering::Data::PipelineState p_pso,
	OvCore::ECS::Actor& p_actor,
	const OvMaths::FVector4& p_color
)
{
	if (p_actor.IsActive())
	{
		if (auto modelRenderer = p_actor.GetComponent<OvCore::ECS::Components::CModelRenderer>(); modelRenderer && modelRenderer->GetModel())
		{
			if (auto materialRenderer = p_actor.GetComponent<OvCore::ECS::Components::CMaterialRenderer>())
			{
				DrawModelOutline(
					p_pso,
					p_actor.transform.GetWorldMatrix(),
					*modelRenderer->GetModel(),
					p_color,
					materialRenderer->GetMaterials()
				);
			}
		}

		if (auto cameraComponent = p_actor.GetComponent<OvCore::ECS::Components::CCamera>(); cameraComponent)
		{
			auto translation = OvMaths::FMatrix4::Translation(p_actor.transform.GetWorldPosition());
			auto rotation = OvMaths::FQuaternion::ToMatrix4(p_actor.transform.GetWorldRotation());
			auto model = translation * rotation;
			DrawModelOutline(p_pso, model, *EDITOR_CONTEXT(editorResources)->GetModel("Camera"), p_color);
		}

		if (auto reflectionProbeComponent = p_actor.GetComponent<OvCore::ECS::Components::CReflectionProbe>(); reflectionProbeComponent)
		{
			const auto translation = OvMaths::FMatrix4::Translation(p_actor.transform.GetWorldPosition());
			const auto rotation = OvMaths::FQuaternion::ToMatrix4(p_actor.transform.GetWorldRotation());
			const auto scale = OvMaths::FMatrix4::Scaling({ 0.5f, 0.5f, 0.5f });
			const auto model = translation * rotation * scale;
			DrawModelOutline(p_pso, model, *EDITOR_CONTEXT(editorResources)->GetModel("Sphere"), p_color);
		}

		for (auto& child : p_actor.GetChildren())
		{
			DrawActorOutline(p_pso, *child, p_color);
		}
	}
}

void OvEditor::Rendering::OutlineRenderFeature::DrawModelToStencil(
	OvRendering::Data::PipelineState p_pso,
	const OvMaths::FMatrix4& p_worldMatrix,
	OvRendering::Resources::Model& p_model,
	OvTools::Utils::OptRef<const OvCore::ECS::Components::CMaterialRenderer::MaterialList> p_materials
)
{
	const std::string outlinePassName{ kOutlinePassName };

	for (auto mesh : p_model.GetMeshes())
	{
		auto getStencilMaterial = [&]() -> OvCore::Resources::Material& {
			auto material = p_materials.has_value() ? p_materials->at(mesh->GetMaterialIndex()) : nullptr;
			if (material && material->IsValid() && material->HasPass(outlinePassName))
			{
				return *material;
			}
			return m_stencilFillMaterial;
		};

		auto& targetMaterial = getStencilMaterial();

		auto stateMask = targetMaterial.GenerateStateMask();

		auto engineDrawableDescriptor = OvCore::Rendering::EngineDrawableDescriptor{
			p_worldMatrix,
			OvMaths::FMatrix4::Identity
		};

		OvRendering::Entities::Drawable element;
		element.mesh = *mesh;
		element.material = targetMaterial;
		element.stateMask = stateMask;
		element.stateMask.depthTest = false;
		element.stateMask.colorWriting = false;
		element.pass = outlinePassName;

		element.AddDescriptor(engineDrawableDescriptor);

		m_renderer.DrawEntity(p_pso, element);
	}
}

void OvEditor::Rendering::OutlineRenderFeature::DrawModelOutline(
	OvRendering::Data::PipelineState p_pso,
	const OvMaths::FMatrix4& p_worldMatrix,
	OvRendering::Resources::Model& p_model,
	const OvMaths::FVector4& p_color,
	OvTools::Utils::OptRef<const OvCore::ECS::Components::CMaterialRenderer::MaterialList> p_materials
)
{
	const std::string outlinePassName{ kOutlinePassName };

	for (auto mesh : p_model.GetMeshes())
	{
		auto getStencilMaterial = [&]() -> OvCore::Resources::Material& {
			auto material = p_materials.has_value() ? p_materials->at(mesh->GetMaterialIndex()) : nullptr;
			if (material && material->IsValid() && material->HasPass(outlinePassName))
			{
				return *material;
			}
			return m_stencilFillMaterial;
		};

		auto& targetMaterial = getStencilMaterial();

		// Set the outline color property if it exists
		if (targetMaterial.GetProperty("_OutlineColor"))
		{
			targetMaterial.SetProperty("_OutlineColor", p_color, true);
		}

		auto stateMask = targetMaterial.GenerateStateMask();

		auto engineDrawableDescriptor = OvCore::Rendering::EngineDrawableDescriptor{
			p_worldMatrix,
			OvMaths::FMatrix4::Identity
		};

		OvRendering::Entities::Drawable drawable;
		drawable.mesh = *mesh;
		drawable.material = targetMaterial;
		drawable.stateMask = stateMask;
		drawable.stateMask.depthTest = false;
		drawable.pass = outlinePassName;

		drawable.AddDescriptor(engineDrawableDescriptor);

		m_renderer.DrawEntity(p_pso, drawable);
	}
}
