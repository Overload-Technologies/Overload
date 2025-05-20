/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include "OvRendering/Features/ARenderFeature.h"
#include "OvRendering/Core/ABaseRenderer.h"
#include "OvRendering/Core/CompositeRenderer.h"

OvRendering::Features::ARenderFeature::ARenderFeature(Core::CompositeRenderer& p_renderer)
	: m_renderer(p_renderer)
{
}

void OvRendering::Features::ARenderFeature::SetEnabled(bool p_enabled)
{
	OVASSERT(!m_renderer.IsDrawing(), "Cannot toggle a render feature while rendering is in progress.");
	m_enabled = p_enabled;
}

bool OvRendering::Features::ARenderFeature::IsEnabled() const
{
	return m_enabled;
}

void OvRendering::Features::ARenderFeature::OnBeginFrame(const Data::FrameDescriptor& p_frameDescriptor)
{
}

void OvRendering::Features::ARenderFeature::OnEndFrame()
{
}

void OvRendering::Features::ARenderFeature::OnBeforeDraw(Data::PipelineState& p_pso, Entities::Drawable& p_drawable)
{
}

void OvRendering::Features::ARenderFeature::OnAfterDraw(const Entities::Drawable& p_drawable)
{
}
