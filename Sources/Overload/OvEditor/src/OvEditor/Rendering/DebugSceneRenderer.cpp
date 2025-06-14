/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include <OvCore/ECS/Components/CCamera.h>
#include <OvCore/ECS/Components/CDirectionalLight.h>
#include <OvCore/ECS/Components/CMaterialRenderer.h>
#include <OvCore/ECS/Components/CPhysicalBox.h>
#include <OvCore/ECS/Components/CPhysicalCapsule.h>
#include <OvCore/ECS/Components/CPhysicalSphere.h>
#include <OvCore/ECS/Components/CPointLight.h>
#include <OvCore/ECS/Components/CSpotLight.h>
#include <OvCore/Rendering/EngineDrawableDescriptor.h>
#include <OvCore/Rendering/ReflectionRenderFeature.h>

#include <OvDebug/Assertion.h>

#include <OvEditor/Core/EditorActions.h>
#include <OvEditor/Core/EditorResources.h>
#include <OvEditor/Panels/AView.h>
#include <OvEditor/Panels/GameView.h>
#include <OvEditor/Rendering/DebugModelRenderFeature.h>
#include <OvEditor/Rendering/DebugSceneRenderer.h>
#include <OvEditor/Rendering/GizmoRenderFeature.h>
#include <OvEditor/Rendering/GridRenderPass.h>
#include <OvEditor/Rendering/OutlineRenderFeature.h>
#include <OvEditor/Rendering/PickingRenderPass.h>
#include <OvEditor/Settings/EditorSettings.h>

#include <OvRendering/Features/DebugShapeRenderFeature.h>
#include <OvRendering/Features/FrameInfoRenderFeature.h>
#include <OvRendering/Features/LightingRenderFeature.h>
#include <OvRendering/HAL/Profiling.h>

using namespace OvMaths;
using namespace OvRendering::Resources;
using namespace OvCore::Resources;

namespace
{
	const OvMaths::FVector3 kDebugBoundsColor = { 1.0f, 0.0f, 0.0f };
	const OvMaths::FVector3 kLightVolumeColor = { 1.0f, 1.0f, 0.0f };
	const OvMaths::FVector3 kColliderColor = { 0.0f, 1.0f, 0.0f };
	const OvMaths::FVector3 kFrustumColor = { 1.0f, 1.0f, 1.0f };

	const OvMaths::FVector4 kHoveredOutlineColor{ 1.0f, 1.0f, 0.0f, 1.0f };
	const OvMaths::FVector4 kSelectedOutlineColor{ 1.0f, 0.7f, 0.0f, 1.0f };

	constexpr float kHoveredOutlineWidth = 2.5f;
	constexpr float kSelectedOutlineWidth = 5.0f;

	OvMaths::FMatrix4 CalculateUnscaledModelMatrix(OvCore::ECS::Actor& p_actor)
	{
		auto translation = FMatrix4::Translation(p_actor.transform.GetWorldPosition());
		auto rotation = FQuaternion::ToMatrix4(p_actor.transform.GetWorldRotation());
		return translation * rotation;
	}

	std::optional<std::string> GetLightTypeTextureName(OvRendering::Settings::ELightType type)
	{
		using namespace OvRendering::Settings;

		switch (type)
		{
		case ELightType::POINT: return "Point_Light";
		case ELightType::SPOT: return "Spot_Light";
		case ELightType::DIRECTIONAL: return "Directional_Light";
		case ELightType::AMBIENT_BOX: return "Ambient_Box_Light";
		case ELightType::AMBIENT_SPHERE: return "Ambient_Sphere_Light";
		}

		return std::nullopt;
	}

	OvMaths::FMatrix4 CreateDebugDirectionalLight()
	{
		OvRendering::Entities::Light directionalLight{
			.intensity = 2.0f,
			.type = OvRendering::Settings::ELightType::DIRECTIONAL,
		};

		directionalLight.transform->SetLocalPosition({ 0.0f, 10.0f, 0.0f });
		directionalLight.transform->SetLocalRotation(OvMaths::FQuaternion({ 120.0f, -40.0f, 0.0f }));
		return directionalLight.GenerateMatrix();
	}

