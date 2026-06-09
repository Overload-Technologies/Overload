/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include <memory>
#include <vector>

#include <OvCore/ECS/Components/UI/CLayoutGroup.h>
#include <OvMaths/FVector2.h>
#include <OvMaths/FVector4.h>

namespace OvCore::ECS { class Actor; }

namespace OvCore::ECS::Components::UI
{
	class ClayLayoutSolverContext
	{
	public:
		ClayLayoutSolverContext();
		~ClayLayoutSolverContext();

		ClayLayoutSolverContext(const ClayLayoutSolverContext&) = delete;
		ClayLayoutSolverContext& operator=(const ClayLayoutSolverContext&) = delete;
		ClayLayoutSolverContext(ClayLayoutSolverContext&&) noexcept;
		ClayLayoutSolverContext& operator=(ClayLayoutSolverContext&&) noexcept;

	private:
		friend class ClayLayoutSolver;

		struct Impl;
		std::unique_ptr<Impl> m_impl;
	};

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

	struct ClayLayoutMeasurement
	{
		ClayLayoutSettings settings;
		OvMaths::FVector2 preferredSize = OvMaths::FVector2::Zero;
		bool valid = false;
	};

	struct ClayLayoutSolution
	{
		ClayLayoutSettings settings;
		OvMaths::FVector2 preferredSize = OvMaths::FVector2::Zero;
		ClayLayoutResult result;
		bool valid = false;
	};

	class ClayLayoutSolver
	{
	public:
		static ClayLayoutMeasurement Measure(
			ClayLayoutSolverContext& p_context,
			const ClayLayoutSettings& p_settings,
			const std::vector<ClayLayoutChildInput>& p_children
		);

		static ClayLayoutSolution SolveLayout(
			ClayLayoutSolverContext& p_context,
			const ClayLayoutMeasurement& p_measurement,
			const std::vector<ClayLayoutChildInput>& p_children
		);

		static ClayLayoutResult Postprocess(const ClayLayoutSolution& p_solution);

		static ClayLayoutResult Solve(
			ClayLayoutSolverContext& p_context,
			const ClayLayoutSettings& p_settings,
			const std::vector<ClayLayoutChildInput>& p_children
		);
	};
}
