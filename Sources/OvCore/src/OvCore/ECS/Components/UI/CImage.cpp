/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include <array>
#include <cmath>
#include <cstdint>

#include <tinyxml2.h>

#include <OvCore/ECS/Actor.h>
#include <OvCore/ECS/Components/UI/CImage.h>
#include <OvCore/ECS/Components/UI/UIInspectorUtils.h>
#include <OvCore/Global/ServiceLocator.h>
#include <OvCore/Helpers/GUIDrawer.h>
#include <OvCore/Helpers/Serializer.h>
#include <OvCore/ResourceManagement/MaterialManager.h>
#include <OvCore/ResourceManagement/TextureManager.h>
#include <OvCore/ResourceManagement/UIResourceRegistry.h>
#include <OvRendering/Geometry/Vertex.h>
#include <OvUI/Types/Color.h>

namespace
{
	constexpr const char* kTextureUniform = "u_Image";
	constexpr const char* kTintUniform = "u_Tint";

	OvMaths::FVector2 GetDefaultImageSize()
	{
		return { 100.0f, 100.0f };
	}

	float KeepFinite(float p_value, float p_fallback)
	{
		return std::isfinite(p_value) ? p_value : p_fallback;
	}

	OvUI::Types::Color ToColor(const OvMaths::FVector4& p_value)
	{
		return { p_value.x, p_value.y, p_value.z, p_value.w };
	}

	OvMaths::FVector4 ToVec4(const OvUI::Types::Color& p_value)
	{
		return { p_value.r, p_value.g, p_value.b, p_value.a };
	}
}

OvCore::ECS::Components::UI::CImage::CImage(ECS::Actor& p_owner) :
AComponent(p_owner)
{
	m_textureChangedEvent += [this]
	{
		m_textureReferenceDirty = true;
		m_materialTextureDirty = true;
		SynchronizeTextureState();
	};

	owner.transform.EnableUIData();
	UpdateIntrinsicSize();
	RebuildMesh();
}

std::string OvCore::ECS::Components::UI::CImage::GetName()
{
	return "Image";
}

std::string OvCore::ECS::Components::UI::CImage::GetTypeName()
{
	return std::string{ComponentTraits<CImage>::Name};
}

void OvCore::ECS::Components::UI::CImage::SetTexture(OvRendering::Resources::Texture* p_texture)
{
	SynchronizeTextureState();

	if (m_texture == p_texture)
	{
		return;
	}

	m_texture = p_texture;
	m_textureReferenceDirty = true;
	m_materialTextureDirty = true;
	SynchronizeTextureState();
}

OvRendering::Resources::Texture* OvCore::ECS::Components::UI::CImage::GetTexture() const
{
	SynchronizeTextureState();
	return m_texture;
}

void OvCore::ECS::Components::UI::CImage::SetSize(const OvMaths::FVector2& p_size)
{
	owner.transform.SetUISize(p_size);
}

OvMaths::FVector2 OvCore::ECS::Components::UI::CImage::GetSize() const
{
	const auto& transformSize = owner.transform.GetUISize();
	const auto intrinsicSize = GetIntrinsicSize();

	return {
		transformSize.x > 0.0f ? transformSize.x : intrinsicSize.x,
		transformSize.y > 0.0f ? transformSize.y : intrinsicSize.y
	};
}

OvMaths::FVector2 OvCore::ECS::Components::UI::CImage::GetIntrinsicSize() const
{
	SynchronizeTextureState();
	return m_intrinsicSize;
}

void OvCore::ECS::Components::UI::CImage::SetTint(const OvMaths::FVector4& p_tint)
{
	m_tint.x = KeepFinite(p_tint.x, m_tint.x);
	m_tint.y = KeepFinite(p_tint.y, m_tint.y);
	m_tint.z = KeepFinite(p_tint.z, m_tint.z);
	m_tint.w = KeepFinite(p_tint.w, m_tint.w);
	m_materialTintDirty = true;
}

const OvMaths::FVector4& OvCore::ECS::Components::UI::CImage::GetTint() const
{
	return m_tint;
}

void OvCore::ECS::Components::UI::CImage::SetPreserveAspect(bool p_preserveAspect)
{
	m_preserveAspect = p_preserveAspect;
}

