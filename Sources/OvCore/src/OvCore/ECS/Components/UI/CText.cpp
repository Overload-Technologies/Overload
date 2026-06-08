/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include <algorithm>
#include <cmath>
#include <functional>
#include <string>
#include <utility>

#include <tinyxml2.h>

#include <OvCore/ECS/Actor.h>
#include <OvCore/ECS/Components/UI/CText.h>
#include <OvCore/ECS/Components/UI/TextMeshBuilder.h>
#include <OvCore/Global/ServiceLocator.h>
#include <OvCore/Helpers/GUIDrawer.h>
#include <OvCore/Helpers/Serializer.h>
#include <OvCore/ResourceManagement/FontManager.h>
#include <OvCore/ResourceManagement/MaterialManager.h>
#include <OvTools/Utils/PathParser.h>
#include <OvUI/Types/Color.h>
#include <OvUI/Widgets/InputFields/InputText.h>
#include <OvUI/Widgets/Selection/ComboBox.h>

namespace
{
	constexpr float kMinimumFontSize = 1.0f;
	constexpr float kSizeUpdateEpsilon = 0.0001f;
	constexpr const char* kDefaultMaterialPath = ":Materials\\Text.ovmat";
	constexpr const char* kColorUniform = "u_Color";

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

	OvCore::ECS::Components::UI::CText::EHorizontalAlignment ToHorizontalAlignment(int p_value)
	{
		using EHorizontalAlignment = OvCore::ECS::Components::UI::CText::EHorizontalAlignment;

		switch (p_value)
		{
		case static_cast<int>(EHorizontalAlignment::CENTER):
			return EHorizontalAlignment::CENTER;
		case static_cast<int>(EHorizontalAlignment::RIGHT):
			return EHorizontalAlignment::RIGHT;
		case static_cast<int>(EHorizontalAlignment::LEFT):
		default:
			return EHorizontalAlignment::LEFT;
		}
	}

	OvCore::ECS::Components::UI::CText::EVerticalAlignment ToVerticalAlignment(int p_value)
	{
		using EVerticalAlignment = OvCore::ECS::Components::UI::CText::EVerticalAlignment;

		switch (p_value)
		{
		case static_cast<int>(EVerticalAlignment::CENTER):
			return EVerticalAlignment::CENTER;
		case static_cast<int>(EVerticalAlignment::BOTTOM):
			return EVerticalAlignment::BOTTOM;
		case static_cast<int>(EVerticalAlignment::TOP):
		default:
			return EVerticalAlignment::TOP;
		}
	}

	bool IsNearlyEqual(float p_left, float p_right)
	{
		return std::abs(p_left - p_right) <= kSizeUpdateEpsilon;
	}

	bool IsSameSize(const OvMaths::FVector2& p_left, const OvMaths::FVector2& p_right)
	{
		return IsNearlyEqual(p_left.x, p_right.x) && IsNearlyEqual(p_left.y, p_right.y);
	}

	OvCore::ECS::Components::UI::TextMeshBuilder::EHorizontalAlignment ToTextMeshBuilderAlignment(
		OvCore::ECS::Components::UI::CText::EHorizontalAlignment p_alignment
	)
	{
		using EHorizontalAlignment = OvCore::ECS::Components::UI::CText::EHorizontalAlignment;
		using EBuilderAlignment = OvCore::ECS::Components::UI::TextMeshBuilder::EHorizontalAlignment;

		switch (p_alignment)
		{
		case EHorizontalAlignment::CENTER:
			return EBuilderAlignment::CENTER;
		case EHorizontalAlignment::RIGHT:
			return EBuilderAlignment::RIGHT;
		case EHorizontalAlignment::LEFT:
		default:
			return EBuilderAlignment::LEFT;
		}
	}

	OvCore::ECS::Components::UI::TextMeshBuilder::EVerticalAlignment ToTextMeshBuilderAlignment(
		OvCore::ECS::Components::UI::CText::EVerticalAlignment p_alignment
	)
	{
		using EVerticalAlignment = OvCore::ECS::Components::UI::CText::EVerticalAlignment;
		using EBuilderAlignment = OvCore::ECS::Components::UI::TextMeshBuilder::EVerticalAlignment;

		switch (p_alignment)
		{
		case EVerticalAlignment::CENTER:
			return EBuilderAlignment::CENTER;
		case EVerticalAlignment::BOTTOM:
			return EBuilderAlignment::BOTTOM;
		case EVerticalAlignment::TOP:
		default:
			return EBuilderAlignment::TOP;
		}
	}
}

