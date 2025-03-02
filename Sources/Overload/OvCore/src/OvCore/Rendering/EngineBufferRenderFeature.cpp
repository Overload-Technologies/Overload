/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include <OvRendering/Core/CompositeRenderer.h>

#include "OvCore/Rendering/EngineBufferRenderFeature.h"
#include "OvCore/Rendering/EngineDrawableDescriptor.h"

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

OvCore::Rendering::EngineBufferRenderFeature::EngineBufferRenderFeature(OvRendering::Core::CompositeRenderer& p_renderer)
	: ARenderFeature(p_renderer)
{
	m_engineBuffer = std::make_unique<OvRendering::HAL::UniformBuffer>();

	m_engineBuffer->Allocate(kUBOSize, OvRendering::Settings::EAccessSpecifier::STREAM_DRAW);

	m_startTime = std::chrono::high_resolution_clock::now();
}

void OvCore::Rendering::EngineBufferRenderFeature::OnBeginFrame(const OvRendering::Data::FrameDescriptor& p_frameDescriptor)
{
	m_cachedFrameDescriptor = p_frameDescriptor;

	auto currentTime = std::chrono::high_resolution_clock::now();
	auto elapsedTime = std::chrono::duration_cast<std::chrono::duration<float>>(currentTime - m_startTime);

	struct
	{
		OvMaths::FMatrix4 viewMatrix;
		OvMaths::FMatrix4 projectionMatrix;
		OvMaths::FVector3 cameraPosition;
		float elapsedTime;
	} uboDataPage {
		.viewMatrix = OvMaths::FMatrix4::Transpose(p_frameDescriptor.camera->GetViewMatrix()),
		.projectionMatrix = OvMaths::FMatrix4::Transpose(p_frameDescriptor.camera->GetProjectionMatrix()),
		.cameraPosition = p_frameDescriptor.camera->GetPosition(),
		.elapsedTime = elapsedTime.count()
	};

	m_engineBuffer->Upload(&uboDataPage, OvRendering::HAL::BufferMemoryRange{
		.offset = sizeof(OvMaths::FMatrix4), // Skip uploading the first matrix (Model matrix)
		.size = sizeof(uboDataPage)
	});

	m_engineBuffer->Bind(0);
}

void OvCore::Rendering::EngineBufferRenderFeature::OnEndFrame()
{
	m_engineBuffer->Unbind();
}

void OvCore::Rendering::EngineBufferRenderFeature::OnBeforeDraw(OvRendering::Data::PipelineState& p_pso, const OvRendering::Entities::Drawable& p_drawable)
{
	OvTools::Utils::OptRef<const EngineDrawableDescriptor> descriptor;

	if (p_drawable.TryGetDescriptor<EngineDrawableDescriptor>(descriptor))
	{
		const auto modelMatrix = OvMaths::FMatrix4::Transpose(descriptor->modelMatrix);

		// Upload model matrix (First matrix in the UBO)
		m_engineBuffer->Upload(&modelMatrix, OvRendering::HAL::BufferMemoryRange{
			.offset = 0,
			.size = sizeof(modelMatrix)
		});

		// Upload user matrix (Last matrix in the UBO)
		m_engineBuffer->Upload(&descriptor->userMatrix, OvRendering::HAL::BufferMemoryRange{
			.offset = kUBOSize - sizeof(modelMatrix),
			.size = sizeof(modelMatrix)
		});
	}
}
