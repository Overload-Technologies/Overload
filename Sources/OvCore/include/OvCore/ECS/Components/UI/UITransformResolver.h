/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include <OvCore/ECS/Components/CTransform.h>
#include <OvMaths/FMatrix4.h>
#include <OvMaths/FVector2.h>

namespace OvCore::ECS { class Actor; }

namespace OvCore::ECS::Components::UI::UITransformResolver
{
	struct LayoutData
	{
		OvMaths::FVector2 offset = OvMaths::FVector2::Zero;
		OvMaths::FVector2 directSize = OvMaths::FVector2::Zero;
		bool hasDirectWidth = false;
		bool hasDirectHeight = false;
	};

	CTransform::EUIAnchorPreset ToAnchorPreset(int p_value);
	OvMaths::FVector2 GetAnchorRatio(CTransform::EUIAnchorPreset p_anchorPreset);
	bool IsHorizontalPositionEditable(CTransform::EUIAnchorPreset p_anchorPreset);
	bool IsVerticalPositionEditable(CTransform::EUIAnchorPreset p_anchorPreset);

	ECS::Actor* FindCanvasOwner(ECS::Actor& p_owner);
	const ECS::Actor* FindCanvasOwner(const ECS::Actor& p_owner);
	const ECS::Actor* FindActiveCanvasOwner(const ECS::Actor& p_owner);
	bool HasActiveUIData(const ECS::Actor& p_owner);

	bool IsDrivenByLayout(const ECS::Actor& p_owner);
	LayoutData ResolveLayoutData(const ECS::Actor& p_owner);

	OvMaths::FVector2 GetEffectiveSize(
		const CTransform& p_transform,
		const OvMaths::FVector2& p_elementSize
	);

	OvMaths::FVector2 GetAnchoredPosition(
		const CTransform& p_transform,
		const OvMaths::FVector2& p_canvasSize,
		const OvMaths::FVector2& p_layoutOffset
	);

	OvMaths::FMatrix4 GetMatrix(
		const CTransform& p_transform,
		const OvMaths::FVector2& p_canvasSize,
		const OvMaths::FVector2& p_layoutOffset,
		const OvMaths::FVector2& p_elementSize
	);

	OvMaths::FMatrix4 GetMatrixWithEffectiveSize(
		const CTransform& p_transform,
		const OvMaths::FVector2& p_canvasSize,
		const OvMaths::FVector2& p_layoutOffset,
		const OvMaths::FVector2& p_effectiveSize
	);
}
