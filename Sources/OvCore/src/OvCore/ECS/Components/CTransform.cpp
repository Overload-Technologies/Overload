/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include <algorithm>
#include <array>
#include <cmath>
#include <functional>

#include <tinyxml2.h>

#include <OvCore/ECS/Components/CTransform.h>
#include <OvCore/ECS/Actor.h>
#include <OvCore/ECS/Components/UI/UITransformResolver.h>
#include <OvCore/Helpers/GUIDrawer.h>
#include <OvCore/Helpers/Serializer.h>
#include <OvUI/Widgets/Drags/DragMultipleScalars.h>
#include <OvUI/Widgets/Selection/ComboBox.h>

namespace
{
	constexpr float kMinimumScale = 0.0001f;
	constexpr float kMinimumSize = 0.0f;
	constexpr float kMinimumPivot = -1.0f;
	constexpr float kMaximumPivot = 1.0f;

	float ClampScaleAxis(float p_value, float p_fallback)
	{
		return std::isfinite(p_value) ? std::max(p_value, kMinimumScale) : p_fallback;
	}

	float KeepFinite(float p_value, float p_fallback)
	{
		return std::isfinite(p_value) ? p_value : p_fallback;
	}

	float ClampPivotAxis(float p_value, float p_fallback)
	{
		if (!std::isfinite(p_value))
		{
			return p_fallback;
		}

		return std::clamp(p_value, kMinimumPivot, kMaximumPivot);
	}
}

OvCore::ECS::Components::CTransform::CTransform(ECS::Actor& p_owner, OvMaths::FVector3 p_localPosition, OvMaths::FQuaternion p_localRotation, OvMaths::FVector3 p_localScale) :
AComponent(p_owner)
{
	m_transform.GenerateMatricesLocal(p_localPosition, p_localRotation, p_localScale);
}

std::string OvCore::ECS::Components::CTransform::GetName()
{
	return "Transform";
}

std::string OvCore::ECS::Components::CTransform::GetTypeName()
{
	return std::string{ComponentTraits<CTransform>::Name};
}

void OvCore::ECS::Components::CTransform::SetParent(CTransform& p_parent)
{
	m_transform.SetParent(p_parent.GetFTransform());
}

bool OvCore::ECS::Components::CTransform::RemoveParent()
{
	return m_transform.RemoveParent();
}

bool OvCore::ECS::Components::CTransform::HasParent() const
{
	return m_transform.HasParent();
}

void OvCore::ECS::Components::CTransform::SetLocalPosition(OvMaths::FVector3 p_newPosition)
{
	m_transform.SetLocalPosition(p_newPosition);
}

void OvCore::ECS::Components::CTransform::SetLocalRotation(OvMaths::FQuaternion p_newRotation)
{
	m_transform.SetLocalRotation(p_newRotation);
}

void OvCore::ECS::Components::CTransform::SetLocalScale(OvMaths::FVector3 p_newScale)
{
	m_transform.SetLocalScale(p_newScale);
}

void OvCore::ECS::Components::CTransform::SetWorldPosition(OvMaths::FVector3 p_newPosition)
{
	m_transform.SetWorldPosition(p_newPosition);
}

void OvCore::ECS::Components::CTransform::SetWorldRotation(OvMaths::FQuaternion p_newRotation)
{
	m_transform.SetWorldRotation(p_newRotation);
}

void OvCore::ECS::Components::CTransform::SetWorldScale(OvMaths::FVector3 p_newScale)
{
	m_transform.SetWorldScale(p_newScale);
}

void OvCore::ECS::Components::CTransform::TranslateLocal(const OvMaths::FVector3 & p_translation)
{
	m_transform.TranslateLocal(p_translation);
}

void OvCore::ECS::Components::CTransform::RotateLocal(const OvMaths::FQuaternion & p_rotation)
{
	m_transform.RotateLocal(p_rotation);
}

void OvCore::ECS::Components::CTransform::ScaleLocal(const OvMaths::FVector3 & p_scale)
{
	m_transform.ScaleLocal(p_scale);
}

const OvMaths::FVector3 & OvCore::ECS::Components::CTransform::GetLocalPosition() const
{
	return m_transform.GetLocalPosition();
}

const OvMaths::FQuaternion & OvCore::ECS::Components::CTransform::GetLocalRotation() const
{
	return m_transform.GetLocalRotation();
}

const OvMaths::FVector3 & OvCore::ECS::Components::CTransform::GetLocalScale() const
{
	return m_transform.GetLocalScale();
}

