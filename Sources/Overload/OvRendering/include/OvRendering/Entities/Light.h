/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include <OvMaths/FVector3.h>
#include <OvMaths/FMatrix4.h>
#include <OvMaths/FTransform.h>

#include "OvRendering/Entities/Entity.h"
#include "OvRendering/Settings/ELightType.h"
#include "OvRendering/Resources/Texture.h"
#include "OvRendering/HAL/Framebuffer.h"
#include <OvRendering/Entities/Camera.h>

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

		/**
		* Update the content of the shadow cache
		* @param p_shadowMapResolution
		* @param p_camera
		*/
		void UpdateShadowData(const OvRendering::Entities::Camera& p_camera);

		/**
		* Returns the light space matrix
		*/
		const OvMaths::FMatrix4& GetLightSpaceMatrix() const;

		/**
		* Returns the framebuffer used to render the shadow map
		*/
		const OvRendering::HAL::Framebuffer& GetShadowBuffer() const;

		/**
		* Generate the light matrix, ready to send to the GPU
		*/
		OvMaths::FMatrix4 GenerateMatrix() const;

		/**
		* Calculate the light effect range from the quadratic falloff equation
		*/
		float GetEffectRange() const;

		OvMaths::FMatrix4 lightSpaceMatrix;
		std::unique_ptr<OvRendering::HAL::Framebuffer> shadowBuffer;
	};
}
