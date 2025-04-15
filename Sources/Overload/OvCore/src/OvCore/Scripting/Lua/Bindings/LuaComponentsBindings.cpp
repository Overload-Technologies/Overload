/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include "OvCore/ECS/Actor.h"

#include "OvCore/ECS/Components/CTransform.h"
#include "OvCore/ECS/Components/CCamera.h"
#include "OvCore/ECS/Components/CPhysicalBox.h"
#include "OvCore/ECS/Components/CPhysicalSphere.h"
#include "OvCore/ECS/Components/CPhysicalCapsule.h"
#include "OvCore/ECS/Components/CDirectionalLight.h"
#include "OvCore/ECS/Components/CPointLight.h"
#include "OvCore/ECS/Components/CSpotLight.h"
#include "OvCore/ECS/Components/CAmbientBoxLight.h"
#include "OvCore/ECS/Components/CAmbientSphereLight.h"
#include "OvCore/ECS/Components/CModelRenderer.h"
#include "OvCore/ECS/Components/CMaterialRenderer.h"
#include "OvCore/ECS/Components/CAudioSource.h"
#include "OvCore/ECS/Components/CAudioListener.h"
#include "OvCore/ECS/Components/CPostProcessStack.h"

#include <sol/sol.hpp>

void BindLuaComponents(sol::state& p_luaState)
{
	using namespace OvMaths;
	using namespace OvCore::ECS;
	using namespace OvCore::ECS::Components;

	p_luaState.new_usertype<AComponent>("Component",
		"GetOwner", [](AComponent& p_component) -> Actor& { return p_component.owner; }
	);

	p_luaState.new_usertype<CTransform>("Transform",
		sol::base_classes, sol::bases<AComponent>(),
		"SetPosition", &CTransform::SetLocalPosition,
		"SetRotation", &CTransform::SetLocalRotation,
		"SetScale", &CTransform::SetLocalScale,
		"SetLocalPosition", &CTransform::SetLocalPosition,
		"SetLocalRotation", &CTransform::SetLocalRotation,
		"SetLocalScale", &CTransform::SetLocalScale,
		"SetWorldPosition", &CTransform::SetWorldPosition,
		"SetWorldRotation", &CTransform::SetWorldRotation,
		"SetWorldScale", &CTransform::SetWorldScale,
		"GetPosition", [](CTransform& p_this) -> FVector3 { return p_this.GetWorldPosition(); },
		"GetRotation", [](CTransform& p_this) -> FQuaternion { return p_this.GetWorldRotation(); },
		"GetScale", [](CTransform& p_this) -> FVector3 { return p_this.GetWorldScale(); },
		"GetLocalPosition", [](CTransform& p_this) -> FVector3 { return p_this.GetLocalPosition(); },
		"GetLocalRotation", [](CTransform& p_this) -> FQuaternion { return p_this.GetLocalRotation(); },
		"GetLocalScale", [](CTransform& p_this) -> FVector3 { return p_this.GetLocalScale(); },
		"GetWorldPosition", [](CTransform& p_this) -> FVector3 { return p_this.GetWorldPosition(); },
		"GetWorldRotation", [](CTransform& p_this) -> FQuaternion { return p_this.GetWorldRotation(); },
		"GetWorldScale", [](CTransform& p_this) -> FVector3 { return p_this.GetWorldScale(); },
		"GetForward", &CTransform::GetWorldForward,
		"GetUp", &CTransform::GetWorldUp,
		"GetRight", &CTransform::GetWorldRight,
		"GetLocalForward", &CTransform::GetLocalForward,
		"GetLocalUp", &CTransform::GetLocalUp,
		"GetLocalRight", &CTransform::GetLocalRight,
		"GetWorldForward", &CTransform::GetWorldForward,
		"GetWorldUp", &CTransform::GetWorldUp,
		"GetWorldRight", &CTransform::GetWorldRight
	);

	p_luaState.new_enum<OvCore::ECS::Components::CModelRenderer::EFrustumBehaviour>("FrustumBehaviour", {
		{"DISABLED", OvCore::ECS::Components::CModelRenderer::EFrustumBehaviour::DISABLED},
		{"CULL_MODEL", OvCore::ECS::Components::CModelRenderer::EFrustumBehaviour::CULL_MODEL},
		{"CULL_MESHES", OvCore::ECS::Components::CModelRenderer::EFrustumBehaviour::CULL_MESHES},
		{"CULL_CUSTOM", OvCore::ECS::Components::CModelRenderer::EFrustumBehaviour::CULL_CUSTOM}
	});

	p_luaState.new_usertype<CModelRenderer>("ModelRenderer",
		sol::base_classes, sol::bases<AComponent>(),
		"GetModel", &CModelRenderer::GetModel,
		"SetModel", &CModelRenderer::SetModel,
		"GetFrustumBehaviour", &CModelRenderer::GetFrustumBehaviour,
		"SetFrustumBehaviour", &CModelRenderer::SetFrustumBehaviour
	);

	p_luaState.new_usertype<CMaterialRenderer>("MaterialRenderer",
		sol::base_classes, sol::bases<AComponent>(),
		"SetMaterial", &CMaterialRenderer::SetMaterialAtIndex,
		"SetUserMatrixElement", &CMaterialRenderer::SetUserMatrixElement,
		"GetUserMatrixElement", &CMaterialRenderer::GetUserMatrixElement
	);

	p_luaState.new_enum<OvPhysics::Entities::PhysicalObject::ECollisionDetectionMode>("CollisionDetectionMode", {
		{"DISCRETE", OvPhysics::Entities::PhysicalObject::ECollisionDetectionMode::DISCRETE},
		{"CONTINUOUS", OvPhysics::Entities::PhysicalObject::ECollisionDetectionMode::CONTINUOUS}
	});

	p_luaState.new_usertype<CPhysicalObject>("PhysicalObject",
		sol::base_classes, sol::bases<AComponent>(),
		"GetMass", &CPhysicalObject::GetMass,
		"SetMass", &CPhysicalObject::SetMass,
		"GetFriction", &CPhysicalObject::GetFriction,
		"SetFriction", &CPhysicalObject::SetFriction,
		"GetBounciness", &CPhysicalObject::GetBounciness,
		"SetBounciness", &CPhysicalObject::SetBounciness,
		"SetLinearVelocity", &CPhysicalObject::SetLinearVelocity,
		"SetAngularVelocity", &CPhysicalObject::SetAngularVelocity,
		"GetLinearVelocity", &CPhysicalObject::GetLinearVelocity,
		"GetAngularVelocity", &CPhysicalObject::GetAngularVelocity,
		"SetLinearFactor", &CPhysicalObject::SetLinearFactor,
		"SetAngularFactor", &CPhysicalObject::SetAngularFactor,
		"GetLinearFactor", &CPhysicalObject::GetLinearFactor,
		"GetAngularFactor", &CPhysicalObject::GetAngularFactor,
		"IsTrigger", &CPhysicalObject::IsTrigger,
		"SetTrigger", &CPhysicalObject::SetTrigger,
		"AddForce", &CPhysicalObject::AddForce,
		"AddImpulse", &CPhysicalObject::AddImpulse,
		"ClearForces", &CPhysicalObject::ClearForces,
		"SetCollisionDetectionMode", &CPhysicalObject::SetCollisionDetectionMode,
		"GetCollisionMode", &CPhysicalObject::GetCollisionDetectionMode,
		"SetKinematic", &CPhysicalObject::SetKinematic
		);

	p_luaState.new_usertype<CPhysicalBox>("PhysicalBox",
		sol::base_classes, sol::bases<CPhysicalObject>(),
		"GetSize", &CPhysicalBox::GetSize,
		"SetSize", &CPhysicalBox::SetSize
	);

	p_luaState.new_usertype<CPhysicalSphere>("PhysicalSphere",
		sol::base_classes, sol::bases<CPhysicalObject>(),
		"GetRadius", &CPhysicalSphere::GetRadius,
		"SetRadius", &CPhysicalSphere::SetRadius
	);

	p_luaState.new_usertype<CPhysicalCapsule>("PhysicalCapsule",
		sol::base_classes, sol::bases<CPhysicalObject>(),
		"GetRadius", &CPhysicalCapsule::GetRadius,
		"SetRadius", &CPhysicalCapsule::SetRadius,
		"GetHeight", &CPhysicalCapsule::GetHeight,
		"SetHeight", &CPhysicalCapsule::SetHeight
	);

	p_luaState.new_enum<OvRendering::Settings::EProjectionMode>("ProjectionMode", {
		{"ORTHOGRAPHIC", OvRendering::Settings::EProjectionMode::ORTHOGRAPHIC},
		{"PERSPECTIVE", OvRendering::Settings::EProjectionMode::PERSPECTIVE}
	});

	p_luaState.new_usertype<CCamera>("Camera",
		sol::base_classes, sol::bases<AComponent>(),
		"GetFov", &CCamera::GetFov,
		"GetSize", &CCamera::GetSize,
		"GetNear", &CCamera::GetNear,
		"GetFar", &CCamera::GetFar,
		"GetClearColor", &CCamera::GetClearColor,
		"SetFov", &CCamera::SetFov,
		"SetSize", &CCamera::SetSize,
		"SetNear", &CCamera::SetNear,
		"SetFar", &CCamera::SetFar,
		"SetClearColor", &CCamera::SetClearColor,
		"HasFrustumGeometryCulling", &CCamera::HasFrustumGeometryCulling,
		"HasFrustumLightCulling", &CCamera::HasFrustumLightCulling,
		"GetProjectionMode", &CCamera::GetProjectionMode,
		"SetFrustumGeometryCulling", &CCamera::SetFrustumGeometryCulling,
		"SetFrustumLightCulling", &CCamera::SetFrustumLightCulling,
		"SetProjectionMode", &CCamera::SetProjectionMode
	);

	p_luaState.new_usertype<CLight>("Light",
		sol::base_classes, sol::bases<AComponent>(),
		"GetColor", &CPointLight::GetColor,
		"GetIntensity", &CPointLight::GetIntensity,
		"SetColor", &CPointLight::SetColor,
		"SetIntensity", &CPointLight::SetIntensity
	);

	p_luaState.new_usertype<CPointLight>("PointLight",
		sol::base_classes, sol::bases<CLight>(),
		"GetConstant", &CPointLight::GetConstant,
		"GetLinear", &CPointLight::GetLinear,
		"GetQuadratic", &CPointLight::GetQuadratic,
		"SetConstant", &CPointLight::SetConstant,
		"SetLinear", &CPointLight::SetLinear,
		"SetQuadratic", &CPointLight::SetQuadratic
	);

	p_luaState.new_usertype<CSpotLight>("SpotLight",
		sol::base_classes, sol::bases<CLight>(),
		"GetConstant", &CSpotLight::GetConstant,
		"GetLinear", &CSpotLight::GetLinear,
		"GetQuadratic", &CSpotLight::GetQuadratic,
		"GetCutOff", &CSpotLight::GetCutoff,
		"GetOuterCutOff", &CSpotLight::GetOuterCutoff,
		"SetConstant", &CSpotLight::SetConstant,
		"SetLinear", &CSpotLight::SetLinear,
		"SetQuadratic", &CSpotLight::SetQuadratic,
		"SetCutOff", &CSpotLight::SetCutoff,
		"SetOuterCutOff", &CSpotLight::SetOuterCutoff
	);

	p_luaState.new_usertype<CAmbientBoxLight>("AmbientBoxLight",
		sol::base_classes, sol::bases<CLight>(),
		"GetSize", &CAmbientBoxLight::GetSize,
		"SetSize", &CAmbientBoxLight::SetSize
	);

	p_luaState.new_usertype<CAmbientSphereLight>("AmbientSphereLight",
		sol::base_classes, sol::bases<CLight>(),
		"GetRadius", &CAmbientSphereLight::GetRadius,
		"SetRadius", &CAmbientSphereLight::SetRadius
	);

	p_luaState.new_usertype<CDirectionalLight>("DirectionalLight",
		sol::base_classes, sol::bases<CLight>(),
		"GetCastShadow", &CDirectionalLight::GetCastShadows,
		"SetCastShadow", &CDirectionalLight::SetCastShadows,
		"GetShadowAreaSize", &CDirectionalLight::GetShadowAreaSize,
		"SetShadowAreaSize", &CDirectionalLight::SetShadowAreaSize,
		"GetShadowFollowCamera", &CDirectionalLight::GetShadowFollowCamera,
		"SetShadowFollowCamera", &CDirectionalLight::SetShadowFollowCamera,
		"GetShadowMapResolution", &CDirectionalLight::GetShadowMapResolution,
		"SetShadowMapResolution", &CDirectionalLight::SetShadowMapResolution
	);

	p_luaState.new_usertype<CAudioSource>("AudioSource",
		sol::base_classes, sol::bases<AComponent>(),
		"Play", &CAudioSource::Play,
		"Stop", &CAudioSource::Stop,
		"Pause", &CAudioSource::Pause,
		"Resume", &CAudioSource::Resume,
		"GetSound", &CAudioSource::GetSound,
		"GetVolume", &CAudioSource::GetVolume,
		"GetPan", &CAudioSource::GetPan,
		"IsLooped", &CAudioSource::IsLooped,
		"GetPitch", &CAudioSource::GetPitch,
		"IsPlaying", &CAudioSource::IsPlaying,
		"IsSpatial", &CAudioSource::IsSpatial,
		"GetAttenuationThreshold", &CAudioSource::GetAttenuationThreshold,
		"SetSound", &CAudioSource::SetSound,
		"SetVolume", &CAudioSource::SetVolume,
		"SetPan", &CAudioSource::SetPan,
		"SetLooped", &CAudioSource::SetLooped,
		"SetPitch", &CAudioSource::SetPitch,
		"SetSpatial", &CAudioSource::SetSpatial,
		"SetAttenuationThreshold", &CAudioSource::SetAttenuationThreshold
	);

	p_luaState.new_usertype<CAudioListener>("AudioListener",
		sol::base_classes, sol::bases<AComponent>()
	);

	p_luaState.new_usertype<OvCore::Rendering::PostProcess::EffectSettings>("EffectSettings",
		"Enabled", &OvCore::Rendering::PostProcess::EffectSettings::enabled
	);

	p_luaState.new_usertype<OvCore::Rendering::PostProcess::BloomSettings>("BloomSettings",
		sol::base_classes, sol::bases<OvCore::Rendering::PostProcess::EffectSettings>(),
		"Threshold", &OvCore::Rendering::PostProcess::BloomSettings::threshold,
		"KernelSize", &OvCore::Rendering::PostProcess::BloomSettings::kernelSize,
		"Radius", &OvCore::Rendering::PostProcess::BloomSettings::radius,
		"Intensity", &OvCore::Rendering::PostProcess::BloomSettings::intensity,
		"Passes", &OvCore::Rendering::PostProcess::BloomSettings::passes
	);

	p_luaState.new_usertype<OvCore::Rendering::PostProcess::AutoExposureSettings>("AutoExposureSettings",
		sol::base_classes, sol::bases<OvCore::Rendering::PostProcess::EffectSettings>(),
		"CenterWeightBias", &OvCore::Rendering::PostProcess::AutoExposureSettings::centerWeightBias,
		"MinLuminanceEV", &OvCore::Rendering::PostProcess::AutoExposureSettings::minLuminanceEV,
		"MaxLuminanceEV", &OvCore::Rendering::PostProcess::AutoExposureSettings::maxLuminanceEV,
		"ExposureCompensationEV", &OvCore::Rendering::PostProcess::AutoExposureSettings::exposureCompensationEV,
		"Progressive", &OvCore::Rendering::PostProcess::AutoExposureSettings::progressive,
		"SpeedDown", &OvCore::Rendering::PostProcess::AutoExposureSettings::speedDown,
		"SpeedUp", &OvCore::Rendering::PostProcess::AutoExposureSettings::speedUp
	);

	p_luaState.new_enum<OvCore::Rendering::PostProcess::ETonemappingMode>("TonemappingMode", {
		{"NEUTRAL", OvCore::Rendering::PostProcess::ETonemappingMode::NEUTRAL},
		{"REINHARD", OvCore::Rendering::PostProcess::ETonemappingMode::REINHARD},
		{"REINHARD_JODIE", OvCore::Rendering::PostProcess::ETonemappingMode::REINHARD_JODIE},
		{"UNCHARTED2", OvCore::Rendering::PostProcess::ETonemappingMode::UNCHARTED2},
		{"UNCHARTED2_FILMIC", OvCore::Rendering::PostProcess::ETonemappingMode::UNCHARTED2_FILMIC},
		{"ACES", OvCore::Rendering::PostProcess::ETonemappingMode::ACES}
	});

	p_luaState.new_usertype<OvCore::Rendering::PostProcess::TonemappingSettings>("TonemappingSettings",
		sol::base_classes, sol::bases<OvCore::Rendering::PostProcess::EffectSettings>(),
		"Exposure", &OvCore::Rendering::PostProcess::TonemappingSettings::exposure,
		"Mode", &OvCore::Rendering::PostProcess::TonemappingSettings::mode,
		"GammaCorrection", &OvCore::Rendering::PostProcess::TonemappingSettings::gammaCorrection
	);

	p_luaState.new_usertype<OvCore::Rendering::PostProcess::FXAASettings>("FXAASettings",
		sol::base_classes, sol::bases<OvCore::Rendering::PostProcess::EffectSettings>()
	);

	p_luaState.new_usertype<CPostProcessStack>("PostProcessStack",
		sol::base_classes, sol::bases<AComponent>(),
		"GetTonemappingSettings", &CPostProcessStack::GetTonemappingSettings,
		"GetBloomSettings", &CPostProcessStack::GetBloomSettings,
		"GetAutoExposureSettings", &CPostProcessStack::GetAutoExposureSettings,
		"GetFXAASettings", &CPostProcessStack::GetFXAASettings,
		"SetTonemappingSettings", &CPostProcessStack::SetTonemappingSettings,
		"SetBloomSettings", &CPostProcessStack::SetBloomSettings,
		"SetAutoExposureSettings", &CPostProcessStack::SetAutoExposureSettings,
		"SetFXAASettings", &CPostProcessStack::SetFXAASettings
	);
}
