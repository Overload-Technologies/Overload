/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include <OvMaths/FVector3.h>
#include <OvMaths/FMatrix4.h>
#include <OvMaths/FTransform.h>

#include <OvRendering/Data/FrameDescriptor.h>
#include <OvRendering/Entities/Camera.h>
#include <OvRendering/Entities/Entity.h>
#include <OvRendering/HAL/Framebuffer.h>
#include <OvRendering/Resources/Texture.h>
#include <OvRendering/Settings/ELightType.h>

namespace OvRendering::Entities
{
	/**
	* Data structure that can represent any type of light
	*/
	struct Light : public Entity
	{
		OvMaths::FVector3 color{ 1.f, 1.f, 1.f };
		float intensity = 1.f;
		float constant = 0.0f;
		float linear = 0.0f;
		float quadratic = 1.0f;
		float cutoff = 12.f;
		float outerCutoff = 15.f;
		Settings::ELightType type = Settings::ELightType::POINT;

		bool castShadows = false;
		float shadowAreaSize = 50.0f;
		bool shadowFollowCamera = true;
		int16_t shadowMapResolution = 8192;

		std::unique_ptr<OvRendering::HAL::Framebuffer> shadowBuffer;
		std::optional<OvRendering::Entities::Camera> shadowCamera;
		std::optional<OvMaths::FMatrix4> lightSpaceMatrix;

		/**
		* Generate and cache light space matrix for the light
		*/
		void PrepareForShadowRendering(const OvRendering::Data::FrameDescriptor& p_frameDescriptor);

		/**
		* Returns true if the light is setup for shadow rendering
		*/
		bool IsSetupForShadowRendering() const;

		/**
		* Generate the light matrix, ready to send to the GPU
		*/
		OvMaths::FMatrix4 GenerateMatrix() const;

		/**
		* Calculate the light effect range from the quadratic falloff equation
		*/
		float CalculateEffectRange() const;
	};
}
