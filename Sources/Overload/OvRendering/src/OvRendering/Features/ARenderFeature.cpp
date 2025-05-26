/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include "OvRendering/Features/ARenderFeature.h"
#include "OvRendering/Core/ABaseRenderer.h"
#include "OvRendering/Core/CompositeRenderer.h"

OvRendering::Features::ARenderFeature::ARenderFeature(
	Core::CompositeRenderer& p_renderer,
	EFeatureExecutionPolicy p_executionPolicy
) :
	m_renderer(p_renderer),
	m_executionPolicy(p_executionPolicy)
{
}

bool OvRendering::Features::ARenderFeature::IsEnabled() const
{
	return m_executionPolicy != EFeatureExecutionPolicy::NEVER;
}

bool OvRendering::Features::ARenderFeature::IsEnabledFor(std::type_index p_type) const
{
	return (m_executionPolicy == EFeatureExecutionPolicy::ALWAYS) ||
		(m_executionPolicy == EFeatureExecutionPolicy::WHITELIST_ONLY && m_whitelist.contains(p_type)) ||
		(m_executionPolicy == EFeatureExecutionPolicy::DEFAULT && !m_blacklist.contains(p_type));
}

OvRendering::Features::ARenderFeature& OvRendering::Features::ARenderFeature::Except(std::type_index p_type)
{
	OVASSERT(!m_renderer.IsDrawing(), "Cannot add a render pass to the blacklist while rendering is in progress.");
	m_blacklist.insert(p_type);
	return *this;
}

OvRendering::Features::ARenderFeature& OvRendering::Features::ARenderFeature::Include(std::type_index p_type)
{
	OVASSERT(!m_renderer.IsDrawing(), "Cannot add a render pass to the whitelist while rendering is in progress.");
	m_whitelist.insert(p_type);
	return *this;
}

void OvRendering::Features::ARenderFeature::SetExecutionPolicy(EFeatureExecutionPolicy p_policy)
{
	OVASSERT(!m_renderer.IsDrawing(), "Cannot set the execution policy while rendering is in progress.");
	m_executionPolicy = p_policy;
}

OvRendering::Features::EFeatureExecutionPolicy OvRendering::Features::ARenderFeature::GetExecutionPolicy() const
{
	return m_executionPolicy;
}

void OvRendering::Features::ARenderFeature::OnBeginFrame(const Data::FrameDescriptor& p_frameDescriptor)
{
}

void OvRendering::Features::ARenderFeature::OnEndFrame()
{
}

void OvRendering::Features::ARenderFeature::OnBeforeDraw(Data::PipelineState& p_pso, const Entities::Drawable& p_drawable)
{
}

void OvRendering::Features::ARenderFeature::OnAfterDraw(const Entities::Drawable& p_drawable)
{
}
