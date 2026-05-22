/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include <algorithm>
#include <cmath>
#include <functional>

#include <tinyxml2.h>

#include <OvCore/ECS/Components/UI/CTransform2D.h>
#include <OvCore/Helpers/GUIDrawer.h>
#include <OvCore/Helpers/Serializer.h>

#include <OvUI/Widgets/Selection/ComboBox.h>

namespace
{
	constexpr float kDegreesToRadians = 3.14159265359f / 180.0f;
	constexpr float kMinimumScale = 0.0001f;

	float ClampScaleAxis(float p_value, float p_fallback)
	{
		return std::isfinite(p_value) ? std::max(p_value, kMinimumScale) : p_fallback;
	}

	float KeepFinite(float p_value, float p_fallback)
	{
		return std::isfinite(p_value) ? p_value : p_fallback;
	}

	OvCore::ECS::Components::UI::CTransform2D::EAnchorPreset ToAnchorPreset(int p_value)
	{
		using EAnchorPreset = OvCore::ECS::Components::UI::CTransform2D::EAnchorPreset;

		switch (p_value)
		{
		case static_cast<int>(EAnchorPreset::TOP_LEFT):
			return EAnchorPreset::TOP_LEFT;
		case static_cast<int>(EAnchorPreset::TOP_CENTER):
			return EAnchorPreset::TOP_CENTER;
		case static_cast<int>(EAnchorPreset::TOP_RIGHT):
			return EAnchorPreset::TOP_RIGHT;
		case static_cast<int>(EAnchorPreset::MIDDLE_LEFT):
			return EAnchorPreset::MIDDLE_LEFT;
		case static_cast<int>(EAnchorPreset::MIDDLE_RIGHT):
			return EAnchorPreset::MIDDLE_RIGHT;
		case static_cast<int>(EAnchorPreset::BOTTOM_LEFT):
			return EAnchorPreset::BOTTOM_LEFT;
		case static_cast<int>(EAnchorPreset::BOTTOM_CENTER):
			return EAnchorPreset::BOTTOM_CENTER;
		case static_cast<int>(EAnchorPreset::BOTTOM_RIGHT):
			return EAnchorPreset::BOTTOM_RIGHT;
		case static_cast<int>(EAnchorPreset::CENTER):
		default:
			return EAnchorPreset::CENTER;
		}
	}

	OvMaths::FVector2 GetAnchorRatio(OvCore::ECS::Components::UI::CTransform2D::EAnchorPreset p_anchorPreset)
	{
		using EAnchorPreset = OvCore::ECS::Components::UI::CTransform2D::EAnchorPreset;

		switch (p_anchorPreset)
		{
		case EAnchorPreset::TOP_LEFT:
			return { -0.5f, 0.5f };
		case EAnchorPreset::TOP_CENTER:
			return { 0.0f, 0.5f };
		case EAnchorPreset::TOP_RIGHT:
			return { 0.5f, 0.5f };
		case EAnchorPreset::MIDDLE_LEFT:
			return { -0.5f, 0.0f };
		case EAnchorPreset::MIDDLE_RIGHT:
			return { 0.5f, 0.0f };
		case EAnchorPreset::BOTTOM_LEFT:
			return { -0.5f, -0.5f };
		case EAnchorPreset::BOTTOM_CENTER:
			return { 0.0f, -0.5f };
		case EAnchorPreset::BOTTOM_RIGHT:
			return { 0.5f, -0.5f };
		case EAnchorPreset::CENTER:
		default:
			return { 0.0f, 0.0f };
		}
	}
}

OvCore::ECS::Components::UI::CTransform2D::CTransform2D(ECS::Actor& p_owner) :
AComponent(p_owner)
{
}

std::string OvCore::ECS::Components::UI::CTransform2D::GetName()
{
	return "Transform 2D";
}

std::string OvCore::ECS::Components::UI::CTransform2D::GetTypeName()
{
	return std::string{ComponentTraits<CTransform2D>::Name};
}

void OvCore::ECS::Components::UI::CTransform2D::SetPosition(const OvMaths::FVector2& p_position)
{
	m_position.x = KeepFinite(p_position.x, m_position.x);
	m_position.y = KeepFinite(p_position.y, m_position.y);
}

const OvMaths::FVector2& OvCore::ECS::Components::UI::CTransform2D::GetPosition() const
{
	return m_position;
}

void OvCore::ECS::Components::UI::CTransform2D::SetRotation(float p_rotation)
{
	m_rotation = KeepFinite(p_rotation, m_rotation);
}

float OvCore::ECS::Components::UI::CTransform2D::GetRotation() const
{
	return m_rotation;
}

void OvCore::ECS::Components::UI::CTransform2D::SetScale(const OvMaths::FVector2& p_scale)
{
	m_scale.x = ClampScaleAxis(p_scale.x, m_scale.x);
	m_scale.y = ClampScaleAxis(p_scale.y, m_scale.y);
}

const OvMaths::FVector2& OvCore::ECS::Components::UI::CTransform2D::GetScale() const
{
	return m_scale;
}

void OvCore::ECS::Components::UI::CTransform2D::SetAnchorPreset(EAnchorPreset p_anchorPreset)
{
	m_anchorPreset = ToAnchorPreset(static_cast<int>(p_anchorPreset));
}

OvCore::ECS::Components::UI::CTransform2D::EAnchorPreset OvCore::ECS::Components::UI::CTransform2D::GetAnchorPreset() const
{
	return m_anchorPreset;
}

