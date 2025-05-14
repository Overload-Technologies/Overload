/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include <OvRendering/Features/FrameInfoRenderFeature.h>

#include <OvTools/Utils/PathParser.h>

#include <OvUI/Plugins/DDTarget.h>

#include <OvCore/ECS/Components/CDirectionalLight.h>
#include <OvCore/ECS/Components/CAmbientSphereLight.h>
#include <OvCore/Rendering/SceneRenderer.h>

#include "OvEditor/Core/EditorActions.h"
#include "OvEditor/Panels/AssetView.h"
#include "OvEditor/Rendering/GridRenderPass.h"
#include "OvEditor/Rendering/DebugModelRenderFeature.h"

OvEditor::Panels::AssetView::AssetView
(
	const std::string& p_title,
	bool p_opened,
	const OvUI::Settings::PanelWindowSettings& p_windowSettings
) : AViewControllable(p_title, p_opened, p_windowSettings)
{
	m_renderer = std::make_unique<OvCore::Rendering::SceneRenderer>(*EDITOR_CONTEXT(driver));

	m_renderer->AddFeature<OvEditor::Rendering::DebugModelRenderFeature>();
	m_renderer->AddFeature<OvRendering::Features::DebugShapeRenderFeature>();
	m_renderer->AddFeature<OvRendering::Features::FrameInfoRenderFeature>();

	m_renderer->AddPass<OvEditor::Rendering::GridRenderPass>("Grid", OvRendering::Settings::ERenderPassOrder::Debug);

	m_camera.SetFar(5000.0f);

	m_scene.AddDefaultLights();
	m_scene.AddDefaultPostProcessStack();
	m_scene.AddDefaultSkysphere();

	m_assetActor = &m_scene.CreateActor("Asset");
	m_modelRenderer = &m_assetActor->AddComponent<OvCore::ECS::Components::CModelRenderer>();
	m_modelRenderer->SetFrustumBehaviour(OvCore::ECS::Components::CModelRenderer::EFrustumBehaviour::DISABLED);
	m_materialRenderer = &m_assetActor->AddComponent<OvCore::ECS::Components::CMaterialRenderer>();

	m_cameraController.LockTargetActor(*m_assetActor);
	
	/* Default Material */
	m_defaultMaterial.SetShader(EDITOR_CONTEXT(shaderManager)[":Shaders\\StandardPBR.ovfx"]);
	m_defaultMaterial.SetProperty("u_Metallic", 0.0f);
	m_defaultMaterial.SetProperty("u_Roughness", 0.5f);

	/* Texture Material */
	m_textureMaterial.SetShader(EDITOR_CONTEXT(shaderManager)[":Shaders\\Unlit.ovfx"]);
	m_textureMaterial.SetProperty("u_Diffuse", OvMaths::FVector4(1.f, 1.f, 1.f, 1.f));
	m_textureMaterial.SetBackfaceCulling(false);
	m_textureMaterial.SetBlendable(true);
	m_textureMaterial.SetProperty("u_DiffuseMap", static_cast<OvRendering::Resources::Texture*>(nullptr));

	m_image->AddPlugin<OvUI::Plugins::DDTarget<std::pair<std::string, OvUI::Widgets::Layout::Group*>>>("File").DataReceivedEvent += [this](auto p_data)
	{
		std::string path = p_data.first;

		switch (OvTools::Utils::PathParser::GetFileType(path))
		{
		case OvTools::Utils::PathParser::EFileType::MODEL:
			if (auto resource = OvCore::Global::ServiceLocator::Get<OvCore::ResourceManagement::ModelManager>().GetResource(path); resource)
				SetModel(*resource);
			break;
		case OvTools::Utils::PathParser::EFileType::TEXTURE:
			if (auto resource = OvCore::Global::ServiceLocator::Get<OvCore::ResourceManagement::TextureManager>().GetResource(path); resource)
				SetTexture(*resource);
			break;

		case OvTools::Utils::PathParser::EFileType::MATERIAL:
			if (auto resource = OvCore::Global::ServiceLocator::Get<OvCore::ResourceManagement::MaterialManager>().GetResource(path); resource)
				SetMaterial(*resource);
			break;
		}
	};
}

OvCore::SceneSystem::Scene* OvEditor::Panels::AssetView::GetScene()
{
	return &m_scene;
}

void OvEditor::Panels::AssetView::SetResource(ViewableResource p_resource)
{
	if (auto pval = std::get_if<OvRendering::Resources::Model*>(&p_resource); pval && *pval)
	{
		SetModel(**pval);
	}
	else if (auto pval = std::get_if<OvRendering::Resources::Texture*>(&p_resource); pval && *pval)
	{
		SetTexture(**pval);
	}
	else if (auto pval = std::get_if<OvCore::Resources::Material*>(&p_resource); pval && *pval)
	{
		SetMaterial(**pval);
	}
}

void OvEditor::Panels::AssetView::ClearResource()
{
	m_resource = static_cast<OvRendering::Resources::Texture*>(nullptr);
	m_modelRenderer->SetModel(nullptr);
}

void OvEditor::Panels::AssetView::SetTexture(OvRendering::Resources::Texture& p_texture)
{
	m_resource = &p_texture;
	m_assetActor->transform.SetLocalRotation(OvMaths::FQuaternion({ -90.0f, 0.0f, 0.0f }));
	m_assetActor->transform.SetLocalScale(OvMaths::FVector3::One * 3.0f);
	m_modelRenderer->SetModel(EDITOR_CONTEXT(editorResources)->GetModel("Plane"));
	m_textureMaterial.SetProperty("u_DiffuseMap", &p_texture);
	m_materialRenderer->FillWithMaterial(m_textureMaterial);

	m_cameraController.MoveToTarget(*m_assetActor);
}

void OvEditor::Panels::AssetView::SetModel(OvRendering::Resources::Model& p_model)
{
	m_resource = &p_model;
	m_assetActor->transform.SetLocalRotation(OvMaths::FQuaternion::Identity);
	m_assetActor->transform.SetLocalScale(OvMaths::FVector3::One);
	m_modelRenderer->SetModel(&p_model);
	m_materialRenderer->FillWithMaterial(m_defaultMaterial);

	m_cameraController.MoveToTarget(*m_assetActor);
}

void OvEditor::Panels::AssetView::SetMaterial(OvCore::Resources::Material& p_material)
{
	m_resource = &p_material;
	m_assetActor->transform.SetLocalRotation(OvMaths::FQuaternion::Identity);
	m_assetActor->transform.SetLocalScale(OvMaths::FVector3::One);
	m_modelRenderer->SetModel(EDITOR_CONTEXT(editorResources)->GetModel("Sphere"));
	m_materialRenderer->FillWithMaterial(p_material);

	m_cameraController.MoveToTarget(*m_assetActor);
}

const OvEditor::Panels::AssetView::ViewableResource& OvEditor::Panels::AssetView::GetResource() const
{
	return m_resource;
}