OvCore::ECS::Components::UI::CText::CText(ECS::Actor& p_owner) :
AComponent(p_owner)
{
	owner.transform.EnableUIData();
}

std::string OvCore::ECS::Components::UI::CText::GetName()
{
	return "Text";
}

std::string OvCore::ECS::Components::UI::CText::GetTypeName()
{
	return std::string{ComponentTraits<CText>::Name};
}

void OvCore::ECS::Components::UI::CText::SetText(const std::string& p_text)
{
	m_text = p_text;
	MarkMeshDirty();
}

const std::string& OvCore::ECS::Components::UI::CText::GetText() const
{
	return m_text;
}

void OvCore::ECS::Components::UI::CText::SetFontPath(const std::string& p_fontPath)
{
	m_fontPath = p_fontPath;
	m_unavailableFontPath.clear();
	MarkMeshDirty();
	RefreshMaterial();
}

const std::string& OvCore::ECS::Components::UI::CText::GetFontPath() const
{
	return m_fontPath;
}

void OvCore::ECS::Components::UI::CText::SetFontSize(float p_fontSize)
{
	m_fontSize = ClampFinite(p_fontSize, kMinimumFontSize);
	MarkMeshDirty();
}

float OvCore::ECS::Components::UI::CText::GetFontSize() const
{
	return m_fontSize;
}

void OvCore::ECS::Components::UI::CText::SetColor(const OvMaths::FVector4& p_color)
{
	m_color.x = KeepFinite(p_color.x, m_color.x);
	m_color.y = KeepFinite(p_color.y, m_color.y);
	m_color.z = KeepFinite(p_color.z, m_color.z);
	m_color.w = KeepFinite(p_color.w, m_color.w);
	RefreshMaterial();
}

const OvMaths::FVector4& OvCore::ECS::Components::UI::CText::GetColor() const
{
	return m_color;
}

void OvCore::ECS::Components::UI::CText::SetHorizontalAlignment(EHorizontalAlignment p_alignment)
{
	m_horizontalAlignment = ToHorizontalAlignment(static_cast<int>(p_alignment));
	MarkMeshDirty();
}

OvCore::ECS::Components::UI::CText::EHorizontalAlignment OvCore::ECS::Components::UI::CText::GetHorizontalAlignment() const
{
	return m_horizontalAlignment;
}

void OvCore::ECS::Components::UI::CText::SetVerticalAlignment(EVerticalAlignment p_alignment)
{
	m_verticalAlignment = ToVerticalAlignment(static_cast<int>(p_alignment));
	MarkMeshDirty();
}

OvCore::ECS::Components::UI::CText::EVerticalAlignment OvCore::ECS::Components::UI::CText::GetVerticalAlignment() const
{
	return m_verticalAlignment;
}

OvRendering::Resources::Mesh* OvCore::ECS::Components::UI::CText::GetMesh() const
{
	RebuildMesh();
	return m_mesh.get();
}

OvRendering::Resources::Mesh* OvCore::ECS::Components::UI::CText::GetMesh(const OvMaths::FVector2& p_resolvedSize) const
{
	RebuildMesh(p_resolvedSize);
	return m_mesh.get();
}

OvRendering::Data::Material* OvCore::ECS::Components::UI::CText::GetMaterial()
{
	RefreshMaterial();
	return m_material && m_material->IsValid() ? m_material.get() : nullptr;
}

const OvMaths::FVector2& OvCore::ECS::Components::UI::CText::GetSize() const
{
	RebuildLayout();
	return m_size;
}

OvMaths::FVector2 OvCore::ECS::Components::UI::CText::GetSize(const OvMaths::FVector2& p_resolvedSize) const
{
	RebuildLayout(p_resolvedSize);
	return m_size;
}

void OvCore::ECS::Components::UI::CText::OnSerialize(tinyxml2::XMLDocument& p_doc, tinyxml2::XMLNode* p_node)
{
	Helpers::Serializer::SerializeString(p_doc, p_node, "text", m_text);
	Helpers::Serializer::SerializeString(p_doc, p_node, "font_path", m_fontPath);
	Helpers::Serializer::SerializeFloat(p_doc, p_node, "font_size", m_fontSize);
	Helpers::Serializer::SerializeVec4(p_doc, p_node, "color", m_color);
	Helpers::Serializer::SerializeInt(p_doc, p_node, "horizontal_alignment", static_cast<int>(m_horizontalAlignment));
	Helpers::Serializer::SerializeInt(p_doc, p_node, "vertical_alignment", static_cast<int>(m_verticalAlignment));
}