bool OvCore::ECS::Components::UI::CImage::GetPreserveAspect() const
{
	return m_preserveAspect;
}

OvRendering::Resources::Mesh& OvCore::ECS::Components::UI::CImage::GetMesh() const
{
	SynchronizeTextureState();
	return *m_mesh;
}

OvCore::Resources::Material* OvCore::ECS::Components::UI::CImage::GetMaterial()
{
	RefreshMaterial();
	return m_material && m_material->IsValid() ? m_material.get() : nullptr;
}

void OvCore::ECS::Components::UI::CImage::OnSerialize(tinyxml2::XMLDocument& p_doc, tinyxml2::XMLNode* p_node)
{
	SynchronizeTextureState();
	Helpers::Serializer::SerializeTexture(p_doc, p_node, "texture", m_texture);
	Helpers::Serializer::SerializeVec4(p_doc, p_node, "tint", m_tint);
	Helpers::Serializer::SerializeBoolean(p_doc, p_node, "preserve_aspect", m_preserveAspect);
}

void OvCore::ECS::Components::UI::CImage::OnDeserialize(tinyxml2::XMLDocument& p_doc, tinyxml2::XMLNode* p_node)
{
	if (p_node->FirstChildElement("texture"))
	{
		OvRendering::Resources::Texture* texture = m_texture;
		Helpers::Serializer::DeserializeTexture(p_doc, p_node, "texture", texture);
		SetTexture(texture);
	}

	if (p_node->FirstChildElement("tint"))
	{
		auto tint = m_tint;
		Helpers::Serializer::DeserializeVec4(p_doc, p_node, "tint", tint);
		SetTint(tint);
	}

	if (p_node->FirstChildElement("preserve_aspect"))
	{
		auto preserveAspect = m_preserveAspect;
		Helpers::Serializer::DeserializeBoolean(p_doc, p_node, "preserve_aspect", preserveAspect);
		SetPreserveAspect(preserveAspect);
	}
}

void OvCore::ECS::Components::UI::CImage::OnInspector(OvUI::Internal::WidgetContainer& p_root)
{
	UIInspectorUtils::DrawCanvasRequirement(p_root, owner);

	SynchronizeTextureState();
	Helpers::GUIDrawer::DrawTexture(p_root, "Texture", m_texture, &m_textureChangedEvent);
	Helpers::GUIDrawer::DrawBoolean(
		p_root,
		"Preserve Aspect",
		[this]() { return GetPreserveAspect(); },
		[this](bool p_value) { SetPreserveAspect(p_value); }
	);

	Helpers::GUIDrawer::DrawColor(
		p_root,
		"Tint",
		[this]() { return ToColor(m_tint); },
		[this](OvUI::Types::Color p_value) { SetTint(ToVec4(p_value)); },
		true
	);
}

void OvCore::ECS::Components::UI::CImage::RebuildMesh() const
{
	const auto size = m_intrinsicSize;

	const float halfWidth = size.x * 0.5f;
	const float halfHeight = size.y * 0.5f;

	const std::array<OvRendering::Geometry::Vertex, 4> vertices = {
		OvRendering::Geometry::Vertex{{ -halfWidth, -halfHeight, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f, 1.0f }, {}, {}},
		OvRendering::Geometry::Vertex{{  halfWidth, -halfHeight, 0.0f }, { 1.0f, 0.0f }, { 0.0f, 0.0f, 1.0f }, {}, {}},
		OvRendering::Geometry::Vertex{{  halfWidth,  halfHeight, 0.0f }, { 1.0f, 1.0f }, { 0.0f, 0.0f, 1.0f }, {}, {}},
		OvRendering::Geometry::Vertex{{ -halfWidth,  halfHeight, 0.0f }, { 0.0f, 1.0f }, { 0.0f, 0.0f, 1.0f }, {}, {}}
	};

	const std::array<uint32_t, 6> indices = { 0, 1, 2, 0, 2, 3 };

	m_mesh = std::make_unique<OvRendering::Resources::Mesh>(vertices, indices);
}

