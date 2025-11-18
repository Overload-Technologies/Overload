/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include "OvCore/Rendering/PostProcess/PostProcessStack.h"
#include "OvCore/Rendering/PostProcess/BloomEffect.h"
#include "OvCore/Rendering/PostProcess/FXAAEffect.h"
#include "OvCore/Rendering/PostProcess/TonemappingEffect.h"
#include "OvCore/Rendering/PostProcess/AutoExposureEffect.h"

OvCore::Rendering::PostProcess::PostProcessStack::PostProcessStack()
{
	Set<BloomEffect>(BloomSettings());
	Set<AutoExposureEffect>(AutoExposureSettings());
	Set<FXAAEffect>(FXAASettings());
	Set<TonemappingEffect>(TonemappingSettings());
}

OvCore::Rendering::PostProcess::EffectSettings& OvCore::Rendering::PostProcess::PostProcessStack::Get(const std::type_index& type)
{
	auto it = m_settings.find(type);
	OVASSERT(it != m_settings.end(), "Settings not found for the given Effect type");
	return *it->second;
}

const OvCore::Rendering::PostProcess::EffectSettings& OvCore::Rendering::PostProcess::PostProcessStack::Get(const std::type_index& type) const
{
	auto it = m_settings.find(type);
	OVASSERT(it != m_settings.end(), "Settings not found for the given Effect type");
	return *it->second;
}