/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include <cmath>

#include "OvRendering/Entities/Camera.h"
#include "OvMaths/FMatrix4.h"

OvRendering::Entities::Camera::Camera(OvTools::Utils::OptRef<OvMaths::FTransform> p_transform) :
	Entity{ p_transform },
	m_projectionMode(Settings::EProjectionMode::PERSPECTIVE),
	m_fov(60.0f),
	m_size(5.0f),
	m_near(0.1f),
	m_far(1000.f),
	m_clearColor(0.f, 0.f, 0.f),
	m_clearColorBuffer(true),
	m_clearDepthBuffer(true),
	m_clearStencilBuffer(true),
	m_frustumGeometryCulling(true),
	m_frustumLightCulling(true),
	m_frustum{}
{
}

void OvRendering::Entities::Camera::CacheMatrices(uint16_t p_windowWidth, uint16_t p_windowHeight)
{
	CacheProjectionMatrix(p_windowWidth, p_windowHeight);
	CacheViewMatrix();
	CacheFrustum(m_viewMatrix, m_projectionMatrix);
}

void OvRendering::Entities::Camera::CacheProjectionMatrix(uint16_t p_windowWidth, uint16_t p_windowHeight)
{
	m_projectionMatrix = CalculateProjectionMatrix(p_windowWidth, p_windowHeight);
}

void OvRendering::Entities::Camera::CacheViewMatrix()
{
	m_viewMatrix = CalculateViewMatrix();
}

void OvRendering::Entities::Camera::CacheFrustum(const OvMaths::FMatrix4& p_view, const OvMaths::FMatrix4& p_projection)
{
	m_frustum.CalculateFrustum(p_projection * p_view);
}

const OvMaths::FVector3& OvRendering::Entities::Camera::GetPosition() const
{
	return transform->GetWorldPosition();
}

const OvMaths::FQuaternion& OvRendering::Entities::Camera::GetRotation() const
{
	return transform->GetWorldRotation();
}

float OvRendering::Entities::Camera::GetFov() const
{
	return m_fov;
}

float OvRendering::Entities::Camera::GetSize() const
{
    return m_size;
}

float OvRendering::Entities::Camera::GetNear() const
{
	return m_near;
}

float OvRendering::Entities::Camera::GetFar() const
{
	return m_far;
}

const OvMaths::FVector3 & OvRendering::Entities::Camera::GetClearColor() const
{
	return m_clearColor;
}

bool OvRendering::Entities::Camera::GetClearColorBuffer() const
{
	return m_clearColorBuffer;
}

bool OvRendering::Entities::Camera::GetClearDepthBuffer() const
{
	return m_clearDepthBuffer;
}

bool OvRendering::Entities::Camera::GetClearStencilBuffer() const
{
	return m_clearStencilBuffer;
}

const OvMaths::FMatrix4& OvRendering::Entities::Camera::GetProjectionMatrix() const
{
	return m_projectionMatrix;
}

const OvMaths::FMatrix4& OvRendering::Entities::Camera::GetViewMatrix() const
{
	return m_viewMatrix;
}

const OvRendering::Data::Frustum& OvRendering::Entities::Camera::GetFrustum() const
{
	return m_frustum;
}

OvTools::Utils::OptRef<const OvRendering::Data::Frustum> OvRendering::Entities::Camera::GetGeometryFrustum() const
{
	if (m_frustumGeometryCulling)
	{
		return m_frustum;
	}

	return std::nullopt;
}

OvTools::Utils::OptRef<const OvRendering::Data::Frustum> OvRendering::Entities::Camera::GetLightFrustum() const
{
	if (m_frustumLightCulling)
	{
		return m_frustum;
	}

	return std::nullopt;
}

bool OvRendering::Entities::Camera::HasFrustumGeometryCulling() const
{
	return m_frustumGeometryCulling;
}

bool OvRendering::Entities::Camera::HasFrustumLightCulling() const
{
	return m_frustumLightCulling;
}

OvRendering::Settings::EProjectionMode OvRendering::Entities::Camera::GetProjectionMode() const
{
    return m_projectionMode;
}

void OvRendering::Entities::Camera::SetPosition(const OvMaths::FVector3& p_position)
{
	transform->SetWorldPosition(p_position);
}

void OvRendering::Entities::Camera::SetRotation(const OvMaths::FQuaternion& p_rotation)
{
	transform->SetWorldRotation(p_rotation);
}

void OvRendering::Entities::Camera::SetFov(float p_value)
{
	m_fov = p_value;
}

void OvRendering::Entities::Camera::SetSize(float p_value)
{
    m_size = p_value;
}

void OvRendering::Entities::Camera::SetNear(float p_value)
{
	m_near = p_value;
}

void OvRendering::Entities::Camera::SetFar(float p_value)
{
	m_far = p_value;
}

void OvRendering::Entities::Camera::SetClearColor(const OvMaths::FVector3 & p_clearColor)
{
	m_clearColor = p_clearColor;
}

void OvRendering::Entities::Camera::SetClearColorBuffer(bool p_value)
{
	m_clearColorBuffer = p_value;
}

void OvRendering::Entities::Camera::SetClearDepthBuffer(bool p_value)
{
	m_clearDepthBuffer = p_value;
}

void OvRendering::Entities::Camera::SetClearStencilBuffer(bool p_value)
{
	m_clearStencilBuffer = p_value;
}

void OvRendering::Entities::Camera::SetFrustumGeometryCulling(bool p_enable)
{
	m_frustumGeometryCulling = p_enable;
}

void OvRendering::Entities::Camera::SetFrustumLightCulling(bool p_enable)
{
	m_frustumLightCulling = p_enable;
}

void OvRendering::Entities::Camera::SetProjectionMode(OvRendering::Settings::EProjectionMode p_projectionMode)
{
    m_projectionMode = p_projectionMode;
}

OvMaths::FMatrix4 OvRendering::Entities::Camera::CalculateProjectionMatrix(uint16_t p_windowWidth, uint16_t p_windowHeight) const
{
    using namespace OvMaths;
    using namespace OvRendering::Settings;

    const auto ratio = p_windowWidth / static_cast<float>(p_windowHeight);

    switch (m_projectionMode)
    {
    case EProjectionMode::ORTHOGRAPHIC:
        return FMatrix4::CreateOrthographic(m_size, ratio, m_near, m_far);

    case EProjectionMode::PERSPECTIVE: 
        return FMatrix4::CreatePerspective(m_fov, ratio, m_near, m_far);

    default:
        return FMatrix4::Identity;
    }
}

OvMaths::FMatrix4 OvRendering::Entities::Camera::CalculateViewMatrix() const
{
	const OvMaths::FVector3& position = transform->GetWorldPosition();
	const OvMaths::FQuaternion& rotation = transform->GetWorldRotation();
	const OvMaths::FVector3& up = transform->GetWorldUp();
	const OvMaths::FVector3& forward = transform->GetWorldForward();

	return OvMaths::FMatrix4::CreateView(
		position.x, position.y, position.z,
		position.x + forward.x, position.y + forward.y, position.z + forward.z,
		up.x, up.y, up.z
	);
}