void OvCore::ECS::Components::UI::CImage::SynchronizeTextureState() const
{
	auto& textureManager = Global::ServiceLocator::Get<ResourceManagement::TextureManager>();
	const auto managerRevision = textureManager.GetResourcesRevision();
	const bool registryChanged = managerRevision != m_textureManagerRevision;
	bool textureStateChanged = m_textureReferenceDirty;

	if (m_texture && (m_textureReferenceDirty || registryChanged) && !textureManager.ContainsResource(m_texture))
	{
		m_texture = nullptr;
		textureStateChanged = true;
	}

	m_textureReferenceDirty = false;
	m_textureManagerRevision = managerRevision;

	const uint64_t textureRevision = m_texture ? m_texture->GetRevision() : 0;
	textureStateChanged |= textureRevision != m_textureRevision;
	m_textureRevision = textureRevision;

	if (textureStateChanged)
	{
		m_materialTextureDirty = true;
		UpdateIntrinsicSize();
		RebuildMesh();
	}
}

void OvCore::ECS::Components::UI::CImage::UpdateIntrinsicSize() const
{
	const auto defaultSize = GetDefaultImageSize();
	m_intrinsicSize = defaultSize;

	if (!m_texture)
	{
		return;
	}

	const auto& textureDesc = m_texture->GetTexture().GetDesc();
	m_intrinsicSize = {
		textureDesc.width > 0 ? static_cast<float>(textureDesc.width) : defaultSize.x,
		textureDesc.height > 0 ? static_cast<float>(textureDesc.height) : defaultSize.y
	};
}

void OvCore::ECS::Components::UI::CImage::RefreshMaterial()
{
	SynchronizeTextureState();
	auto& materialManager = Global::ServiceLocator::Get<ResourceManagement::MaterialManager>();

	if (m_materialManagerRevision != materialManager.GetResourcesRevision())
	{
		m_materialStateDirty = true;
	}

	if (!m_material)
	{
		m_material = std::make_unique<OvCore::Resources::Material>();
		m_materialStateDirty = true;
		m_materialTextureDirty = true;
		m_materialTintDirty = true;
	}

	if (m_materialTexture != m_texture)
	{
		m_materialTextureDirty = true;
	}

	if (!m_materialStateDirty && !m_materialTextureDirty && !m_materialTintDirty)
	{
		return;
	}

	if (m_materialStateDirty)
	{
		const auto& imageMaterialPath = Global::ServiceLocator::Get<ResourceManagement::UIResourceRegistry>().GetDefinition().imageMaterialPath;
		auto* defaultMaterial = imageMaterialPath.empty() ?
			nullptr :
			materialManager.GetResource(imageMaterialPath);
		m_materialManagerRevision = materialManager.GetResourcesRevision();

		if (!defaultMaterial || !defaultMaterial->HasShader())
		{
			if (m_material->HasShader())
			{
				m_material->SetShader(nullptr);
			}

			m_materialTextureDirty = true;
			m_materialTintDirty = true;
			return;
		}

		if (m_material->GetShader() != defaultMaterial->GetShader())
		{
			m_material->SetShader(defaultMaterial->GetShader());
			m_materialTextureDirty = true;
			m_materialTintDirty = true;
		}

		m_material->SetOrthographicSupport(true);
		m_material->SetPerspectiveSupport(true);
		m_material->SetBlendable(true);
		m_material->SetUserInterface(true);
		m_material->SetBackfaceCulling(false);
		m_material->SetFrontfaceCulling(false);
		m_material->SetDepthTest(false);
		m_material->SetDepthWriting(false);
		m_material->SetColorWriting(true);
		m_material->SetCastShadows(false);
		m_material->SetReceiveShadows(false);
		m_material->SetCapturedByReflectionProbes(false);
		m_material->SetReceiveReflections(false);
		m_material->SetGPUInstances(1);
		m_materialStateDirty = false;
	}

	if (!m_material->IsValid())
	{
		return;
	}

	if (m_materialTextureDirty)
	{
		m_material->TrySetProperty(kTextureUniform, m_texture);
		m_materialTexture = m_texture;
		m_materialTextureDirty = false;
	}

	if (m_materialTintDirty)
	{
		m_material->TrySetProperty(kTintUniform, m_tint);
		m_materialTintDirty = false;
	}
}
