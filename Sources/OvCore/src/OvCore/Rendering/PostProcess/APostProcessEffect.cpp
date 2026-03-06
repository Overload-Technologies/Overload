/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include "OvCore/Rendering/PostProcess/AEffect.h"

OvCore::Rendering::PostProcess::AEffect::AEffect(OvRendering::Core::CompositeRenderer& p_renderer) :
	m_renderer(p_renderer)
{
}

bool OvCore::Rendering::PostProcess::AEffect::IsApplicable(const EffectSettings& p_settings) const
{
	return p_settings.enabled;
}
