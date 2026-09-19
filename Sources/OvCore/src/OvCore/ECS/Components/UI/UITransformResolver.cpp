/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include <algorithm>
#include <cmath>

#include <OvCore/ECS/Actor.h>
#include <OvCore/ECS/Components/UI/CCanvas.h>
#include <OvCore/ECS/Components/UI/CLayoutGroup.h>
#include <OvCore/ECS/Components/UI/UITransformResolver.h>

namespace
{
	OvCore::ECS::Actor* FindCanvasOwnerInHierarchy(OvCore::ECS::Actor& p_owner, bool p_includeSelf)
	{
		auto* current = p_includeSelf ? &p_owner : p_owner.GetParent();

		while (current)
		{
			if (current->GetComponent<OvCore::ECS::Components::UI::CCanvas>())
			{
				return current;
			}

			current = current->GetParent();
		}

		return nullptr;
	}

	const OvCore::ECS::Actor* FindCanvasOwnerInHierarchy(const OvCore::ECS::Actor& p_owner, bool p_includeSelf)
	{
		const auto* current = p_includeSelf ? &p_owner : p_owner.GetParent();

		while (current)
		{
			if (current->GetComponent<OvCore::ECS::Components::UI::CCanvas>())
			{
				return current;
			}

			current = current->GetParent();
		}

		return nullptr;
	}
}

OvCore::ECS::Components::CTransform::EUIAnchorPreset OvCore::ECS::Components::UI::UITransformResolver::ToAnchorPreset(int p_value)
{
	using EUIAnchorPreset = OvCore::ECS::Components::CTransform::EUIAnchorPreset;

	switch (p_value)
	{
	case static_cast<int>(EUIAnchorPreset::TOP_LEFT):
		return EUIAnchorPreset::TOP_LEFT;
	case static_cast<int>(EUIAnchorPreset::TOP_CENTER):
		return EUIAnchorPreset::TOP_CENTER;
	case static_cast<int>(EUIAnchorPreset::TOP_RIGHT):
		return EUIAnchorPreset::TOP_RIGHT;
	case static_cast<int>(EUIAnchorPreset::MIDDLE_LEFT):
		return EUIAnchorPreset::MIDDLE_LEFT;
	case static_cast<int>(EUIAnchorPreset::MIDDLE_RIGHT):
		return EUIAnchorPreset::MIDDLE_RIGHT;
	case static_cast<int>(EUIAnchorPreset::BOTTOM_LEFT):
		return EUIAnchorPreset::BOTTOM_LEFT;
	case static_cast<int>(EUIAnchorPreset::BOTTOM_CENTER):
		return EUIAnchorPreset::BOTTOM_CENTER;
	case static_cast<int>(EUIAnchorPreset::BOTTOM_RIGHT):
		return EUIAnchorPreset::BOTTOM_RIGHT;
	case static_cast<int>(EUIAnchorPreset::HORIZONTAL_STRETCH_TOP):
		return EUIAnchorPreset::HORIZONTAL_STRETCH_TOP;
	case static_cast<int>(EUIAnchorPreset::HORIZONTAL_STRETCH_MIDDLE):
		return EUIAnchorPreset::HORIZONTAL_STRETCH_MIDDLE;
	case static_cast<int>(EUIAnchorPreset::HORIZONTAL_STRETCH_BOTTOM):
		return EUIAnchorPreset::HORIZONTAL_STRETCH_BOTTOM;
	case static_cast<int>(EUIAnchorPreset::VERTICAL_STRETCH_LEFT):
		return EUIAnchorPreset::VERTICAL_STRETCH_LEFT;
	case static_cast<int>(EUIAnchorPreset::VERTICAL_STRETCH_CENTER):
		return EUIAnchorPreset::VERTICAL_STRETCH_CENTER;
	case static_cast<int>(EUIAnchorPreset::VERTICAL_STRETCH_RIGHT):
		return EUIAnchorPreset::VERTICAL_STRETCH_RIGHT;
	case static_cast<int>(EUIAnchorPreset::STRETCH_BOTH):
		return EUIAnchorPreset::STRETCH_BOTH;
	case static_cast<int>(EUIAnchorPreset::CENTER):
	default:
		return EUIAnchorPreset::CENTER;
	}
}