OvMaths::FMatrix4 OvCore::ECS::Components::UI::CTransform2D::GetMatrix(const OvMaths::FVector2& p_canvasSize, const OvMaths::FVector2& p_layoutOffset) const
{
	const auto position = GetAnchoredPosition(p_canvasSize, p_layoutOffset);

	return
		OvMaths::FMatrix4::Translation({ position.x, position.y, 0.0f }) *
		OvMaths::FMatrix4::RotationOnAxisZ(m_rotation * kDegreesToRadians) *
		OvMaths::FMatrix4::Scaling({ m_scale.x, m_scale.y, 1.0f });
}

void OvCore::ECS::Components::UI::CTransform2D::OnSerialize(tinyxml2::XMLDocument& p_doc, tinyxml2::XMLNode* p_node)
{
	Helpers::Serializer::SerializeVec2(p_doc, p_node, "position", m_position);
	Helpers::Serializer::SerializeFloat(p_doc, p_node, "rotation", m_rotation);
	Helpers::Serializer::SerializeVec2(p_doc, p_node, "scale", m_scale);
	Helpers::Serializer::SerializeInt(p_doc, p_node, "anchor_preset", static_cast<int>(m_anchorPreset));
}

void OvCore::ECS::Components::UI::CTransform2D::OnDeserialize(tinyxml2::XMLDocument& p_doc, tinyxml2::XMLNode* p_node)
{
	if (p_node->FirstChildElement("position"))
	{
		auto position = m_position;
		Helpers::Serializer::DeserializeVec2(p_doc, p_node, "position", position);
		SetPosition(position);
	}

	if (p_node->FirstChildElement("rotation"))
	{
		auto rotation = m_rotation;
		Helpers::Serializer::DeserializeFloat(p_doc, p_node, "rotation", rotation);
		SetRotation(rotation);
	}

	if (p_node->FirstChildElement("scale"))
	{
		auto scale = m_scale;
		Helpers::Serializer::DeserializeVec2(p_doc, p_node, "scale", scale);
		SetScale(scale);
	}

	if (p_node->FirstChildElement("anchor_preset"))
	{
		auto anchorPreset = static_cast<int>(m_anchorPreset);
		Helpers::Serializer::DeserializeInt(p_doc, p_node, "anchor_preset", anchorPreset);
		SetAnchorPreset(ToAnchorPreset(anchorPreset));
	}
}

void OvCore::ECS::Components::UI::CTransform2D::OnInspector(OvUI::Internal::WidgetContainer& p_root)
{
	Helpers::GUIDrawer::DrawVec2(
		p_root,
		"Position",
		[this]() { return GetPosition(); },
		[this](OvMaths::FVector2 p_value) { SetPosition(p_value); },
		1.0f
	);

	Helpers::GUIDrawer::DrawScalar<float>(
		p_root,
		"Rotation",
		std::bind(&CTransform2D::GetRotation, this),
		std::bind(&CTransform2D::SetRotation, this, std::placeholders::_1),
		0.1f
	);

	Helpers::GUIDrawer::DrawVec2(
		p_root,
		"Scale",
		[this]() { return GetScale(); },
		[this](OvMaths::FVector2 p_value) { SetScale(p_value); },
		0.01f,
		kMinimumScale
	);

	Helpers::GUIDrawer::CreateTitle(p_root, "Anchor Preset");
	auto& anchorPreset = p_root.CreateWidget<OvUI::Widgets::Selection::ComboBox>(static_cast<int>(GetAnchorPreset()));
	anchorPreset.choices.emplace(static_cast<int>(EAnchorPreset::TOP_LEFT), "Top Left");
	anchorPreset.choices.emplace(static_cast<int>(EAnchorPreset::TOP_CENTER), "Top Center");
	anchorPreset.choices.emplace(static_cast<int>(EAnchorPreset::TOP_RIGHT), "Top Right");
	anchorPreset.choices.emplace(static_cast<int>(EAnchorPreset::MIDDLE_LEFT), "Middle Left");
	anchorPreset.choices.emplace(static_cast<int>(EAnchorPreset::CENTER), "Center");
	anchorPreset.choices.emplace(static_cast<int>(EAnchorPreset::MIDDLE_RIGHT), "Middle Right");
	anchorPreset.choices.emplace(static_cast<int>(EAnchorPreset::BOTTOM_LEFT), "Bottom Left");
	anchorPreset.choices.emplace(static_cast<int>(EAnchorPreset::BOTTOM_CENTER), "Bottom Center");
	anchorPreset.choices.emplace(static_cast<int>(EAnchorPreset::BOTTOM_RIGHT), "Bottom Right");
	anchorPreset.ValueChangedEvent += [this](int p_choice)
	{
		SetAnchorPreset(ToAnchorPreset(p_choice));
	};
}

OvMaths::FVector2 OvCore::ECS::Components::UI::CTransform2D::GetAnchoredPosition(const OvMaths::FVector2& p_canvasSize, const OvMaths::FVector2& p_layoutOffset) const
{
	const auto anchorRatio = GetAnchorRatio(m_anchorPreset);
	const OvMaths::FVector2 anchorOffset = {
		KeepFinite(p_canvasSize.x, 0.0f) * anchorRatio.x,
		KeepFinite(p_canvasSize.y, 0.0f) * anchorRatio.y
	};

	return {
		anchorOffset.x + p_layoutOffset.x + m_position.x,
		anchorOffset.y + p_layoutOffset.y + m_position.y
	};
}
