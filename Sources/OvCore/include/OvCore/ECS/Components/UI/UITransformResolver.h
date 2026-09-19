/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include <OvCore/ECS/Components/CTransform.h>
#include <OvMaths/FVector2.h>

namespace OvCore::ECS { class Actor; }

namespace OvCore::ECS::Components::UI::UITransformResolver
{
	/**
	* Convert a serialized value to an anchor preset, falling back to CENTER when out of range
	* @param p_value
	*/
	CTransform::EUIAnchorPreset ToAnchorPreset(int p_value);

	/**
	* Return the normalized offset applied to the parent size for the given anchor preset
	* @param p_anchorPreset
	*/
	OvMaths::FVector2 GetAnchorRatio(CTransform::EUIAnchorPreset p_anchorPreset);

	/**
	* Check if the given anchor preset stretches the element horizontally
	* @param p_anchorPreset
	*/
	bool IsHorizontalStretch(CTransform::EUIAnchorPreset p_anchorPreset);

	/**
	* Check if the given anchor preset stretches the element vertically
	* @param p_anchorPreset
	*/
	bool IsVerticalStretch(CTransform::EUIAnchorPreset p_anchorPreset);

	/**
	* Check if the horizontal anchored position is editable for the given anchor preset
	* @param p_anchorPreset
	*/
	bool IsHorizontalPositionEditable(CTransform::EUIAnchorPreset p_anchorPreset);

	/**
	* Check if the vertical anchored position is editable for the given anchor preset
	* @param p_anchorPreset
	*/
	bool IsVerticalPositionEditable(CTransform::EUIAnchorPreset p_anchorPreset);

	/**
	* Return the closest actor holding a canvas, the given actor included, or nullptr
	* @param p_owner
	*/
	ECS::Actor* FindCanvasOwner(ECS::Actor& p_owner);

	/**
	* Return the closest actor holding a canvas, the given actor included, or nullptr
	* @param p_owner
	*/
	const ECS::Actor* FindCanvasOwner(const ECS::Actor& p_owner);

	/**
	* Return the closest ancestor holding a canvas, excluding the given actor, or nullptr
	* @param p_owner
	*/
	const ECS::Actor* FindActiveCanvasOwner(const ECS::Actor& p_owner);

	/**
	* Check if the given actor has a canvas ancestor, meaning its user interface data is in effect
	* @param p_owner
	*/
	bool HasActiveUIData(const ECS::Actor& p_owner);

	/**
	* Check if the given actor is a direct child of a layout group
	* @param p_owner
	*/
	bool IsDrivenByLayout(const ECS::Actor& p_owner);

	/**
	* Return the transform size, falling back to the element size on any axis left to zero
	* @param p_transform
	* @param p_elementSize
	*/
	OvMaths::FVector2 GetEffectiveSize(
		const CTransform& p_transform,
		const OvMaths::FVector2& p_elementSize
	);
}
