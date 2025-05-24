/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include <format>

#include <OvCore/ECS/Actor.h>
#include <OvCore/ECS/Components/CReflectionProbe.h>

namespace
{
	constexpr uint32_t kProbeResolution = 512;
}

OvCore::ECS::Components::CReflectionProbe::CReflectionProbe(ECS::Actor& p_owner) : AComponent(p_owner)
{
	m_cubemap = std::make_unique<OvRendering::HAL::Texture>(
		OvRendering::Settings::ETextureType::TEXTURE_CUBE,
		"ReflectionProbe" // TODO: Find better name?
	);

	m_cubemap->Allocate(
		OvRendering::Settings::TextureDesc{
			.width = kProbeResolution,
			.height = kProbeResolution,
			.minFilter = OvRendering::Settings::ETextureFilteringMode::LINEAR,
			.magFilter = OvRendering::Settings::ETextureFilteringMode::LINEAR,
			.horizontalWrap = OvRendering::Settings::ETextureWrapMode::CLAMP_TO_EDGE,
			.verticalWrap = OvRendering::Settings::ETextureWrapMode::CLAMP_TO_EDGE,
			.internalFormat = OvRendering::Settings::EInternalFormat::RGBA32F,
			.useMipMaps = false
		}
	);
}

std::string OvCore::ECS::Components::CReflectionProbe::GetName()
{
	return "Reflection Probe";
}

void OvCore::ECS::Components::CReflectionProbe::SetSize(const OvMaths::FVector3& p_size)
{
	m_size = p_size;
}

const OvMaths::FVector3& OvCore::ECS::Components::CReflectionProbe::GetSize() const
{
	return m_size;
}

void OvCore::ECS::Components::CReflectionProbe::OnSerialize(tinyxml2::XMLDocument& p_doc, tinyxml2::XMLNode* p_node)
{
	using namespace OvCore::Helpers;
}

void OvCore::ECS::Components::CReflectionProbe::OnDeserialize(tinyxml2::XMLDocument& p_doc, tinyxml2::XMLNode* p_node)
{
	using namespace OvCore::Helpers;
}

void OvCore::ECS::Components::CReflectionProbe::OnInspector(OvUI::Internal::WidgetContainer& p_root)
{
	using namespace OvCore::Helpers;
}
