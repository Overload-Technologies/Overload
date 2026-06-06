/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <functional>
#include <limits>
#include <vector>

#include <tinyxml2.h>

#include <OvCore/ECS/Actor.h>
#include <OvCore/ECS/Components/UI/CText.h>
#include <OvCore/Global/ServiceLocator.h>
#include <OvCore/Helpers/GUIDrawer.h>
#include <OvCore/Helpers/Serializer.h>
#include <OvCore/ResourceManagement/FontManager.h>
#include <OvCore/ResourceManagement/MaterialManager.h>
#include <OvRendering/Geometry/Vertex.h>
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

	OvMaths::FVector2 ResolveTextSize(const OvMaths::FVector2& p_contentSize, const OvMaths::FVector2& p_uiSize)
	{
		return {
			p_uiSize.x > 0.0f ? p_uiSize.x : p_contentSize.x,
			p_uiSize.y > 0.0f ? p_uiSize.y : p_contentSize.y
		};
	}

	float GetAlignedCenterX(float p_textWidth, float p_contentWidth, OvCore::ECS::Components::UI::CText::EHorizontalAlignment p_alignment)
	{
		switch (p_alignment)
		{
		case OvCore::ECS::Components::UI::CText::EHorizontalAlignment::CENTER:
			return 0.0f;
		case OvCore::ECS::Components::UI::CText::EHorizontalAlignment::RIGHT:
			return p_textWidth * 0.5f - p_contentWidth * 0.5f;
		case OvCore::ECS::Components::UI::CText::EHorizontalAlignment::LEFT:
		default:
			return -p_textWidth * 0.5f + p_contentWidth * 0.5f;
		}
	}

	float GetAlignedCenterY(float p_textHeight, float p_contentHeight, OvCore::ECS::Components::UI::CText::EVerticalAlignment p_alignment)
	{
		switch (p_alignment)
		{
		case OvCore::ECS::Components::UI::CText::EVerticalAlignment::CENTER:
			return 0.0f;
		case OvCore::ECS::Components::UI::CText::EVerticalAlignment::BOTTOM:
			return -p_textHeight * 0.5f + p_contentHeight * 0.5f;
		case OvCore::ECS::Components::UI::CText::EVerticalAlignment::TOP:
		default:
			return p_textHeight * 0.5f - p_contentHeight * 0.5f;
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

OvRendering::Data::Material* OvCore::ECS::Components::UI::CText::GetMaterial()
{
	RefreshMaterial();
	return m_material && m_material->IsValid() ? m_material.get() : nullptr;
}

const OvMaths::FVector2& OvCore::ECS::Components::UI::CText::GetSize() const
{
	RebuildMesh();
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
	m_meshDirty = true;
}

void OvCore::ECS::Components::UI::CText::RebuildMesh() const
{
	const auto uiSize = owner.transform.GetUISize();
	if (!m_meshDirty && IsSameSize(m_lastUISize, uiSize))
	{
		return;
	}

	m_meshDirty = false;
	m_lastUISize = uiSize;
	m_mesh.reset();
	m_size = ResolveTextSize(OvMaths::FVector2::Zero, uiSize);

	auto* font = GetFont();
	if (!font || m_text.empty() || !font->EnsurePixelSize(m_fontSize))
	{
		return;
	}

	const float bakedPixelSize = font->GetPixelSize(m_fontSize);
	if (bakedPixelSize <= 0.0f)
	{
		return;
	}

	const float scale = m_fontSize / bakedPixelSize;
	const float lineHeight = font->GetLineHeight(m_fontSize);
	std::vector<OvRendering::Geometry::Vertex> vertices;
	std::vector<uint32_t> indices;
	vertices.reserve(m_text.size() * 4);
	indices.reserve(m_text.size() * 6);

	struct LineInfo
	{
		size_t firstVertex = 0;
		size_t lastVertex = 0;
		float minX = std::numeric_limits<float>::max();
		float maxX = std::numeric_limits<float>::lowest();
		bool hasGeometry = false;
	};

	std::vector<LineInfo> lines;
	lines.push_back({});
	lines.back().firstVertex = 0;

	float cursorX = 0.0f;
	float baselineY = 0.0f;
	float minX = std::numeric_limits<float>::max();
	float minY = std::numeric_limits<float>::max();
	float maxX = std::numeric_limits<float>::lowest();
	float maxY = std::numeric_limits<float>::lowest();

	const auto* fallbackGlyph = font->GetGlyph('?', m_fontSize);

	for (const char character : m_text)
	{
		if (character == '\r')
		{
			continue;
		}

		if (character == '\n')
		{
			lines.back().lastVertex = vertices.size();
			lines.push_back({});
			lines.back().firstVertex = vertices.size();
			cursorX = 0.0f;
			baselineY -= lineHeight * scale;
			continue;
		}

		const auto* glyph = font->GetGlyph(character, m_fontSize);
		if (!glyph)
		{
			glyph = fallbackGlyph;
		}

		if (!glyph)
		{
			continue;
		}

		const float x0 = cursorX + glyph->xOffset * scale;
		const float topY = baselineY - glyph->yOffset * scale;
		const float x1 = x0 + glyph->width * scale;
		const float bottomY = topY - glyph->height * scale;

		const uint32_t firstVertex = static_cast<uint32_t>(vertices.size());
		vertices.push_back({ { x0, bottomY, 0.0f }, { glyph->uMin, glyph->vMax }, { 0.0f, 0.0f, 1.0f }, {}, {} });
		vertices.push_back({ { x1, bottomY, 0.0f }, { glyph->uMax, glyph->vMax }, { 0.0f, 0.0f, 1.0f }, {}, {} });
		vertices.push_back({ { x1, topY,    0.0f }, { glyph->uMax, glyph->vMin }, { 0.0f, 0.0f, 1.0f }, {}, {} });
		vertices.push_back({ { x0, topY,    0.0f }, { glyph->uMin, glyph->vMin }, { 0.0f, 0.0f, 1.0f }, {}, {} });

		indices.push_back(firstVertex + 0);
		indices.push_back(firstVertex + 1);
		indices.push_back(firstVertex + 2);
		indices.push_back(firstVertex + 0);
		indices.push_back(firstVertex + 2);
		indices.push_back(firstVertex + 3);

		minX = std::min(minX, x0);
		minY = std::min(minY, bottomY);
		maxX = std::max(maxX, x1);
		maxY = std::max(maxY, topY);

		auto& line = lines.back();
		line.hasGeometry = true;
		line.minX = std::min(line.minX, x0);
		line.maxX = std::max(line.maxX, x1);
		line.lastVertex = vertices.size();

		cursorX += glyph->xAdvance * scale;
	}

	lines.back().lastVertex = vertices.size();

	if (vertices.empty() || indices.empty())
	{
		return;
	}

	m_size = {
		std::max(maxX - minX, 0.0f),
		std::max(maxY - minY, 0.0f)
	};

	const auto contentSize = m_size;
	m_size = ResolveTextSize(contentSize, uiSize);

	for (const auto& line : lines)
	{
		if (!line.hasGeometry || line.lastVertex <= line.firstVertex)
		{
			continue;
		}

		const float lineWidth = std::max(line.maxX - line.minX, 0.0f);
		const float lineCenterX = line.minX + lineWidth * 0.5f;
		const float alignedLineCenterX = GetAlignedCenterX(m_size.x, lineWidth, m_horizontalAlignment);
		const float lineOffsetX = alignedLineCenterX - lineCenterX;

		for (size_t vertexIndex = line.firstVertex; vertexIndex < line.lastVertex; ++vertexIndex)
		{
			vertices[vertexIndex].position[0] += lineOffsetX;
		}
	}

	const float contentCenterY = minY + contentSize.y * 0.5f;
	const float alignedCenterY = GetAlignedCenterY(m_size.y, contentSize.y, m_verticalAlignment);
	const float globalOffsetY = alignedCenterY - contentCenterY;

	for (auto& vertex : vertices)
	{
		vertex.position[1] += globalOffsetY;
	}

	m_mesh = std::make_unique<OvRendering::Resources::Mesh>(vertices, indices);
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