OvMaths::FVector2 OvCore::ECS::Components::UI::UITransformResolver::GetAnchorRatio(OvCore::ECS::Components::CTransform::EUIAnchorPreset p_anchorPreset)
{
	using EUIAnchorPreset = OvCore::ECS::Components::CTransform::EUIAnchorPreset;

	switch (p_anchorPreset)
	{
	case EUIAnchorPreset::TOP_LEFT:
		return { -0.5f, 0.5f };
	case EUIAnchorPreset::TOP_CENTER:
		return { 0.0f, 0.5f };
	case EUIAnchorPreset::TOP_RIGHT:
		return { 0.5f, 0.5f };
	case EUIAnchorPreset::MIDDLE_LEFT:
		return { -0.5f, 0.0f };
	case EUIAnchorPreset::MIDDLE_RIGHT:
		return { 0.5f, 0.0f };
	case EUIAnchorPreset::BOTTOM_LEFT:
		return { -0.5f, -0.5f };
	case EUIAnchorPreset::BOTTOM_CENTER:
		return { 0.0f, -0.5f };
	case EUIAnchorPreset::BOTTOM_RIGHT:
		return { 0.5f, -0.5f };
	case EUIAnchorPreset::HORIZONTAL_STRETCH_TOP:
		return { 0.0f, 0.5f };
	case EUIAnchorPreset::HORIZONTAL_STRETCH_MIDDLE:
		return { 0.0f, 0.0f };
	case EUIAnchorPreset::HORIZONTAL_STRETCH_BOTTOM:
		return { 0.0f, -0.5f };
	case EUIAnchorPreset::VERTICAL_STRETCH_LEFT:
		return { -0.5f, 0.0f };
	case EUIAnchorPreset::VERTICAL_STRETCH_CENTER:
		return { 0.0f, 0.0f };
	case EUIAnchorPreset::VERTICAL_STRETCH_RIGHT:
		return { 0.5f, 0.0f };
	case EUIAnchorPreset::STRETCH_BOTH:
		return { 0.0f, 0.0f };
	case EUIAnchorPreset::CENTER:
	default:
		return { 0.0f, 0.0f };
	}
}

bool OvCore::ECS::Components::UI::UITransformResolver::IsHorizontalPositionEditable(OvCore::ECS::Components::CTransform::EUIAnchorPreset p_anchorPreset)
{
	return !IsHorizontalStretch(p_anchorPreset);
}

bool OvCore::ECS::Components::UI::UITransformResolver::IsVerticalPositionEditable(OvCore::ECS::Components::CTransform::EUIAnchorPreset p_anchorPreset)
{
	return !IsVerticalStretch(p_anchorPreset);
}

bool OvCore::ECS::Components::UI::UITransformResolver::IsHorizontalStretch(OvCore::ECS::Components::CTransform::EUIAnchorPreset p_anchorPreset)
{
	using EUIAnchorPreset = OvCore::ECS::Components::CTransform::EUIAnchorPreset;

	switch (p_anchorPreset)
	{
	case EUIAnchorPreset::HORIZONTAL_STRETCH_TOP:
	case EUIAnchorPreset::HORIZONTAL_STRETCH_MIDDLE:
	case EUIAnchorPreset::HORIZONTAL_STRETCH_BOTTOM:
	case EUIAnchorPreset::STRETCH_BOTH:
		return true;
	default:
		return false;
	}
}

bool OvCore::ECS::Components::UI::UITransformResolver::IsVerticalStretch(OvCore::ECS::Components::CTransform::EUIAnchorPreset p_anchorPreset)
{
	using EUIAnchorPreset = OvCore::ECS::Components::CTransform::EUIAnchorPreset;

	switch (p_anchorPreset)
	{
	case EUIAnchorPreset::VERTICAL_STRETCH_LEFT:
	case EUIAnchorPreset::VERTICAL_STRETCH_CENTER:
	case EUIAnchorPreset::VERTICAL_STRETCH_RIGHT:
	case EUIAnchorPreset::STRETCH_BOTH:
		return true;
	default:
		return false;
	}
}

OvCore::ECS::Actor* OvCore::ECS::Components::UI::UITransformResolver::FindCanvasOwner(ECS::Actor& p_owner)
{
	return FindCanvasOwnerInHierarchy(p_owner, true);
}

const OvCore::ECS::Actor* OvCore::ECS::Components::UI::UITransformResolver::FindCanvasOwner(const ECS::Actor& p_owner)
{
	return FindCanvasOwnerInHierarchy(p_owner, true);
}

const OvCore::ECS::Actor* OvCore::ECS::Components::UI::UITransformResolver::FindActiveCanvasOwner(const ECS::Actor& p_owner)
{
	return FindCanvasOwnerInHierarchy(p_owner, false);
}

bool OvCore::ECS::Components::UI::UITransformResolver::HasActiveUIData(const ECS::Actor& p_owner)
{
	return FindActiveCanvasOwner(p_owner) != nullptr;
}

bool OvCore::ECS::Components::UI::UITransformResolver::IsDrivenByLayout(const ECS::Actor& p_owner)
{
	const auto* parent = p_owner.GetParent();
	return parent && parent->GetComponent<OvCore::ECS::Components::UI::CLayoutGroup>();
}

OvMaths::FVector2 OvCore::ECS::Components::UI::UITransformResolver::GetEffectiveSize(
	const OvCore::ECS::Components::CTransform& p_transform,
	const OvMaths::FVector2& p_elementSize
)
{
	const auto& size = p_transform.GetUISize();
	const auto fallbackWidth = std::isfinite(p_elementSize.x) ? std::max(p_elementSize.x, 0.0f) : 0.0f;
	const auto fallbackHeight = std::isfinite(p_elementSize.y) ? std::max(p_elementSize.y, 0.0f) : 0.0f;
	return {
		std::isfinite(size.x) && size.x > 0.0f ? size.x : fallbackWidth,
		std::isfinite(size.y) && size.y > 0.0f ? size.y : fallbackHeight
	};
}