const OvMaths::FVector3 & OvCore::ECS::Components::CTransform::GetWorldPosition() const
{
	return m_transform.GetWorldPosition();
}

const OvMaths::FQuaternion & OvCore::ECS::Components::CTransform::GetWorldRotation() const
{
	return m_transform.GetWorldRotation();
}

const OvMaths::FVector3 & OvCore::ECS::Components::CTransform::GetWorldScale() const
{
	return m_transform.GetWorldScale();
}

const OvMaths::FMatrix4 & OvCore::ECS::Components::CTransform::GetLocalMatrix() const
{
	return m_transform.GetLocalMatrix();
}

const OvMaths::FMatrix4 & OvCore::ECS::Components::CTransform::GetWorldMatrix() const
{
	return m_transform.GetWorldMatrix();
}

OvMaths::FTransform & OvCore::ECS::Components::CTransform::GetFTransform()
{
	return m_transform;
}

OvMaths::FVector3 OvCore::ECS::Components::CTransform::GetWorldForward() const
{
	return m_transform.GetWorldForward();
}

OvMaths::FVector3 OvCore::ECS::Components::CTransform::GetWorldUp() const
{
	return m_transform.GetWorldUp();
}

OvMaths::FVector3 OvCore::ECS::Components::CTransform::GetWorldRight() const
{
	return m_transform.GetWorldRight();
}

OvMaths::FVector3 OvCore::ECS::Components::CTransform::GetLocalForward() const
{
	return m_transform.GetLocalForward();
}

OvMaths::FVector3 OvCore::ECS::Components::CTransform::GetLocalUp() const
{
	return m_transform.GetLocalUp();
}

OvMaths::FVector3 OvCore::ECS::Components::CTransform::GetLocalRight() const
{
	return m_transform.GetLocalRight();
}

OvCore::ECS::Components::CTransform::EUIAnchorPreset OvCore::ECS::Components::CTransform::ToUIAnchorPreset(int p_value)
{
	return OvCore::ECS::Components::UI::UITransformResolver::ToAnchorPreset(p_value);
}

bool OvCore::ECS::Components::CTransform::IsHorizontalUIPositionEditable(EUIAnchorPreset p_anchorPreset)
{
	return OvCore::ECS::Components::UI::UITransformResolver::IsHorizontalPositionEditable(p_anchorPreset);
}

bool OvCore::ECS::Components::CTransform::IsVerticalUIPositionEditable(EUIAnchorPreset p_anchorPreset)
{
	return OvCore::ECS::Components::UI::UITransformResolver::IsVerticalPositionEditable(p_anchorPreset);
}

OvCore::ECS::Components::CTransform::UIData& OvCore::ECS::Components::CTransform::GetOrCreateUIData()
{
	if (!m_uiData)
	{
		m_uiData = UIData{};
	}

	return m_uiData.value();
}

const OvCore::ECS::Components::CTransform::UIData& OvCore::ECS::Components::CTransform::GetUIDataOrDefault() const
{
	static const UIData kDefaultUIData{};
	return m_uiData ? m_uiData.value() : kDefaultUIData;
}

void OvCore::ECS::Components::CTransform::EnableUIData()
{
	GetOrCreateUIData();
}

void OvCore::ECS::Components::CTransform::DisableUIData()
{
	m_uiData.reset();
}

bool OvCore::ECS::Components::CTransform::HasUIData() const
{
	return m_uiData.has_value();
}

bool OvCore::ECS::Components::CTransform::HasActiveUIData() const
{
	return OvCore::ECS::Components::UI::UITransformResolver::HasActiveUIData(owner);
}

const std::optional<OvCore::ECS::Components::CTransform::UIData>& OvCore::ECS::Components::CTransform::GetUIData() const
{
	return m_uiData;
}

void OvCore::ECS::Components::CTransform::SetUIPosition(const OvMaths::FVector2& p_position)
{
	auto& uiData = GetOrCreateUIData();
	uiData.position.x = KeepFinite(p_position.x, uiData.position.x);
	uiData.position.y = KeepFinite(p_position.y, uiData.position.y);
}

const OvMaths::FVector2& OvCore::ECS::Components::CTransform::GetUIPosition() const
{
	return GetUIDataOrDefault().position;
}

void OvCore::ECS::Components::CTransform::SetUIRotation(float p_rotation)
{
	const auto rotation = KeepFinite(p_rotation, GetUIRotation());
	auto eulerRotation = OvMaths::FQuaternion::EulerAngles(GetLocalRotation());
	eulerRotation.z = rotation;
	SetLocalRotation(OvMaths::FQuaternion(eulerRotation));
}