void OvCore::ECS::Components::UI::CText::OnDeserialize(tinyxml2::XMLDocument& p_doc, tinyxml2::XMLNode* p_node)
{
	if (p_node->FirstChildElement("text"))
	{
		auto text = m_text;
		Helpers::Serializer::DeserializeString(p_doc, p_node, "text", text);
		SetText(text);
	}

	if (p_node->FirstChildElement("font_path"))
	{
		auto fontPath = m_fontPath;
		Helpers::Serializer::DeserializeString(p_doc, p_node, "font_path", fontPath);
		SetFontPath(fontPath);
	}

	if (p_node->FirstChildElement("font_size"))
	{
		auto fontSize = m_fontSize;
		Helpers::Serializer::DeserializeFloat(p_doc, p_node, "font_size", fontSize);
		SetFontSize(fontSize);
	}

	if (p_node->FirstChildElement("color"))
	{
		auto color = m_color;
		Helpers::Serializer::DeserializeVec4(p_doc, p_node, "color", color);
		SetColor(color);
	}

	if (p_node->FirstChildElement("horizontal_alignment"))
	{
		auto horizontalAlignment = static_cast<int>(m_horizontalAlignment);
		Helpers::Serializer::DeserializeInt(p_doc, p_node, "horizontal_alignment", horizontalAlignment);
		SetHorizontalAlignment(ToHorizontalAlignment(horizontalAlignment));
	}

	if (p_node->FirstChildElement("vertical_alignment"))
	{
		auto verticalAlignment = static_cast<int>(m_verticalAlignment);
		Helpers::Serializer::DeserializeInt(p_doc, p_node, "vertical_alignment", verticalAlignment);
		SetVerticalAlignment(ToVerticalAlignment(verticalAlignment));
	}
}

void OvCore::ECS::Components::UI::CText::OnInspector(OvUI::Internal::WidgetContainer& p_root)
{
	Helpers::GUIDrawer::CreateTitle(p_root, "Text");
	auto& textInput = p_root.CreateWidget<OvUI::Widgets::InputFields::InputText>("");
	textInput.multiline = true;
	textInput.multilineHeight = 100.0f;
	textInput.fullWidth = true;

	auto& textDispatcher = textInput.AddPlugin<OvUI::Plugins::DataDispatcher<std::string>>();
	textDispatcher.RegisterGatherer([this]() { return GetText(); });
	textDispatcher.RegisterProvider([this](std::string p_value) { SetText(p_value); });

	Helpers::GUIDrawer::DrawAsset(
		p_root,
		"Font",
		[this]() { return GetFontPath(); },
		[this](std::string p_value) { SetFontPath(p_value); },
		OvTools::Utils::PathParser::EFileType::FONT
	);

	Helpers::GUIDrawer::DrawScalar<float>(
		p_root,
		"Font Size",
		std::bind(&CText::GetFontSize, this),
		std::bind(&CText::SetFontSize, this, std::placeholders::_1),
		1.0f,
		kMinimumFontSize
	);

	Helpers::GUIDrawer::CreateTitle(p_root, "Horizontal Alignment");
	auto& horizontalAlignment = p_root.CreateWidget<OvUI::Widgets::Selection::ComboBox>(static_cast<int>(GetHorizontalAlignment()));
	horizontalAlignment.choices.emplace(static_cast<int>(EHorizontalAlignment::LEFT), "Left");
	horizontalAlignment.choices.emplace(static_cast<int>(EHorizontalAlignment::CENTER), "Center");
	horizontalAlignment.choices.emplace(static_cast<int>(EHorizontalAlignment::RIGHT), "Right");
	horizontalAlignment.ValueChangedEvent += [this](int p_choice)
	{
		SetHorizontalAlignment(ToHorizontalAlignment(p_choice));
	};

	Helpers::GUIDrawer::CreateTitle(p_root, "Vertical Alignment");
	auto& verticalAlignment = p_root.CreateWidget<OvUI::Widgets::Selection::ComboBox>(static_cast<int>(GetVerticalAlignment()));
	verticalAlignment.choices.emplace(static_cast<int>(EVerticalAlignment::TOP), "Top");
	verticalAlignment.choices.emplace(static_cast<int>(EVerticalAlignment::CENTER), "Center");
	verticalAlignment.choices.emplace(static_cast<int>(EVerticalAlignment::BOTTOM), "Bottom");
	verticalAlignment.ValueChangedEvent += [this](int p_choice)
	{
		SetVerticalAlignment(ToVerticalAlignment(p_choice));
	};

	Helpers::GUIDrawer::DrawColor(
		p_root,
		"Color",
		[this]() { return ToColor(m_color); },
		[this](OvUI::Types::Color p_value) { SetColor(ToVec4(p_value)); },
		true
	);
}

