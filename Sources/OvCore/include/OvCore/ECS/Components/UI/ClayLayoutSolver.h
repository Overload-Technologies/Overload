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
	struct ClayLayoutSettings
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
		OvMaths::FVector2 pivot = OvMaths::FVector2::Zero;
	};

	struct ClayLayoutChildInput
	{
		ECS::Actor* actor = nullptr;
		OvMaths::FVector2 preferredSize = OvMaths::FVector2::Zero;
	};

	struct ClayLayoutChildResult
	{
		ECS::Actor* actor = nullptr;
		OvMaths::FVector2 offset = OvMaths::FVector2::Zero;
		OvMaths::FVector2 size = OvMaths::FVector2::Zero;
		bool valid = false;
	};

	struct ClayLayoutResult
	{
		OvMaths::FVector2 size = OvMaths::FVector2::Zero;
		std::vector<ClayLayoutChildResult> children;
	};

	class ClayLayoutSolver
	{
	public:
		static ClayLayoutResult Solve(
			const ClayLayoutSettings& p_settings,
			const std::vector<ClayLayoutChildInput>& p_children
		);
	};
}
