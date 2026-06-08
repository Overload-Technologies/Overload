/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include <algorithm>
#include <array>
#include <cmath>
#include <cstdint>

#include <tinyxml2.h>

#include <OvCore/ECS/Actor.h>
#include <OvCore/ECS/Components/UI/CImage.h>
#include <OvCore/Global/ServiceLocator.h>
#include <OvCore/Helpers/GUIDrawer.h>
#include <OvCore/Helpers/Serializer.h>
#include <OvCore/ResourceManagement/MaterialManager.h>
#include <OvCore/ResourceManagement/TextureManager.h>
#include <OvRendering/Geometry/Vertex.h>
#include <OvUI/Types/Color.h>

namespace
{
	constexpr float kMinimumSize = 0.0001f;
	constexpr const char* kDefaultMaterialPath = ":Materials\\Image.ovmat";
	constexpr const char* kTextureUniform = "u_Image";
	constexpr const char* kTintUniform = "u_Tint";

	OvMaths::FVector2 GetDefaultImageSize()
	{
		return { 100.0f, 100.0f };
	}

	float ClampFinite(float p_value, float p_min)
	{
		return std::isfinite(p_value) ? std::max(p_value, p_min) : p_min;
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

	bool IsRegisteredTexture(const OvRendering::Resources::Texture* p_texture)
	{
		if (!p_texture)
		{
			return false;
		}

		for (const auto& [_, texture] : OvCore::Global::ServiceLocator::Get<OvCore::ResourceManagement::TextureManager>().GetResources())
		{
			if (texture == p_texture)
			{
				return true;
			}
		}

		return false;
	}
}

OvCore::ECS::Components::UI::CImage::CImage(ECS::Actor& p_owner) :
AComponent(p_owner)
{
	owner.transform.EnableUIData();
	EnsureTransformSize();
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
	m_texture = p_texture;
	RefreshMaterial();
}

OvRendering::Resources::Texture* OvCore::ECS::Components::UI::CImage::GetTexture() const
{
	return m_texture;
}

void OvCore::ECS::Components::UI::CImage::SetSize(const OvMaths::FVector2& p_size)
{
	owner.transform.SetUISize({
		ClampFinite(p_size.x, kMinimumSize),
		ClampFinite(p_size.y, kMinimumSize)
	});
}

OvMaths::FVector2 OvCore::ECS::Components::UI::CImage::GetSize() const
{
	const auto& transformSize = owner.transform.GetUISize();
	const auto defaultSize = GetDefaultImageSize();

	return {
		transformSize.x > 0.0f ? transformSize.x : defaultSize.x,
		transformSize.y > 0.0f ? transformSize.y : defaultSize.y
	};
}

OvMaths::FVector2 OvCore::ECS::Components::UI::CImage::GetIntrinsicSize() const
{
	return GetDefaultImageSize();
}

void OvCore::ECS::Components::UI::CImage::SetTint(const OvMaths::FVector4& p_tint)
{
	m_tint.x = KeepFinite(p_tint.x, m_tint.x);
	m_tint.y = KeepFinite(p_tint.y, m_tint.y);
	m_tint.z = KeepFinite(p_tint.z, m_tint.z);
	m_tint.w = KeepFinite(p_tint.w, m_tint.w);
	RefreshMaterial();
}

const OvMaths::FVector4& OvCore::ECS::Components::UI::CImage::GetTint() const
{
	return m_tint;
}

OvRendering::Resources::Mesh& OvCore::ECS::Components::UI::CImage::GetMesh() const
{
	return *m_mesh;
}

OvCore::Resources::Material* OvCore::ECS::Components::UI::CImage::GetMaterial()
{
	RefreshMaterial();
	return m_material && m_material->IsValid() ? m_material.get() : nullptr;
}

void OvCore::ECS::Components::UI::CImage::OnSerialize(tinyxml2::XMLDocument& p_doc, tinyxml2::XMLNode* p_node)
{
	ValidateTextureReference();
	Helpers::Serializer::SerializeTexture(p_doc, p_node, "texture", m_texture);
	Helpers::Serializer::SerializeVec4(p_doc, p_node, "tint", m_tint);
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
}

void OvCore::ECS::Components::UI::CImage::OnInspector(OvUI::Internal::WidgetContainer& p_root)
{
	ValidateTextureReference();
	Helpers::GUIDrawer::DrawTexture(p_root, "Texture", m_texture);

	Helpers::GUIDrawer::DrawColor(
		p_root,
		"Tint",
		[this]() { return ToColor(m_tint); },
		[this](OvUI::Types::Color p_value) { SetTint(ToVec4(p_value)); },
		true
	);
}

void OvCore::ECS::Components::UI::CImage::EnsureTransformSize()
{
	auto uiSize = owner.transform.GetUISize();
	const auto defaultSize = GetDefaultImageSize();
	bool hasChanges = false;

	if (uiSize.x <= 0.0f)
	{
		uiSize.x = defaultSize.x;
		hasChanges = true;
	}

	if (uiSize.y <= 0.0f)
	{
		uiSize.y = defaultSize.y;
		hasChanges = true;
	}

	if (hasChanges)
	{
		owner.transform.SetUISize(uiSize);
	}
}

void OvCore::ECS::Components::UI::CImage::RebuildMesh()
{
	const auto size = GetIntrinsicSize();

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

void OvCore::ECS::Components::UI::CImage::ValidateTextureReference()
{
	if (m_texture && !IsRegisteredTexture(m_texture))
	{
		m_texture = nullptr;
	}
}

void OvCore::ECS::Components::UI::CImage::RefreshMaterial()
{
	ValidateTextureReference();

	if (!m_material)
	{
		m_material = std::make_unique<OvCore::Resources::Material>();
	}

	auto* defaultMaterial = Global::ServiceLocator::Get<ResourceManagement::MaterialManager>().GetResource(kDefaultMaterialPath);
	if (!defaultMaterial || !defaultMaterial->HasShader())
	{
		m_material->SetShader(nullptr);
		return;
	}

	if (m_material->GetShader() != defaultMaterial->GetShader())
	{
		m_material->SetShader(defaultMaterial->GetShader());
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

	m_material->TrySetProperty(kTextureUniform, m_texture);
	m_material->TrySetProperty(kTintUniform, m_tint);
}