	OvMaths::FMatrix4 CreateDebugAmbientLight()
	{
		return OvRendering::Entities::Light{
			.intensity = 0.01f,
			.constant = 10000.0f, // radius
			.type = OvRendering::Settings::ELightType::AMBIENT_SPHERE
		}.GenerateMatrix();
	}

	std::unique_ptr<OvRendering::HAL::ShaderStorageBuffer> CreateDebugLightBuffer()
	{
		auto lightBuffer = std::make_unique<OvRendering::HAL::ShaderStorageBuffer>();

		const auto lightMatrices = std::to_array<OvMaths::FMatrix4>({
			CreateDebugDirectionalLight(),
			CreateDebugAmbientLight()
		});

		lightBuffer->Allocate(sizeof(lightMatrices), OvRendering::Settings::EAccessSpecifier::STATIC_READ);
		lightBuffer->Upload(lightMatrices.data());

		return lightBuffer;
	}
}

class DebugCamerasRenderPass : public OvRendering::Core::ARenderPass
{
public:
	DebugCamerasRenderPass(OvRendering::Core::CompositeRenderer& p_renderer) : OvRendering::Core::ARenderPass(p_renderer)
	{
		m_fakeLightsBuffer = CreateDebugLightBuffer();

		m_cameraMaterial.SetShader(EDITOR_CONTEXT(shaderManager)[":Shaders\\Standard.ovfx"]);
		m_cameraMaterial.SetProperty("u_Albedo", FVector4{ 0.0f, 0.447f, 1.0f, 1.0f });
		m_cameraMaterial.SetProperty("u_Metallic", 0.0f);
		m_cameraMaterial.SetProperty("u_Roughness", 0.25f);
		m_cameraMaterial.SetProperty("u_BuiltInGammaCorrection", true);
		m_cameraMaterial.SetProperty("u_BuiltInToneMapping", true);
	}

protected:
	virtual void Draw(OvRendering::Data::PipelineState p_pso) override
	{
		ZoneScoped;
		TracyGpuZone("DebugCamerasRenderPass");

		using namespace OvRendering::Features;

		const auto lightingRenderFeature = OvTools::Utils::OptRef<LightingRenderFeature>{
			m_renderer.HasFeature<LightingRenderFeature>() ?
			m_renderer.GetFeature<LightingRenderFeature>() :
			OvTools::Utils::OptRef<LightingRenderFeature>{std::nullopt}
		};

		// Override the light buffer with fake lights
		m_fakeLightsBuffer->Bind(
			lightingRenderFeature ?
			lightingRenderFeature->GetBufferBindingPoint() :
			0
		);

		auto& sceneDescriptor = m_renderer.GetDescriptor<OvCore::Rendering::SceneRenderer::SceneDescriptor>();

		for (auto camera : sceneDescriptor.scene.GetFastAccessComponents().cameras)
		{
			auto& actor = camera->owner;

			if (actor.IsActive())
			{
				auto& model = *EDITOR_CONTEXT(editorResources)->GetModel("Camera");
				auto modelMatrix = CalculateUnscaledModelMatrix(actor);

				m_renderer.GetFeature<OvEditor::Rendering::DebugModelRenderFeature>()
					.DrawModelWithSingleMaterial(p_pso, model, m_cameraMaterial, modelMatrix);
			}
		}

		if (lightingRenderFeature)
		{
			// Bind back the original light buffer
			lightingRenderFeature->Bind();
		}
	}

private:
	OvCore::Resources::Material m_cameraMaterial;
	std::unique_ptr<OvRendering::HAL::ShaderStorageBuffer> m_fakeLightsBuffer;
};

