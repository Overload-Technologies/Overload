/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/


#include "OvRendering/Core/ARenderPass.h"
#include "OvRendering/Core/CompositeRenderer.h"

#include <OvDebug/Assertion.h>

OvRendering::Core::ARenderPass::ARenderPass(Core::CompositeRenderer& p_renderer)
	: m_renderer(p_renderer)
{
}

void OvRendering::Core::ARenderPass::SetEnabled(bool p_enabled)
{
	OVASSERT(!m_renderer.IsDrawing(), "Cannot toggle a render feature while rendering is in progress.");
	m_enabled = p_enabled;
}

bool OvRendering::Core::ARenderPass::IsEnabled() const
{
	return m_enabled;
}

void OvRendering::Core::ARenderPass::OnBeginFrame(const Data::FrameDescriptor& p_frameDescriptor)
{
}

void OvRendering::Core::ARenderPass::OnEndFrame()
{
}