float OvCore::ECS::Components::CTransform::GetUIRotation() const
{
	const auto eulerRotation = OvMaths::FQuaternion::EulerAngles(GetLocalRotation());
	return KeepFinite(eulerRotation.z, 0.0f);
}

void OvCore::ECS::Components::CTransform::SetUIScale(const OvMaths::FVector2& p_scale)
{
	auto localScale = GetLocalScale();
	localScale.x = ClampScaleAxis(p_scale.x, localScale.x);
	localScale.y = ClampScaleAxis(p_scale.y, localScale.y);
	SetLocalScale(localScale);
}

OvMaths::FVector2 OvCore::ECS::Components::CTransform::GetUIScale() const
{
	const auto& localScale = GetLocalScale();
	return {
		ClampScaleAxis(localScale.x, kMinimumScale),
		ClampScaleAxis(localScale.y, kMinimumScale)
	};
}

void OvCore::ECS::Components::CTransform::SetUISize(const OvMaths::FVector2& p_size)
{
	auto& uiData = GetOrCreateUIData();
	uiData.size.x = std::isfinite(p_size.x) ? std::max(p_size.x, kMinimumSize) : uiData.size.x;
	uiData.size.y = std::isfinite(p_size.y) ? std::max(p_size.y, kMinimumSize) : uiData.size.y;
}

const OvMaths::FVector2& OvCore::ECS::Components::CTransform::GetUISize() const
{
	return GetUIDataOrDefault().size;
}

void OvCore::ECS::Components::CTransform::SetUIPivot(const OvMaths::FVector2& p_pivot)
{
	auto& uiData = GetOrCreateUIData();
	uiData.pivot.x = ClampPivotAxis(p_pivot.x, uiData.pivot.x);
	uiData.pivot.y = ClampPivotAxis(p_pivot.y, uiData.pivot.y);
}

const OvMaths::FVector2& OvCore::ECS::Components::CTransform::GetUIPivot() const
{
	return GetUIDataOrDefault().pivot;
}

void OvCore::ECS::Components::CTransform::SetUIAnchorPreset(EUIAnchorPreset p_anchorPreset)
{
	GetOrCreateUIData().anchorPreset = ToUIAnchorPreset(static_cast<int>(p_anchorPreset));
}

OvCore::ECS::Components::CTransform::EUIAnchorPreset OvCore::ECS::Components::CTransform::GetUIAnchorPreset() const
{
	return GetUIDataOrDefault().anchorPreset;
}

bool OvCore::ECS::Components::CTransform::IsHorizontalUIPositionEditable() const
{
	return IsHorizontalUIPositionEditable(GetUIAnchorPreset());
}

bool OvCore::ECS::Components::CTransform::IsVerticalUIPositionEditable() const
{
	return IsVerticalUIPositionEditable(GetUIAnchorPreset());
}

void OvCore::ECS::Components::CTransform::OnSerialize(tinyxml2::XMLDocument& p_doc, tinyxml2::XMLNode* p_node)
{
	OvCore::Helpers::Serializer::SerializeVec3(p_doc, p_node, "position", GetLocalPosition());
	OvCore::Helpers::Serializer::SerializeQuat(p_doc, p_node, "rotation", GetLocalRotation());
	OvCore::Helpers::Serializer::SerializeVec3(p_doc, p_node, "scale", GetLocalScale());

	if (HasUIData())
	{
		OvCore::Helpers::Serializer::SerializeBoolean(p_doc, p_node, "ui_enabled", true);
		OvCore::Helpers::Serializer::SerializeVec2(p_doc, p_node, "ui_position", GetUIPosition());
		OvCore::Helpers::Serializer::SerializeVec2(p_doc, p_node, "ui_size", GetUISize());
		OvCore::Helpers::Serializer::SerializeVec2(p_doc, p_node, "ui_pivot", GetUIPivot());
		OvCore::Helpers::Serializer::SerializeInt(p_doc, p_node, "ui_anchor_preset", static_cast<int>(GetUIAnchorPreset()));
	}
}