class DebugReflectionProbesRenderPass : public OvRendering::Core::ARenderPass
{
public:
	DebugReflectionProbesRenderPass(OvRendering::Core::CompositeRenderer& p_renderer) : OvRendering::Core::ARenderPass(p_renderer)
	{
		m_fakeLightsBuffer = CreateDebugLightBuffer();

		m_reflectiveMaterial.SetDepthTest(false);
		m_reflectiveMaterial.SetShader(EDITOR_CONTEXT(shaderManager)[":Shaders\\Standard.ovfx"]);
		m_reflectiveMaterial.SetProperty("u_Albedo", FVector4{ 1.0, 1.0f, 1.0f, 1.0f });
		m_reflectiveMaterial.SetProperty("u_Metallic", 1.0f);
		m_reflectiveMaterial.SetProperty("u_Roughness", 0.0f);
		m_reflectiveMaterial.SetProperty("u_BuiltInGammaCorrection", true);
		m_reflectiveMaterial.SetProperty("u_BuiltInToneMapping", true);
	}

protected:
	virtual void Draw(OvRendering::Data::PipelineState p_pso) override
	{
		ZoneScoped;
		TracyGpuZone("DebugReflectionProbesRenderPass");

		using namespace OvRendering::Features;

		const auto lightingRenderFeature = OvTools::Utils::OptRef<LightingRenderFeature>{
			m_renderer.HasFeature<LightingRenderFeature>() ?
			m_renderer.GetFeature<LightingRenderFeature>() :
			OvTools::Utils::OptRef<LightingRenderFeature>{std::nullopt}
		};

		// Override the light buffer with fake lights
		m_fakeLightsBuffer->Bind(
			lightingRenderFeature ?
			lightingRenderFeature->GetBufferBindingPoint() :
			0
		);

		auto& sceneDescriptor = m_renderer.GetDescriptor<OvCore::Rendering::SceneRenderer::SceneDescriptor>();
		auto& reflectionRenderFeature = m_renderer.GetFeature<OvCore::Rendering::ReflectionRenderFeature>();

		for (auto reflectionProbe : sceneDescriptor.scene.GetFastAccessComponents().reflectionProbes)
		{
			auto& actor = reflectionProbe->owner;

			if (actor.IsActive())
			{
				auto& model = *EDITOR_CONTEXT(editorResources)->GetModel("Sphere");
				auto modelMatrix =
					OvMaths::FMatrix4::Scale(
						OvMaths::FMatrix4::Translate(
							CalculateUnscaledModelMatrix(actor),
							reflectionProbe->GetCapturePosition()
						),
						OvMaths::FVector3::One * OvEditor::Settings::EditorSettings::ReflectionProbeScale
					);

				reflectionRenderFeature.PrepareProbe(*reflectionProbe);
				reflectionRenderFeature.SendProbeData(m_reflectiveMaterial, *reflectionProbe);
				reflectionRenderFeature.BindProbe(*reflectionProbe);

				m_renderer.GetFeature<OvEditor::Rendering::DebugModelRenderFeature>()
					.DrawModelWithSingleMaterial(p_pso, model, m_reflectiveMaterial, modelMatrix);
			}
		}

		if (lightingRenderFeature)
		{
			// Bind back the original light buffer
			lightingRenderFeature->Bind();
		}
	}

private:
	OvCore::Resources::Material m_reflectiveMaterial;
	std::unique_ptr<OvRendering::HAL::ShaderStorageBuffer> m_fakeLightsBuffer;
};

