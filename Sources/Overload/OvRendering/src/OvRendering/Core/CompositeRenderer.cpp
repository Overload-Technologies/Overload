/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include <functional>

#include <OvRendering/Core/CompositeRenderer.h>
#include <OvRendering/Profiling/GPUProfiling.h>

#include <OvTools/Profiling/CPUProfiling.h>

OvRendering::Core::CompositeRenderer::CompositeRenderer(Context::Driver& p_driver)
	: ABaseRenderer(p_driver)
{
}

void OvRendering::Core::CompositeRenderer::BeginFrame(const Data::FrameDescriptor& p_frameDescriptor)
{
	CPUZone;
	GPUZone("BeginFrame");

	ABaseRenderer::BeginFrame(p_frameDescriptor);

	for (const auto& [_, feature] : m_features)
	{
		if (feature->IsEnabled())
		{
			feature->OnBeginFrame(p_frameDescriptor);
		}
	}

	for (const auto& [_, pass] : m_passes)
	{
		if (pass.second->IsEnabled())
		{
			pass.second->OnBeginFrame(p_frameDescriptor);
		}
	}
}

void OvRendering::Core::CompositeRenderer::DrawFrame()
{
	CPUZone;
	GPUZone("DrawFrame");

	auto pso = CreatePipelineState();

	for (const auto& [_, pass] : m_passes)
	{
		m_frameDescriptor.outputBuffer.value().Bind();
		SetViewport(0, 0, m_frameDescriptor.renderWidth, m_frameDescriptor.renderHeight);

		if (pass.second->IsEnabled())
		{
			pass.second->Draw(pso);
		}
	}
}

void OvRendering::Core::CompositeRenderer::EndFrame()
{
	CPUZone;
	GPUZone("EndFrame");

	for (const auto& [_, pass] : m_passes)
	{
		if (pass.second->IsEnabled())
		{
			pass.second->OnEndFrame();
		}
	}

	for (const auto& [_, feature] : m_features)
	{
		if (feature->IsEnabled())
		{
			feature->OnEndFrame();
		}
	}

	ClearDescriptors();
	ABaseRenderer::EndFrame();
}

void OvRendering::Core::CompositeRenderer::DrawEntity(
	OvRendering::Data::PipelineState p_pso,
	const Entities::Drawable& p_drawable
)
{
	CPUZone;

	for (const auto& [_, feature] : m_features)
	{
		if (feature->IsEnabled())
		{
			feature->OnBeforeDraw(p_pso, p_drawable);
		}
	}

	ABaseRenderer::DrawEntity(p_pso, p_drawable);
	
	for (const auto& [_, feature] : m_features)
	{
		if (feature->IsEnabled())
		{
			feature->OnAfterDraw(p_drawable);
		}
	}
}
