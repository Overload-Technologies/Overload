/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include <functional>
#include <ranges>
#include <tracy/Tracy.hpp>

#include <OvRendering/Core/CompositeRenderer.h>
#include <OvRendering/HAL/Profiling.h>

OvRendering::Core::CompositeRenderer::CompositeRenderer(Context::Driver& p_driver)
	: ABaseRenderer(p_driver)
{
}

void OvRendering::Core::CompositeRenderer::BeginFrame(const Data::FrameDescriptor& p_frameDescriptor)
{
	ZoneScoped;
	TracyGpuZone("BeginFrame");

	ABaseRenderer::BeginFrame(p_frameDescriptor);

	for (const auto& feature : m_features | std::views::values)
	{
		if (feature->IsEnabled())
		{
			feature->OnBeginFrame(p_frameDescriptor);
		}
	}

	for (const auto& pass : m_passes | std::views::values)
	{
		if (pass.second->IsEnabled())
		{
			pass.second->OnBeginFrame(p_frameDescriptor);
		}
	}
}

void OvRendering::Core::CompositeRenderer::DrawFrame()
{
	ZoneScoped;
	TracyGpuZone("DrawFrame");

	auto pso = CreatePipelineState();

	OVASSERT(!m_currentPass.has_value(), "Cannot draw frame while a pass is already being drawn. Did you forget to call EndFrame() ?");

	for (const auto& pass : m_passes | std::views::values)
	{
		m_currentPass = pass.second.get();

		m_frameDescriptor.outputBuffer.value().Bind();
		SetViewport(0, 0, m_frameDescriptor.renderWidth, m_frameDescriptor.renderHeight);

		if (m_currentPass->IsEnabled())
		{
			m_currentPass->Draw(pso);
		}

		m_currentPass.reset();
	}
}

void OvRendering::Core::CompositeRenderer::EndFrame()
{
	ZoneScoped;
	TracyGpuZone("EndFrame");

	for (const auto& pass : m_passes | std::views::values)
	{
		if (pass.second->IsEnabled())
		{
			pass.second->OnEndFrame();
		}
	}

	for (const auto& feature : m_features | std::views::values)
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
	ZoneScoped;

	// Ensure the drawable is valid.
	// If not, skip the draw call and the attached features.
	if (!IsDrawable(p_drawable))
	{
		return;
	}

	const auto& currentPass = m_currentPass.value();
	const auto& passTypeId = typeid(currentPass);

	for (const auto& feature : m_features | std::views::values)
	{
		if (feature->IsEnabledFor(passTypeId))
		{
			feature->OnBeforeDraw(p_pso, p_drawable);
		}
	}

	ABaseRenderer::DrawEntity(p_pso, p_drawable);
	
	for (const auto& feature : m_features | std::views::values)
	{
		if (feature->IsEnabledFor(passTypeId))
		{
			feature->OnAfterDraw(p_drawable);
		}
	}
}