class DebugLightsRenderPass : public OvRendering::Core::ARenderPass
{
public:
	DebugLightsRenderPass(OvRendering::Core::CompositeRenderer& p_renderer) : OvRendering::Core::ARenderPass(p_renderer)
	{
		m_lightMaterial.SetShader(EDITOR_CONTEXT(editorResources)->GetShader("Billboard"));
		m_lightMaterial.SetProperty("u_Diffuse", FVector4{ 1.f, 1.f, 0.5f, 0.5f });
		m_lightMaterial.SetBackfaceCulling(false);
		m_lightMaterial.SetBlendable(true);
		m_lightMaterial.SetDepthTest(false);
	}

protected:
	virtual void Draw(OvRendering::Data::PipelineState p_pso) override
	{
		ZoneScoped;
		TracyGpuZone("DebugLightsRenderPass");

		auto& sceneDescriptor = m_renderer.GetDescriptor<OvCore::Rendering::SceneRenderer::SceneDescriptor>();

		m_lightMaterial.SetProperty("u_Scale", OvEditor::Settings::EditorSettings::LightBillboardScale * 0.1f);

		for (auto light : sceneDescriptor.scene.GetFastAccessComponents().lights)
		{
			auto& actor = light->owner;

			if (actor.IsActive())
			{
				auto& model = *EDITOR_CONTEXT(editorResources)->GetModel("Vertical_Plane");
				auto modelMatrix = OvMaths::FMatrix4::Translation(actor.transform.GetWorldPosition());

				auto lightTypeTextureName = GetLightTypeTextureName(light->GetData().type);

				auto lightTexture =
					lightTypeTextureName ?
					EDITOR_CONTEXT(editorResources)->GetTexture(lightTypeTextureName.value()) :
					nullptr;

				const auto& lightColor = light->GetColor();
				m_lightMaterial.SetProperty("u_DiffuseMap", lightTexture);
				m_lightMaterial.SetProperty("u_Diffuse", OvMaths::FVector4(lightColor.x, lightColor.y, lightColor.z, 0.75f));

				m_renderer.GetFeature<OvEditor::Rendering::DebugModelRenderFeature>()
					.DrawModelWithSingleMaterial(p_pso, model, m_lightMaterial, modelMatrix);
			}
		}
	}

private:
	OvCore::Resources::Material m_lightMaterial;
};

class DebugActorRenderPass : public OvRendering::Core::ARenderPass
{
public:
	DebugActorRenderPass(OvRendering::Core::CompositeRenderer& p_renderer) : OvRendering::Core::ARenderPass(p_renderer),
		m_debugShapeFeature(m_renderer.GetFeature<OvRendering::Features::DebugShapeRenderFeature>())
	{
		
	}

protected:
	OvRendering::Features::DebugShapeRenderFeature& m_debugShapeFeature;

	virtual void Draw(OvRendering::Data::PipelineState p_pso) override
	{
		ZoneScoped;
		TracyGpuZone("DebugActorRenderPass");

		// Clear stencil buffer for outline rendering
		m_renderer.Clear(false, false, true);

		auto& debugSceneDescriptor = m_renderer.GetDescriptor<OvEditor::Rendering::DebugSceneRenderer::DebugSceneDescriptor>();

		if (debugSceneDescriptor.selectedActor)
		{
			auto& selectedActor = debugSceneDescriptor.selectedActor.value();
			const bool isActorHovered = debugSceneDescriptor.highlightedActor && debugSceneDescriptor.highlightedActor->GetID() == selectedActor.GetID();

			DrawActorDebugElements(selectedActor);
			m_renderer.GetFeature<OvEditor::Rendering::OutlineRenderFeature>().DrawOutline(
				selectedActor,
				isActorHovered ?
				kHoveredOutlineColor :
				kSelectedOutlineColor,
				kSelectedOutlineWidth
			);
			m_renderer.Clear(false, true, false, OvMaths::FVector3::Zero);
			m_renderer.GetFeature<OvEditor::Rendering::GizmoRenderFeature>().DrawGizmo(
				selectedActor.transform.GetWorldPosition(),
				selectedActor.transform.GetWorldRotation(),
				debugSceneDescriptor.gizmoOperation,
				false,
				debugSceneDescriptor.highlightedGizmoDirection
			);
		}
		
		if (debugSceneDescriptor.highlightedActor)
		{
			auto& highlightedActor = debugSceneDescriptor.highlightedActor.value();

			// Render the outline only if the actor is not already selected (as its outline render should have been handled already).
			if (!debugSceneDescriptor.selectedActor || highlightedActor.GetID() != debugSceneDescriptor.selectedActor->GetID())
			{
				m_renderer.GetFeature<OvEditor::Rendering::OutlineRenderFeature>().DrawOutline(highlightedActor, kHoveredOutlineColor, kHoveredOutlineWidth);
			}
		}
	}

