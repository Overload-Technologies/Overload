/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include <OvDebug/Assertion.h>
#include <OvRendering/Entities/Light.h>
#include <OvRendering/HAL/Renderbuffer.h>

namespace
{
	void SetupFramebufferForShadowMapping(
		OvRendering::HAL::Framebuffer& p_framebuffer,
		uint32_t p_resolution
	)
	{
		using namespace OvRendering::HAL;
		using namespace OvRendering::Settings;

		const auto renderTexture = std::make_shared<Texture>();

		TextureDesc renderTextureDesc{
			.width = p_resolution,
			.height = p_resolution,
			.minFilter = ETextureFilteringMode::LINEAR,
			.magFilter = ETextureFilteringMode::LINEAR,
			.horizontalWrap = ETextureWrapMode::CLAMP_TO_BORDER,
			.verticalWrap = ETextureWrapMode::CLAMP_TO_BORDER,
			.internalFormat = EInternalFormat::DEPTH_COMPONENT,
			.useMipMaps = false,
			.mutableDesc = MutableTextureDesc{
				.format = EFormat::DEPTH_COMPONENT,
				.type = EPixelDataType::FLOAT
			}
		};

		renderTexture->Allocate(renderTextureDesc);
		renderTexture->SetBorderColor(OvMaths::FVector4::One);
		p_framebuffer.Attach<Texture>(renderTexture, EFramebufferAttachment::DEPTH);
		p_framebuffer.Validate();
		p_framebuffer.SetTargetDrawBuffer(std::nullopt);
		p_framebuffer.SetTargetReadBuffer(std::nullopt);
	}

	uint32_t Pack(uint8_t c0, uint8_t c1, uint8_t c2, uint8_t c3)
	{
		return (c0 << 24) | (c1 << 16) | (c2 << 8) | c3;
	}

	uint32_t Pack(const OvMaths::FVector3& p_toPack)
	{
		return Pack(static_cast<uint8_t>(p_toPack.x * 255.f), static_cast<uint8_t>(p_toPack.y * 255.f), static_cast<uint8_t>(p_toPack.z * 255.f), 0);
	}
}

void OvRendering::Entities::Light::UpdateShadowData(const OvRendering::Entities::Camera& p_camera)
{
	if (type == OvRendering::Settings::ELightType::DIRECTIONAL)
	{
		if (!shadowBuffer)
		{
			shadowBuffer = std::make_unique<OvRendering::HAL::Framebuffer>();
			SetupFramebufferForShadowMapping(*shadowBuffer, static_cast<uint32_t>(shadowMapResolution));
		}
		else
		{
			shadowBuffer->Resize(shadowMapResolution, shadowMapResolution);
		}

		const auto lightPosition =
			shadowFollowCamera ?
			p_camera.transform->GetWorldPosition() + OvMaths::FVector3::Up * shadowAreaSize :  // offsets the camera position be positioned on the top plane of the effect area
			transform->GetWorldPosition();

		OvRendering::Entities::Camera lightCamera;
		lightCamera.SetNear(0.1f);
		lightCamera.SetFar(shadowAreaSize * 2.0f); // x2 because the light will be positioned on the top plane of the effect area (above the camera)
		lightCamera.SetSize(shadowAreaSize);
		lightCamera.SetProjectionMode(OvRendering::Settings::EProjectionMode::ORTHOGRAPHIC);
		lightCamera.SetPosition(lightPosition);
		lightCamera.SetRotation(transform->GetWorldRotation()); // keep the forward from the directional light
		lightCamera.CacheMatrices(shadowMapResolution, shadowMapResolution);
		lightSpaceMatrix = lightCamera.GetProjectionMatrix() * lightCamera.GetViewMatrix();
	}
	else
	{
		shadowBuffer.reset();
	}
}

const OvMaths::FMatrix4& OvRendering::Entities::Light::GetLightSpaceMatrix() const
{
	return lightSpaceMatrix;
}

const OvRendering::HAL::Framebuffer& OvRendering::Entities::Light::GetShadowBuffer() const
{
	OVASSERT(shadowBuffer != nullptr, "Cannot retrieve the shadow map because this light has no framebuffer!");
	return *shadowBuffer;
}

OvMaths::FMatrix4 OvRendering::Entities::Light::GenerateMatrix() const
{
	OvMaths::FMatrix4 result;

	auto position = transform.Get().GetWorldPosition();
	result.data[0] = position.x;
	result.data[1] = position.y;
	result.data[2] = position.z;

	auto forward = transform.Get().GetWorldForward();
	result.data[4] = forward.x;
	result.data[5] = forward.y;
	result.data[6] = forward.z;

	result.data[8] = static_cast<float>(Pack(color));

	result.data[12] = static_cast<float>(type);
	result.data[13] = cutoff;
	result.data[14] = outerCutoff;

	result.data[3] = constant;
	result.data[7] = linear;
	result.data[9] = castShadows ? 1.0f : 0.0f;
	result.data[11] = quadratic;
	result.data[15] = intensity;

	return result;
}

float CalculateLuminosity(float p_constant, float p_linear, float p_quadratic, float p_intensity, float p_distance)
{
	auto attenuation = (p_constant + p_linear * p_distance + p_quadratic * (p_distance * p_distance));
	return (1.0f / attenuation) * std::abs(p_intensity);
}

float CalculatePointLightRadius(float p_constant, float p_linear, float p_quadratic, float p_intensity)
{
	constexpr float threshold = 1 / 255.0f;
	constexpr float step = 1.0f;

	float distance = 0.0f;

	#define TRY_GREATER(value)\
	else if (CalculateLuminosity(p_constant, p_linear, p_quadratic, p_intensity, value) > threshold)\
	{\
		distance = value;\
	}

	#define TRY_LESS(value, newValue)\
	else if (CalculateLuminosity(p_constant, p_linear, p_quadratic, p_intensity, value) < threshold)\
	{\
		distance = newValue;\
	}

	// Prevents infinite while true. If a light has a bigger radius than 10000 we ignore it and make it infinite
	if (CalculateLuminosity(p_constant, p_linear, p_quadratic, p_intensity, 1000.0f) > threshold)
	{
		return std::numeric_limits<float>::infinity();
	}
	TRY_LESS(20.0f, 0.0f)
	TRY_GREATER(750.0f)
	TRY_LESS(50.0f, 20.0f + step)
	TRY_LESS(100.0f, 50.0f + step)
	TRY_GREATER(500.0f)
	TRY_GREATER(250.0f)

	while (true)
	{
		if (CalculateLuminosity(p_constant, p_linear, p_quadratic, p_intensity, distance) < threshold) // If the light has a very low luminosity for the given distance, we consider the current distance as the light radius
		{
			return distance;
		}
		else
		{
			distance += step;
		}
	}
}

float CalculateAmbientBoxLightRadius(const OvMaths::FVector3& p_position, const OvMaths::FVector3& p_size)
{
	return OvMaths::FVector3::Distance(p_position, p_position + p_size);
}

float OvRendering::Entities::Light::GetEffectRange() const
{
	switch (type)
	{
	case Settings::ELightType::POINT:
	case Settings::ELightType::SPOT: return CalculatePointLightRadius(constant, linear, quadratic, intensity);
	case Settings::ELightType::AMBIENT_BOX: return CalculateAmbientBoxLightRadius(transform.Get().GetWorldPosition(), {constant, linear, quadratic});
	case Settings::ELightType::AMBIENT_SPHERE: return constant;
	}

	return std::numeric_limits<float>::infinity();
}