void OvCore::ECS::Components::CTransform::OnDeserialize(tinyxml2::XMLDocument & p_doc, tinyxml2::XMLNode * p_node)
{
	m_transform.GenerateMatricesLocal
	(
		OvCore::Helpers::Serializer::DeserializeVec3(p_doc, p_node, "position"),
		OvCore::Helpers::Serializer::DeserializeQuat(p_doc, p_node, "rotation"),
		OvCore::Helpers::Serializer::DeserializeVec3(p_doc, p_node, "scale")
	);

	bool uiEnabled = true;
	if (p_node->FirstChildElement("ui_enabled"))
	{
		OvCore::Helpers::Serializer::DeserializeBoolean(p_doc, p_node, "ui_enabled", uiEnabled);
		if (uiEnabled)
		{
			EnableUIData();
		}
		else
		{
			DisableUIData();
		}
	}

	if (uiEnabled && p_node->FirstChildElement("ui_position"))
	{
		auto position = GetUIPosition();
		OvCore::Helpers::Serializer::DeserializeVec2(p_doc, p_node, "ui_position", position);
		SetUIPosition(position);
	}

	if (uiEnabled && p_node->FirstChildElement("ui_size"))
	{
		auto size = GetUISize();
		OvCore::Helpers::Serializer::DeserializeVec2(p_doc, p_node, "ui_size", size);
		SetUISize(size);
	}

	if (uiEnabled && p_node->FirstChildElement("ui_pivot"))
	{
		auto pivot = GetUIPivot();
		OvCore::Helpers::Serializer::DeserializeVec2(p_doc, p_node, "ui_pivot", pivot);
		SetUIPivot(pivot);
	}

	if (uiEnabled && p_node->FirstChildElement("ui_anchor_preset"))
	{
		auto anchorPreset = static_cast<int>(GetUIAnchorPreset());
		OvCore::Helpers::Serializer::DeserializeInt(p_doc, p_node, "ui_anchor_preset", anchorPreset);
		SetUIAnchorPreset(ToUIAnchorPreset(anchorPreset));
	}
}