	void DrawActorDebugElements(OvCore::ECS::Actor& p_actor)
	{
		if (p_actor.IsActive())
		{
			/* Render static mesh outline and bounding spheres */
			if (OvEditor::Settings::EditorSettings::ShowGeometryBounds)
			{
				auto modelRenderer = p_actor.GetComponent<OvCore::ECS::Components::CModelRenderer>();

				if (modelRenderer && modelRenderer->GetModel())
				{
					DrawBoundingSpheres(*modelRenderer);
				}
			}

			/* Render camera component frustum */
			if (auto cameraComponent = p_actor.GetComponent<OvCore::ECS::Components::CCamera>(); cameraComponent)
			{
				DrawCameraFrustum(*cameraComponent);
			}

			/* Render camera component frustum */
			if (auto reflectionProbeComponent = p_actor.GetComponent<OvCore::ECS::Components::CReflectionProbe>(); reflectionProbeComponent)
			{
				if (reflectionProbeComponent->GetInfluencePolicy() == OvCore::ECS::Components::CReflectionProbe::EInfluencePolicy::LOCAL)
				{
					DrawReflectionProbeInfluenceVolume(*reflectionProbeComponent);
				}
			}

			/* Render the actor collider */
			if (p_actor.GetComponent<OvCore::ECS::Components::CPhysicalObject>())
			{
				DrawActorCollider(p_actor);
			}

			/* Render the actor ambient light */
			if (auto ambientBoxComp = p_actor.GetComponent<OvCore::ECS::Components::CAmbientBoxLight>())
			{
				DrawAmbientBoxVolume(*ambientBoxComp);
			}

			if (auto ambientSphereComp = p_actor.GetComponent<OvCore::ECS::Components::CAmbientSphereLight>())
			{
				DrawAmbientSphereVolume(*ambientSphereComp);
			}

			if (OvEditor::Settings::EditorSettings::ShowLightBounds)
			{
				if (auto light = p_actor.GetComponent<OvCore::ECS::Components::CLight>())
				{
					DrawLightBounds(*light);
				}
			}

			for (auto& child : p_actor.GetChildren())
			{
				DrawActorDebugElements(*child);
			}
		}
	}

	void DrawFrustumLines(
		const OvMaths::FVector3& pos,
		const OvMaths::FVector3& forward,
		float near,
		const float far,
		const OvMaths::FVector3& a,
		const OvMaths::FVector3& b,
		const OvMaths::FVector3& c,
		const OvMaths::FVector3& d,
		const OvMaths::FVector3& e,
		const OvMaths::FVector3& f,
		const OvMaths::FVector3& g,
		const OvMaths::FVector3& h
	)
	{
		auto pso = m_renderer.CreatePipelineState();

		// Convenient lambda to draw a frustum line
		auto draw = [&](const FVector3& p_start, const FVector3& p_end, const float planeDistance) {
			auto offset = pos + forward * planeDistance;
			auto start = offset + p_start;
			auto end = offset + p_end;
			m_debugShapeFeature.DrawLine(pso, start, end, kFrustumColor, 1.0f, false);
		};

		// Draw near plane
		draw(a, b, near);
		draw(b, d, near);
		draw(d, c, near);
		draw(c, a, near);

		// Draw far plane
		draw(e, f, far);
		draw(f, h, far);
		draw(h, g, far);
		draw(g, e, far);

		// Draw lines between near and far planes
		draw(a + forward * near, e + forward * far, 0);
		draw(b + forward * near, f + forward * far, 0);
		draw(c + forward * near, g + forward * far, 0);
		draw(d + forward * near, h + forward * far, 0);
	}

