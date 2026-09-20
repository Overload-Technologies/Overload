/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include <vector>

#include <OvCore/ECS/Components/UI/CLayoutGroup.h>
#include <OvMaths/FVector2.h>
#include <OvMaths/FVector4.h>

namespace OvCore::ECS { class Actor; }

namespace OvCore::ECS::Components::UI
{
	/**
	* Settings driving the resolution of a layout group
	*/
	struct LayoutSettings
	{
		CLayoutGroup::EDirection direction = CLayoutGroup::EDirection::HORIZONTAL;
		float spacing = 0.0f;
		OvMaths::FVector4 padding = OvMaths::FVector4::Zero;
		CLayoutGroup::EHorizontalAlignment horizontalAlignment = CLayoutGroup::EHorizontalAlignment::CENTER;
		CLayoutGroup::EVerticalAlignment verticalAlignment = CLayoutGroup::EVerticalAlignment::CENTER;
		bool controlChildrenWidth = false;
		bool controlChildrenHeight = false;
		bool forceExpandWidth = false;
		bool forceExpandHeight = false;
		OvMaths::FVector2 containerSize = OvMaths::FVector2::Zero;
	};

	/**
	* Child submitted to a layout resolution, with the size it would take on its own
	*/
	struct LayoutChildInput
	{
		ECS::Actor* actor = nullptr;
		OvMaths::FVector2 preferredSize = OvMaths::FVector2::Zero;
	};

	/**
	* Placement resolved for a layout child, expressed in the centered layout coordinate system
	*/
	struct LayoutChildResult
	{
		ECS::Actor* actor = nullptr;
		OvMaths::FVector2 offset = OvMaths::FVector2::Zero;
		OvMaths::FVector2 size = OvMaths::FVector2::Zero;
		bool valid = false;
	};

	/**
	* Container size and child placements produced by a layout resolution
	*/
	struct LayoutResult
	{
		OvMaths::FVector2 size = OvMaths::FVector2::Zero;
		std::vector<LayoutChildResult> children;
	};

	/**
	* Arranges layout children along an axis, distributing the leftover space when requested
	*/
	class LayoutSolver
	{
	public:
		/**
		* Resolve the container size and the placement of every child
		* @param p_settings
		* @param p_children
		*/
		static LayoutResult Solve(
			const LayoutSettings& p_settings,
			const std::vector<LayoutChildInput>& p_children
		);
	};
}