void OvCore::ECS::Components::CTransform::OnInspector(OvUI::Internal::WidgetContainer& p_root)
{
	if (HasActiveUIData())
	{
		OvCore::Helpers::GUIDrawer::CreateTitle(p_root, "Anchored Position (px)");
		auto& anchoredPosition = p_root.CreateWidget<OvUI::Widgets::Drags::DragMultipleScalars<float, 2>>(
			OvCore::Helpers::GUIDrawer::GetDataType<float>(),
			OvCore::Helpers::GUIDrawer::_MIN_FLOAT,
			OvCore::Helpers::GUIDrawer::_MAX_FLOAT,
			0.0f,
			1.0f,
			"",
			OvCore::Helpers::GUIDrawer::GetFormat<float>()
		);
		auto& anchoredPositionDispatcher = anchoredPosition.AddPlugin<OvUI::Plugins::DataDispatcher<std::array<float, 2>>>();
		anchoredPositionDispatcher.RegisterGatherer([this]()
		{
			const auto value = GetUIPosition();
			return std::array<float, 2>{ value.x, value.y };
		});
		anchoredPositionDispatcher.RegisterProvider([this](std::array<float, 2> p_value)
		{
			auto position = GetUIPosition();
			const auto anchorPreset = GetUIAnchorPreset();

			if (IsHorizontalUIPositionEditable(anchorPreset))
			{
				position.x = p_value[0];
			}

			if (IsVerticalUIPositionEditable(anchorPreset))
			{
				position.y = p_value[1];
			}

			SetUIPosition(position);
		});

		const auto updateAnchoredPositionEditability = [this, anchoredPositionWidget = &anchoredPosition]()
		{
			const auto anchorPreset = GetUIAnchorPreset();
			const bool isHorizontalEditable = IsHorizontalUIPositionEditable(anchorPreset);
			const bool isVerticalEditable = IsVerticalUIPositionEditable(anchorPreset);

			anchoredPositionWidget->disabled = OvCore::ECS::Components::UI::UITransformResolver::IsDrivenByLayout(owner) || (!isHorizontalEditable && !isVerticalEditable);
		};
		updateAnchoredPositionEditability();

		OvCore::Helpers::GUIDrawer::DrawVec2(
			p_root,
			"Size",
			[this]() { return GetUISize(); },
			[this](OvMaths::FVector2 p_value) { SetUISize(p_value); },
			1.0f,
			kMinimumSize
		);

		OvCore::Helpers::GUIDrawer::DrawVec2(
			p_root,
			"Pivot",
			[this]() { return GetUIPivot(); },
			[this](OvMaths::FVector2 p_value) { SetUIPivot(p_value); },
			0.01f,
			kMinimumPivot,
			kMaximumPivot
		);

		OvCore::Helpers::GUIDrawer::DrawScalar<float>(
			p_root,
			"Rotation",
			[this]() { return GetUIRotation(); },
			[this](float p_value) { SetUIRotation(p_value); },
			0.05f,
			OvCore::Helpers::GUIDrawer::_MIN_FLOAT,
			OvCore::Helpers::GUIDrawer::_MAX_FLOAT
		);

		OvCore::Helpers::GUIDrawer::DrawVec2(
			p_root,
			"Scale",
			[this]() { return GetUIScale(); },
			[this](OvMaths::FVector2 p_value) { SetUIScale(p_value); },
			0.05f,
			kMinimumScale
		);

		OvCore::Helpers::GUIDrawer::CreateTitle(p_root, "Anchor Preset");
		auto& anchorPreset = p_root.CreateWidget<OvUI::Widgets::Selection::ComboBox>(static_cast<int>(GetUIAnchorPreset()));
		anchorPreset.disabled = OvCore::ECS::Components::UI::UITransformResolver::IsDrivenByLayout(owner);
		anchorPreset.choices.emplace(static_cast<int>(EUIAnchorPreset::TOP_LEFT), "Top Left");
		anchorPreset.choices.emplace(static_cast<int>(EUIAnchorPreset::TOP_CENTER), "Top Center");
		anchorPreset.choices.emplace(static_cast<int>(EUIAnchorPreset::TOP_RIGHT), "Top Right");
		anchorPreset.choices.emplace(static_cast<int>(EUIAnchorPreset::MIDDLE_LEFT), "Middle Left");
		anchorPreset.choices.emplace(static_cast<int>(EUIAnchorPreset::CENTER), "Center");
		anchorPreset.choices.emplace(static_cast<int>(EUIAnchorPreset::MIDDLE_RIGHT), "Middle Right");
		anchorPreset.choices.emplace(static_cast<int>(EUIAnchorPreset::BOTTOM_LEFT), "Bottom Left");
		anchorPreset.choices.emplace(static_cast<int>(EUIAnchorPreset::BOTTOM_CENTER), "Bottom Center");
		anchorPreset.choices.emplace(static_cast<int>(EUIAnchorPreset::BOTTOM_RIGHT), "Bottom Right");
		anchorPreset.choices.emplace(static_cast<int>(EUIAnchorPreset::HORIZONTAL_STRETCH_TOP), "Horizontal Stretch Top");
		anchorPreset.choices.emplace(static_cast<int>(EUIAnchorPreset::HORIZONTAL_STRETCH_MIDDLE), "Horizontal Stretch Middle");
		anchorPreset.choices.emplace(static_cast<int>(EUIAnchorPreset::HORIZONTAL_STRETCH_BOTTOM), "Horizontal Stretch Bottom");
		anchorPreset.choices.emplace(static_cast<int>(EUIAnchorPreset::VERTICAL_STRETCH_LEFT), "Vertical Stretch Left");
		anchorPreset.choices.emplace(static_cast<int>(EUIAnchorPreset::VERTICAL_STRETCH_CENTER), "Vertical Stretch Center");
		anchorPreset.choices.emplace(static_cast<int>(EUIAnchorPreset::VERTICAL_STRETCH_RIGHT), "Vertical Stretch Right");
		anchorPreset.choices.emplace(static_cast<int>(EUIAnchorPreset::STRETCH_BOTH), "Stretch Both");

		anchorPreset.ValueChangedEvent += [this, updateAnchoredPositionEditability](int p_choice)
		{
			SetUIAnchorPreset(ToUIAnchorPreset(p_choice));
			updateAnchoredPositionEditability();
		};
	}
	else
	{
		auto getRotation = [this]
		{
			return OvMaths::FQuaternion::EulerAngles(GetLocalRotation());
		};

		auto setRotation = [this](OvMaths::FVector3 result)
		{
			SetLocalRotation(OvMaths::FQuaternion(result));
		};

		OvCore::Helpers::GUIDrawer::DrawVec3(p_root, "Position", std::bind(&CTransform::GetLocalPosition, this), std::bind(&CTransform::SetLocalPosition, this, std::placeholders::_1), 0.05f);
		OvCore::Helpers::GUIDrawer::DrawVec3(p_root, "Rotation", getRotation, setRotation, 0.05f);
		OvCore::Helpers::GUIDrawer::DrawVec3(p_root, "Scale", std::bind(&CTransform::GetLocalScale, this), std::bind(&CTransform::SetLocalScale, this, std::placeholders::_1), 0.05f, 0.0001f);
	}
}
