/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include <tracy/Tracy.hpp>

#include <OvCore/Rendering/EngineBufferRenderFeature.h>
#include <OvCore/Rendering/EngineDrawableDescriptor.h>
#include <OvRendering/Core/CompositeRenderer.h>

namespace
{
	constexpr size_t kUBOSize =
		sizeof(OvMaths::FMatrix4) +	// Model matrix
		sizeof(OvMaths::FMatrix4) +	// View matrix
		sizeof(OvMaths::FMatrix4) +	// Projection matrix
		sizeof(OvMaths::FVector3) +	// Camera position
		sizeof(float) +				// Elapsed time
		sizeof(OvMaths::FMatrix4);	// User matrix
}

OvCore::Rendering::EngineBufferRenderFeature::EngineBufferRenderFeature(
	OvRendering::Core::CompositeRenderer& p_renderer,
	OvRendering::Features::EFeatureExecutionPolicy p_executionPolicy
) : 
	ARenderFeature(p_renderer, p_executionPolicy)
{
	m_engineBuffer = std::make_unique<baregl::Buffer>();
	m_engineBuffer->Allocate(kUBOSize, baregl::types::EAccessSpecifier::STREAM_DRAW);
	m_startTime = std::chrono::high_resolution_clock::now();
}

void OvCore::Rendering::EngineBufferRenderFeature::SetCamera(const OvRendering::Entities::Camera& p_camera)
{
	m_frameCameraData = {
		.viewMatrix = OvMaths::FMatrix4::Transpose(p_camera.GetViewMatrix()),
		.projectionMatrix = OvMaths::FMatrix4::Transpose(p_camera.GetProjectionMatrix()),
		.position = p_camera.GetPosition()
	};

	UploadCameraData(m_frameCameraData);
	m_cameraOverrideActive = false;
}

void OvCore::Rendering::EngineBufferRenderFeature::OnBeginFrame(const OvRendering::Data::FrameDescriptor& p_frameDescriptor)
{
	OVASSERT(p_frameDescriptor.camera.has_value(), "Camera is not set in the frame descriptor");

	auto currentTime = std::chrono::high_resolution_clock::now();
	auto elapsedTime = std::chrono::duration_cast<std::chrono::duration<float>>(currentTime - m_startTime);

	m_frameCameraData = {
		.viewMatrix = OvMaths::FMatrix4::Transpose(p_frameDescriptor.camera->GetViewMatrix()),
		.projectionMatrix = OvMaths::FMatrix4::Transpose(p_frameDescriptor.camera->GetProjectionMatrix()),
		.position = p_frameDescriptor.camera->GetPosition()
	};

	struct FrameCameraData
	{
		OvMaths::FMatrix4 viewMatrix;
		OvMaths::FMatrix4 projectionMatrix;
		OvMaths::FVector3 position;
		float elapsedTime;
	} uboDataPage{
		.viewMatrix = m_frameCameraData.viewMatrix,
		.projectionMatrix = m_frameCameraData.projectionMatrix,
		.position = m_frameCameraData.position,
		.elapsedTime = elapsedTime.count()
	};
	static_assert(
		sizeof(FrameCameraData) == sizeof(CameraData) + sizeof(float),
		"FrameCameraData must match the engine UBO frame page"
	);

	m_engineBuffer->Upload(&uboDataPage, baregl::data::BufferMemoryRange{
		.offset = sizeof(OvMaths::FMatrix4), // Skip uploading the first matrix (Model matrix)
		.size = sizeof(uboDataPage)
	});

	m_cameraOverrideActive = false;
	m_engineBuffer->Bind(baregl::types::EBufferType::UNIFORM, 0);
}

void OvCore::Rendering::EngineBufferRenderFeature::OnEndFrame()
{
	m_cameraOverrideActive = false;
	m_engineBuffer->Unbind();
}

void OvCore::Rendering::EngineBufferRenderFeature::OnBeforeDraw(OvRendering::Data::PipelineState& p_pso, const OvRendering::Entities::Drawable& p_drawable)
{
	ZoneScoped;

	OvTools::Utils::OptRef<const EngineDrawableDescriptor> descriptor;

	if (!p_drawable.TryGetDescriptor<EngineDrawableDescriptor>(descriptor))
	{
		RestoreFrameCamera();
		return;
	}

	const auto modelMatrix = OvMaths::FMatrix4::Transpose(descriptor->modelMatrix);

	// Upload model matrix (First matrix in the UBO)
	m_engineBuffer->Upload(&modelMatrix, baregl::data::BufferMemoryRange{
		.offset = 0,
		.size = sizeof(modelMatrix)
	});

	// Upload user matrix (Last matrix in the UBO)
	m_engineBuffer->Upload(&descriptor->userMatrix, baregl::data::BufferMemoryRange{
		.offset = kUBOSize - sizeof(modelMatrix),
		.size = sizeof(modelMatrix)
	});

	if (descriptor->viewMatrixOverride || descriptor->projectionMatrixOverride)
	{
		UploadCameraData({
			.viewMatrix = descriptor->viewMatrixOverride ?
				OvMaths::FMatrix4::Transpose(*descriptor->viewMatrixOverride) :
				m_frameCameraData.viewMatrix,
			.projectionMatrix = descriptor->projectionMatrixOverride ?
				OvMaths::FMatrix4::Transpose(*descriptor->projectionMatrixOverride) :
				m_frameCameraData.projectionMatrix,
			.position = descriptor->viewMatrixOverride ?
				OvMaths::FVector3::Zero :
				m_frameCameraData.position
		});
		m_cameraOverrideActive = true;
	}
	else
	{
		RestoreFrameCamera();
	}
}

void OvCore::Rendering::EngineBufferRenderFeature::UploadCameraData(const CameraData& p_cameraData)
{
	m_engineBuffer->Upload(&p_cameraData, baregl::data::BufferMemoryRange{
		.offset = sizeof(OvMaths::FMatrix4), // Skip uploading the model matrix.
		.size = sizeof(p_cameraData)
	});
}

void OvCore::Rendering::EngineBufferRenderFeature::RestoreFrameCamera()
{
	if (!m_cameraOverrideActive)
	{
		return;
	}

	UploadCameraData(m_frameCameraData);
	m_cameraOverrideActive = false;
}