	void DrawCameraPerspectiveFrustum(std::pair<uint16_t, uint16_t>& p_size, OvCore::ECS::Components::CCamera& p_camera)
	{
		const auto& owner = p_camera.owner;
		auto& camera = p_camera.GetCamera();

		const auto& cameraPos = owner.transform.GetWorldPosition();
		const auto& cameraRotation = owner.transform.GetWorldRotation();
		const auto& cameraForward = owner.transform.GetWorldForward();

		camera.CacheMatrices(p_size.first, p_size.second); // TODO: We shouldn't cache matrices mid air, we could use another function to get the matrices/calculate
		const auto proj = FMatrix4::Transpose(camera.GetProjectionMatrix());
		const auto near = camera.GetNear();
		const auto far = camera.GetFar();

		const auto nLeft = near * (proj.data[2] - 1.0f) / proj.data[0];
		const auto nRight = near * (1.0f + proj.data[2]) / proj.data[0];
		const auto nTop = near * (1.0f + proj.data[6]) / proj.data[5];
		const auto nBottom = near * (proj.data[6] - 1.0f) / proj.data[5];

		const auto fLeft = far * (proj.data[2] - 1.0f) / proj.data[0];
		const auto fRight = far * (1.0f + proj.data[2]) / proj.data[0];
		const auto fTop = far * (1.0f + proj.data[6]) / proj.data[5];
		const auto fBottom = far * (proj.data[6] - 1.0f) / proj.data[5];

		auto a = cameraRotation * FVector3{ nLeft, nTop, 0 };
		auto b = cameraRotation * FVector3{ nRight, nTop, 0 };
		auto c = cameraRotation * FVector3{ nLeft, nBottom, 0 };
		auto d = cameraRotation * FVector3{ nRight, nBottom, 0 };
		auto e = cameraRotation * FVector3{ fLeft, fTop, 0 };
		auto f = cameraRotation * FVector3{ fRight, fTop, 0 };
		auto g = cameraRotation * FVector3{ fLeft, fBottom, 0 };
		auto h = cameraRotation * FVector3{ fRight, fBottom, 0 };

		DrawFrustumLines(cameraPos, cameraForward, near, far, a, b, c, d, e, f, g, h);
	}

	void DrawCameraOrthographicFrustum(std::pair<uint16_t, uint16_t>& p_size, OvCore::ECS::Components::CCamera& p_camera)
	{
		auto& owner = p_camera.owner;
		auto& camera = p_camera.GetCamera();
		const auto ratio = p_size.first / static_cast<float>(p_size.second);

		const auto& cameraPos = owner.transform.GetWorldPosition();
		const auto& cameraRotation = owner.transform.GetWorldRotation();
		const auto& cameraForward = owner.transform.GetWorldForward();

		const auto near = camera.GetNear();
		const auto far = camera.GetFar();
		const auto size = p_camera.GetSize();

		const auto right = ratio * size;
		const auto left = -right;
		const auto top = size;
		const auto bottom = -top;

		const auto a = cameraRotation * FVector3{ left, top, 0 };
		const auto b = cameraRotation * FVector3{ right, top, 0 };
		const auto c = cameraRotation * FVector3{ left, bottom, 0 };
		const auto d = cameraRotation * FVector3{ right, bottom, 0 };

		DrawFrustumLines(cameraPos, cameraForward, near, far, a, b, c, d, a, b, c, d);
	}

	void DrawCameraFrustum(OvCore::ECS::Components::CCamera& p_camera)
	{
		auto& gameView = EDITOR_PANEL(OvEditor::Panels::GameView, "Game View");
		auto gameViewSize = gameView.GetSafeSize();

		if (gameViewSize.first == 0 || gameViewSize.second == 0)
		{
			gameViewSize = { 16, 9 };
		}

		switch (p_camera.GetProjectionMode())
		{
		case OvRendering::Settings::EProjectionMode::ORTHOGRAPHIC:
			DrawCameraOrthographicFrustum(gameViewSize, p_camera);
			break;

		case OvRendering::Settings::EProjectionMode::PERSPECTIVE:
			DrawCameraPerspectiveFrustum(gameViewSize, p_camera);
			break;
		}
	}

