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
	constexpr float kDegreesToRadians = 3.14159265359f / 180.0f;

	float KeepFinite(float p_value, float p_fallback)
	{
		return std::isfinite(p_value) ? p_value : p_fallback;
	}

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
	using EUIAnchorPreset = OvCore::ECS::Components::CTransform::EUIAnchorPreset;

	switch (p_anchorPreset)
	{
	case EUIAnchorPreset::HORIZONTAL_STRETCH_TOP:
	case EUIAnchorPreset::HORIZONTAL_STRETCH_MIDDLE:
	case EUIAnchorPreset::HORIZONTAL_STRETCH_BOTTOM:
	case EUIAnchorPreset::STRETCH_BOTH:
		return false;
	default:
		return true;
	}
}

bool OvCore::ECS::Components::UI::UITransformResolver::IsVerticalPositionEditable(OvCore::ECS::Components::CTransform::EUIAnchorPreset p_anchorPreset)
{
	using EUIAnchorPreset = OvCore::ECS::Components::CTransform::EUIAnchorPreset;

	switch (p_anchorPreset)
	{
	case EUIAnchorPreset::VERTICAL_STRETCH_LEFT:
	case EUIAnchorPreset::VERTICAL_STRETCH_CENTER:
	case EUIAnchorPreset::VERTICAL_STRETCH_RIGHT:
	case EUIAnchorPreset::STRETCH_BOTH:
		return false;
	default:
		return true;
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

OvCore::ECS::Components::UI::UITransformResolver::LayoutData OvCore::ECS::Components::UI::UITransformResolver::ResolveLayoutData(const ECS::Actor& p_owner)
{
	LayoutData result;
	const auto* child = &p_owner;

	while (const auto* parent = child->GetParent())
	{
		if (const auto* layout = parent->GetComponent<OvCore::ECS::Components::UI::CLayoutGroup>())
		{
			if (const auto childLayout = layout->GetChildLayout(*child); childLayout && childLayout->valid)
			{
				result.offset += childLayout->offset;

				if (child == &p_owner)
				{
					if (childLayout->size.x > 0.0f)
					{
						result.directSize.x = childLayout->size.x;
						result.hasDirectWidth = true;
					}

					if (childLayout->size.y > 0.0f)
					{
						result.directSize.y = childLayout->size.y;
						result.hasDirectHeight = true;
					}
				}
			}
		}

		child = parent;
	}

	return result;
}

OvMaths::FVector2 OvCore::ECS::Components::UI::UITransformResolver::GetEffectiveSize(
	const OvCore::ECS::Components::CTransform& p_transform,
	const OvMaths::FVector2& p_elementSize
)
{
	const auto& size = p_transform.GetUISize();
	return {
		size.x > 0.0f ? size.x : std::max(p_elementSize.x, 0.0f),
		size.y > 0.0f ? size.y : std::max(p_elementSize.y, 0.0f)
	};
}

OvMaths::FVector2 OvCore::ECS::Components::UI::UITransformResolver::GetAnchoredPosition(
	const OvCore::ECS::Components::CTransform& p_transform,
	const OvMaths::FVector2& p_canvasSize,
	const OvMaths::FVector2& p_layoutOffset
)
{
	if (IsDrivenByLayout(p_transform.owner))
	{
		const auto* parent = p_transform.owner.GetParent();
		if (!parent)
		{
			return p_layoutOffset;
		}

		if (const auto* parentLayout = parent->GetComponent<OvCore::ECS::Components::UI::CLayoutGroup>())
		{
			const auto childLayoutOffset = parentLayout->GetChildOffset(p_transform.owner);
			const auto parentLayoutOffset = p_layoutOffset - childLayoutOffset;
			const auto parentAnchoredPosition = GetAnchoredPosition(parent->transform, p_canvasSize, parentLayoutOffset);

			return parentAnchoredPosition + childLayoutOffset;
		}

		return p_layoutOffset;
	}

	const auto anchorRatio = GetAnchorRatio(p_transform.GetUIAnchorPreset());
	const OvMaths::FVector2 anchorOffset = {
		KeepFinite(p_canvasSize.x, 0.0f) * anchorRatio.x,
		KeepFinite(p_canvasSize.y, 0.0f) * anchorRatio.y
	};
	const float positionX = IsHorizontalPositionEditable(p_transform.GetUIAnchorPreset()) ? p_transform.GetUIPosition().x : 0.0f;
	const float positionY = IsVerticalPositionEditable(p_transform.GetUIAnchorPreset()) ? p_transform.GetUIPosition().y : 0.0f;

	return {
		anchorOffset.x + p_layoutOffset.x + positionX,
		anchorOffset.y + p_layoutOffset.y + positionY
	};
}

OvMaths::FMatrix4 OvCore::ECS::Components::UI::UITransformResolver::GetMatrix(
	const OvCore::ECS::Components::CTransform& p_transform,
	const OvMaths::FVector2& p_canvasSize,
	const OvMaths::FVector2& p_layoutOffset,
	const OvMaths::FVector2& p_elementSize
)
{
	return GetMatrixWithEffectiveSize(
		p_transform,
		p_canvasSize,
		p_layoutOffset,
		GetEffectiveSize(p_transform, p_elementSize)
	);
}

OvMaths::FMatrix4 OvCore::ECS::Components::UI::UITransformResolver::GetMatrixWithEffectiveSize(
	const OvCore::ECS::Components::CTransform& p_transform,
	const OvMaths::FVector2& p_canvasSize,
	const OvMaths::FVector2& p_layoutOffset,
	const OvMaths::FVector2& p_effectiveSize
)
{
	const auto position = GetAnchoredPosition(p_transform, p_canvasSize, p_layoutOffset);
	const auto scale = p_transform.GetUIScale();
	const auto halfSize = p_effectiveSize * 0.5f;
	const auto& pivot = p_transform.GetUIPivot();
	const OvMaths::FVector2 pivotOffset = {
		-pivot.x * halfSize.x,
		pivot.y * halfSize.y
	};

	return
		OvMaths::FMatrix4::Translation({ position.x, position.y, 0.0f }) *
		OvMaths::FMatrix4::RotationOnAxisZ(p_transform.GetUIRotation() * kDegreesToRadians) *
		OvMaths::FMatrix4::Scaling({ scale.x, scale.y, 1.0f }) *
		OvMaths::FMatrix4::Translation({ pivotOffset.x, pivotOffset.y, 0.0f });
}