OvRendering::Resources::Font* OvCore::ECS::Components::UI::CText::GetFont() const
{
	if (m_fontPath.empty() || m_fontPath == "?")
	{
		return nullptr;
	}

	auto& fontManager = Global::ServiceLocator::Get<ResourceManagement::FontManager>();
	if (m_unavailableFontPath == m_fontPath)
	{
		return fontManager.GetResource(m_fontPath, false);
	}

	auto* font = fontManager.GetResource(m_fontPath);
	if (!font)
	{
		m_unavailableFontPath = m_fontPath;
	}

	return font;
}

void OvCore::ECS::Components::UI::CText::MarkMeshDirty()
{
	m_layoutDirty = true;
	m_meshDirty = true;
}

void OvCore::ECS::Components::UI::CText::RebuildLayout() const
{
	RebuildLayout(owner.transform.GetUISize());
}

void OvCore::ECS::Components::UI::CText::RebuildLayout(const OvMaths::FVector2& p_uiSize) const
{
	if (!m_layoutDirty && IsSameSize(m_lastLayoutUISize, p_uiSize))
	{
		return;
	}

	m_layoutDirty = false;
	m_lastLayoutUISize = p_uiSize;

	const auto textLayout = TextMeshBuilder::Build({
		.text = m_text,
		.font = GetFont(),
		.fontSize = m_fontSize,
		.uiSize = p_uiSize,
		.horizontalAlignment = ToTextMeshBuilderAlignment(m_horizontalAlignment),
		.verticalAlignment = ToTextMeshBuilderAlignment(m_verticalAlignment),
		.buildMesh = false
	});

	m_size = textLayout.size;
}

void OvCore::ECS::Components::UI::CText::RebuildMesh() const
{
	RebuildMesh(owner.transform.GetUISize());
}

void OvCore::ECS::Components::UI::CText::RebuildMesh(const OvMaths::FVector2& p_uiSize) const
{
	if (!m_meshDirty && IsSameSize(m_lastMeshUISize, p_uiSize))
	{
		return;
	}

	m_meshDirty = false;
	m_lastMeshUISize = p_uiSize;

	auto textMesh = TextMeshBuilder::Build({
		.text = m_text,
		.font = GetFont(),
		.fontSize = m_fontSize,
		.uiSize = p_uiSize,
		.horizontalAlignment = ToTextMeshBuilderAlignment(m_horizontalAlignment),
		.verticalAlignment = ToTextMeshBuilderAlignment(m_verticalAlignment),
		.buildMesh = true
	});

	m_size = textMesh.size;
	m_layoutDirty = false;
	m_lastLayoutUISize = p_uiSize;
	m_mesh = std::move(textMesh.mesh);
}

void OvCore::ECS::Components::UI::CText::RefreshMaterial()
{
	if (!m_material)
	{
		m_material = std::make_unique<OvRendering::Data::Material>();
	}

	auto* defaultMaterial = Global::ServiceLocator::Get<ResourceManagement::MaterialManager>().GetResource(kDefaultMaterialPath);
	auto* font = GetFont();

	if (
		!defaultMaterial ||
		!defaultMaterial->HasShader() ||
		!font ||
		!font->EnsureEmbeddedMaterial(defaultMaterial->GetShader(), m_fontSize)
	)
	{
		m_material->SetShader(nullptr);
		return;
	}

	auto* embeddedMaterial = font->GetEmbeddedMaterial(m_fontSize);
	if (!embeddedMaterial || !embeddedMaterial->IsValid())
	{
		m_material->SetShader(nullptr);
		return;
	}

	*m_material = *embeddedMaterial;
	m_material->TrySetProperty(kColorUniform, m_color);
}