	void DrawReflectionProbeInfluenceVolume(OvCore::ECS::Components::CReflectionProbe& p_reflectionProbe)
	{
		auto pso = m_renderer.CreatePipelineState();
		pso.depthTest = false;
		const auto& size = p_reflectionProbe.GetInfluenceSize();
		const auto position = p_reflectionProbe.owner.transform.GetWorldPosition();
		m_debugShapeFeature.DrawBox(
			pso,
			position,
			p_reflectionProbe.owner.transform.GetWorldRotation(),
			size,
			kDebugBoundsColor,
			1.0f,
			false
		);
	}

	void DrawActorCollider(OvCore::ECS::Actor& p_actor)
	{
		using namespace OvCore::ECS::Components;
		using namespace OvPhysics::Entities;

		auto pso = m_renderer.CreatePipelineState();
		pso.depthTest = false;

		/* Draw the box collider if any */
		if (auto boxColliderComponent = p_actor.GetComponent<OvCore::ECS::Components::CPhysicalBox>(); boxColliderComponent)
		{
			m_debugShapeFeature.DrawBox(
				pso,
				p_actor.transform.GetWorldPosition(),
				p_actor.transform.GetWorldRotation(),
				boxColliderComponent->GetSize() * p_actor.transform.GetWorldScale(),
				OvMaths::FVector3{ 0.f, 1.f, 0.f },
				1.0f,
				false
			);
		}

		/* Draw the sphere collider if any */
		if (auto sphereColliderComponent = p_actor.GetComponent<OvCore::ECS::Components::CPhysicalSphere>(); sphereColliderComponent)
		{
			FVector3 actorScale = p_actor.transform.GetWorldScale();
			float radius = sphereColliderComponent->GetRadius() * std::max(std::max(std::max(actorScale.x, actorScale.y), actorScale.z), 0.0f);

			m_debugShapeFeature.DrawSphere(
				pso,
				p_actor.transform.GetWorldPosition(),
				p_actor.transform.GetWorldRotation(),
				radius,
				OvMaths::FVector3{ 0.f, 1.f, 0.f },
				1.0f,
				false
			);
		}

		/* Draw the capsule collider if any */
		if (auto capsuleColliderComponent = p_actor.GetComponent<OvCore::ECS::Components::CPhysicalCapsule>(); capsuleColliderComponent)
		{
			FVector3 actorScale = p_actor.transform.GetWorldScale();
			float radius = abs(capsuleColliderComponent->GetRadius() * std::max(std::max(actorScale.x, actorScale.z), 0.f));
			float height = abs(capsuleColliderComponent->GetHeight() * actorScale.y);

			m_debugShapeFeature.DrawCapsule(
				pso,
				p_actor.transform.GetWorldPosition(),
				p_actor.transform.GetWorldRotation(),
				radius,
				height,
				OvMaths::FVector3{ 0.f, 1.f, 0.f },
				1.0f,
				false
			);
		}
	}

	void DrawLightBounds(OvCore::ECS::Components::CLight& p_light)
	{
		auto pso = m_renderer.CreatePipelineState();
		pso.depthTest = false;

		auto& data = p_light.GetData();

		m_debugShapeFeature.DrawSphere(
			pso,
			data.transform->GetWorldPosition(),
			data.transform->GetWorldRotation(),
			data.CalculateEffectRange(),
			kDebugBoundsColor,
			1.0f,
			false
		);
	}

	void DrawAmbientBoxVolume(OvCore::ECS::Components::CAmbientBoxLight& p_ambientBoxLight)
	{
		auto pso = m_renderer.CreatePipelineState();
		pso.depthTest = false;

		auto& data = p_ambientBoxLight.GetData();

		m_debugShapeFeature.DrawBox(
			pso,
			p_ambientBoxLight.owner.transform.GetWorldPosition(),
			data.transform->GetWorldRotation(),
			{ data.constant, data.linear, data.quadratic },
			data.CalculateEffectRange(),
			1.0f,
			false
		);
	}

	void DrawAmbientSphereVolume(OvCore::ECS::Components::CAmbientSphereLight& p_ambientSphereLight)
	{
		auto pso = m_renderer.CreatePipelineState();
		pso.depthTest = false;

		auto& data = p_ambientSphereLight.GetData();

		m_debugShapeFeature.DrawSphere(
			pso,
			p_ambientSphereLight.owner.transform.GetWorldPosition(),
			p_ambientSphereLight.owner.transform.GetWorldRotation(),
			data.constant,
			kLightVolumeColor,
			1.0f,
			false
		);
	}

	void DrawBoundingSpheres(OvCore::ECS::Components::CModelRenderer& p_modelRenderer)
	{
		using namespace OvCore::ECS::Components;
		using namespace OvPhysics::Entities;
		using enum OvCore::ECS::Components::CModelRenderer::EFrustumBehaviour;

		auto pso = m_renderer.CreatePipelineState();

		const auto frustumBehaviour = p_modelRenderer.GetFrustumBehaviour();
		
		if (frustumBehaviour == DISABLED)
		{
			return; // No bounds to draw
		}

		// Draw the mesh, model, or custom bounding sphere
		if (auto model = p_modelRenderer.GetModel())
		{
			auto& actor = p_modelRenderer.owner;

			const auto& actorScale = actor.transform.GetWorldScale();
			const auto& actorRotation = actor.transform.GetWorldRotation();
			const auto& actorPosition = actor.transform.GetWorldPosition();

			const float radiusScale = std::max(std::max(std::max(actorScale.x, actorScale.y), actorScale.z), 0.0f);

			auto drawBounds = [&](const OvRendering::Geometry::BoundingSphere& p_bounds) {
				const float scaledRadius = p_bounds.radius * radiusScale;
				const auto sphereOffset = OvMaths::FQuaternion::RotatePoint(
					p_bounds.position,
					actorRotation
				) * radiusScale;

				m_debugShapeFeature.DrawSphere(
					pso,
					actorPosition + sphereOffset,
					actorRotation,
					scaledRadius,
					kDebugBoundsColor,
					1.0f,
					false
				);
			};

			if (frustumBehaviour == MESH_BOUNDS)
			{
				for (auto mesh : model->GetMeshes())
				{
					drawBounds(mesh->GetBoundingSphere());
				}
			}
			else
			{
				drawBounds(
					frustumBehaviour == CUSTOM_BOUNDS ?
					p_modelRenderer.GetCustomBoundingSphere() :
					model->GetBoundingSphere()
				);
			}
		}
	}
};

OvEditor::Rendering::DebugSceneRenderer::DebugSceneRenderer(OvRendering::Context::Driver& p_driver) :
	OvCore::Rendering::SceneRenderer(p_driver, true /* enable stencil write, required by the grid */)
{
	using namespace OvRendering::Features;
	using namespace OvEditor::Rendering;
	using namespace OvRendering::Settings;
	using enum OvRendering::Features::EFeatureExecutionPolicy;

	AddFeature<FrameInfoRenderFeature, ALWAYS>();
	AddFeature<DebugShapeRenderFeature, FRAME_EVENTS_ONLY>();
	AddFeature<DebugModelRenderFeature, NEVER>();
	AddFeature<OutlineRenderFeature, NEVER>();
	AddFeature<GizmoRenderFeature, NEVER>();

	AddPass<GridRenderPass>("Grid", ERenderPassOrder::Debug);
	AddPass<DebugCamerasRenderPass>("Debug Cameras", ERenderPassOrder::Debug);
	AddPass<DebugReflectionProbesRenderPass>("Debug Reflection Probes", ERenderPassOrder::Debug);
	AddPass<DebugLightsRenderPass>("Debug Lights", ERenderPassOrder::Debug);
	AddPass<DebugActorRenderPass>("Debug Actor", ERenderPassOrder::Debug);
	AddPass<PickingRenderPass>("Picking", ERenderPassOrder::Debug);
}
