/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <memory>
#include <mutex>
#include <string>
#include <vector>

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable: 4244 4305)
#endif

#include <clay.h>

#ifdef _MSC_VER
#pragma warning(pop)
#endif

#include <OvCore/ECS/Actor.h>
#include <OvCore/ECS/Components/UI/ClayLayoutSolver.h>

namespace
{
	constexpr float kMinimumLayoutSize = 1.0f;
	constexpr float kMaximumLayoutSize = 1'000'000.0f;
	constexpr float kMinimumSpacing = 0.0f;
	constexpr float kMinimumPadding = 0.0f;
	constexpr float kMaximumSpacing = static_cast<float>(std::numeric_limits<uint16_t>::max());
	constexpr float kMaximumPadding = static_cast<float>(std::numeric_limits<uint16_t>::max());
	constexpr int32_t kMinimumElementCapacity = 64;
	constexpr int32_t kSolverElementOverhead = 8;

	std::mutex& GetClayApiMutex()
	{
		// Clay stores its current context globally, so isolate that mutable API behind one lock.
		static std::mutex mutex;
		return mutex;
	}

	struct ClayLayoutPassResult
	{
		OvMaths::FVector2 size = OvMaths::FVector2::Zero;
		std::vector<Clay_BoundingBox> childBoxes;
		std::vector<bool> childFound;
	};

	class ScopedClayContext
	{
	public:
		explicit ScopedClayContext(Clay_Context* p_context) :
		m_previousContext(Clay_GetCurrentContext())
		{
			Clay_SetCurrentContext(p_context);
		}

		~ScopedClayContext()
		{
			Clay_SetCurrentContext(m_previousContext);
		}

	private:
		Clay_Context* m_previousContext = nullptr;
	};

	class ClaySolverRuntime
	{
	public:
		Clay_Context* GetContext(size_t p_childCount)
		{
			const auto requiredCapacity = std::max(
				kMinimumElementCapacity,
				static_cast<int32_t>(p_childCount) + kSolverElementOverhead
			);

			if (!m_context || requiredCapacity > m_elementCapacity)
			{
				Initialize(requiredCapacity);
			}

			return m_context;
		}

	private:
		static void HandleError(Clay_ErrorData p_errorData)
		{
			auto* runtime = static_cast<ClaySolverRuntime*>(p_errorData.userData);
			if (!runtime)
			{
				return;
			}

			runtime->m_lastError.assign(p_errorData.errorText.chars, p_errorData.errorText.length);
		}

		void Initialize(int32_t p_elementCapacity)
		{
			std::lock_guard lock(GetClayApiMutex());
			auto* previousContext = Clay_GetCurrentContext();

			Clay_SetCurrentContext(nullptr);
			Clay_SetMaxElementCount(p_elementCapacity);

			const auto memorySize = Clay_MinMemorySize();
			m_memory.resize(memorySize);
			auto arena = Clay_CreateArenaWithCapacityAndMemory(m_memory.size(), m_memory.data());
			m_context = Clay_Initialize(
				arena,
				{ kMaximumLayoutSize, kMaximumLayoutSize },
				{ &ClaySolverRuntime::HandleError, this }
			);

			m_elementCapacity = p_elementCapacity;

			if (m_context)
			{
				ScopedClayContext useContext(m_context);
				Clay_SetCullingEnabled(false);
			}

			Clay_SetCurrentContext(previousContext);
		}

	private:
		Clay_Context* m_context = nullptr;
		int32_t m_elementCapacity = 0;
		std::vector<char> m_memory;
		std::string m_lastError;
	};

	float KeepFinite(float p_value, float p_fallback)
	{
		return std::isfinite(p_value) ? p_value : p_fallback;
	}

	float ClampNonNegative(float p_value)
	{
		return std::max(KeepFinite(p_value, 0.0f), 0.0f);
	}

	uint16_t ToClaySpacing(float p_spacing)
	{
		return static_cast<uint16_t>(std::clamp(KeepFinite(p_spacing, kMinimumSpacing), kMinimumSpacing, kMaximumSpacing));
	}

	Clay_Padding ToClayPadding(const OvMaths::FVector4& p_padding)
	{
		return {
			.left = static_cast<uint16_t>(std::clamp(KeepFinite(p_padding.x, kMinimumPadding), kMinimumPadding, kMaximumPadding)),
			.right = static_cast<uint16_t>(std::clamp(KeepFinite(p_padding.y, kMinimumPadding), kMinimumPadding, kMaximumPadding)),
			.top = static_cast<uint16_t>(std::clamp(KeepFinite(p_padding.z, kMinimumPadding), kMinimumPadding, kMaximumPadding)),
			.bottom = static_cast<uint16_t>(std::clamp(KeepFinite(p_padding.w, kMinimumPadding), kMinimumPadding, kMaximumPadding))
		};
	}

	Clay_LayoutDirection ToClayDirection(OvCore::ECS::Components::UI::CLayoutGroup::EDirection p_direction)
	{
		return p_direction == OvCore::ECS::Components::UI::CLayoutGroup::EDirection::VERTICAL ?
			CLAY_TOP_TO_BOTTOM :
			CLAY_LEFT_TO_RIGHT;
	}

	Clay_LayoutAlignmentX ToClayHorizontalAlignment(OvCore::ECS::Components::UI::CLayoutGroup::EHorizontalAlignment p_alignment)
	{
		using EHorizontalAlignment = OvCore::ECS::Components::UI::CLayoutGroup::EHorizontalAlignment;

		switch (p_alignment)
		{
		case EHorizontalAlignment::LEFT:
			return CLAY_ALIGN_X_LEFT;
		case EHorizontalAlignment::RIGHT:
			return CLAY_ALIGN_X_RIGHT;
		case EHorizontalAlignment::CENTER:
		default:
			return CLAY_ALIGN_X_CENTER;
		}
	}

	Clay_LayoutAlignmentY ToClayVerticalAlignment(OvCore::ECS::Components::UI::CLayoutGroup::EVerticalAlignment p_alignment)
	{
		using EVerticalAlignment = OvCore::ECS::Components::UI::CLayoutGroup::EVerticalAlignment;

		switch (p_alignment)
		{
		case EVerticalAlignment::TOP:
			return CLAY_ALIGN_Y_TOP;
		case EVerticalAlignment::BOTTOM:
			return CLAY_ALIGN_Y_BOTTOM;
		case EVerticalAlignment::CENTER:
		default:
			return CLAY_ALIGN_Y_CENTER;
		}
	}

	Clay_SizingAxis MakeFixedSizing(float p_size)
	{
		return CLAY_SIZING_FIXED(ClampNonNegative(p_size));
	}

	Clay_SizingAxis MakeFitSizing(float p_minSize)
	{
		return CLAY_SIZING_FIT(std::max(ClampNonNegative(p_minSize), kMinimumLayoutSize), kMaximumLayoutSize);
	}

	Clay_SizingAxis MakeGrowSizing(float p_minSize)
	{
		return CLAY_SIZING_GROW(ClampNonNegative(p_minSize), kMaximumLayoutSize);
	}

	Clay_SizingAxis MakeChildSizing(float p_preferredSize, bool p_controlSize, bool p_forceExpand)
	{
		if (p_controlSize)
		{
			return MakeGrowSizing(0.0f);
		}

		if (p_forceExpand)
		{
			return MakeGrowSizing(p_preferredSize);
		}

		return MakeFixedSizing(p_preferredSize);
	}

	Clay_ElementDeclaration CreateContainerDeclaration(
		const OvCore::ECS::Components::UI::ClayLayoutSettings& p_settings,
		Clay_SizingAxis p_widthSizing,
		Clay_SizingAxis p_heightSizing
	)
	{
		Clay_ElementDeclaration declaration{};
		declaration.layout.sizing.width = p_widthSizing;
		declaration.layout.sizing.height = p_heightSizing;
		declaration.layout.padding = ToClayPadding(p_settings.padding);
		declaration.layout.childGap = ToClaySpacing(p_settings.spacing);
		declaration.layout.childAlignment.x = ToClayHorizontalAlignment(p_settings.horizontalAlignment);
		declaration.layout.childAlignment.y = ToClayVerticalAlignment(p_settings.verticalAlignment);
		declaration.layout.layoutDirection = ToClayDirection(p_settings.direction);
		return declaration;
	}

	Clay_ElementDeclaration CreateChildDeclaration(
		const OvCore::ECS::Components::UI::ClayLayoutSettings& p_settings,
		const OvCore::ECS::Components::UI::ClayLayoutChildInput& p_child,
		bool p_useControlledSizing
	)
	{
		Clay_ElementDeclaration declaration{};
		declaration.layout.sizing.width = p_useControlledSizing ?
			MakeChildSizing(p_child.preferredSize.x, p_settings.controlChildrenWidth, p_settings.forceExpandWidth) :
			MakeFixedSizing(p_child.preferredSize.x);
		declaration.layout.sizing.height = p_useControlledSizing ?
			MakeChildSizing(p_child.preferredSize.y, p_settings.controlChildrenHeight, p_settings.forceExpandHeight) :
			MakeFixedSizing(p_child.preferredSize.y);
		return declaration;
	}

	ClayLayoutPassResult RunClayPass(
		Clay_Context* p_context,
		const OvCore::ECS::Components::UI::ClayLayoutSettings& p_settings,
		const std::vector<OvCore::ECS::Components::UI::ClayLayoutChildInput>& p_children,
		Clay_SizingAxis p_containerWidthSizing,
		Clay_SizingAxis p_containerHeightSizing,
		const OvMaths::FVector2& p_rootSize,
		bool p_useControlledChildSizing
	)
	{
		std::lock_guard lock(GetClayApiMutex());
		ScopedClayContext useContext(p_context);

		Clay_SetLayoutDimensions({
			std::max(ClampNonNegative(p_rootSize.x), kMinimumLayoutSize),
			std::max(ClampNonNegative(p_rootSize.y), kMinimumLayoutSize)
		});

		const auto containerId = CLAY_ID("Overload_LayoutGroup_Container");
		const auto childIdBase = CLAY_STRING("Overload_LayoutGroup_Child");
		std::vector<Clay_ElementId> childIds;
		childIds.reserve(p_children.size());

		Clay_BeginLayout();

		CLAY(containerId, CreateContainerDeclaration(p_settings, p_containerWidthSizing, p_containerHeightSizing))
		{
			for (size_t i = 0; i < p_children.size(); ++i)
			{
				const auto childId = Clay_GetElementIdWithIndex(childIdBase, static_cast<uint32_t>(i));
				childIds.push_back(childId);
				CLAY(childId, CreateChildDeclaration(p_settings, p_children[i], p_useControlledChildSizing))
				{
				}
			}
		}

		Clay_EndLayout(0.0f);

		ClayLayoutPassResult result;

		const auto containerData = Clay_GetElementData(containerId);
		if (!containerData.found)
		{
			return result;
		}

		result.size = {
			std::max(containerData.boundingBox.width, kMinimumLayoutSize),
			std::max(containerData.boundingBox.height, kMinimumLayoutSize)
		};

		result.childBoxes.reserve(childIds.size());
		result.childFound.reserve(childIds.size());
		for (const auto& childId : childIds)
		{
			const auto childData = Clay_GetElementData(childId);
			result.childBoxes.push_back(childData.found ? childData.boundingBox : Clay_BoundingBox{});
			result.childFound.push_back(childData.found);
		}

		return result;
	}

	OvMaths::FVector2 GetRootSize(const OvMaths::FVector2& p_containerSize, const OvMaths::FVector2& p_preferredSize)
	{
		return {
			p_containerSize.x > 0.0f ? p_containerSize.x : std::max(p_preferredSize.x, kMinimumLayoutSize),
			p_containerSize.y > 0.0f ? p_containerSize.y : std::max(p_preferredSize.y, kMinimumLayoutSize)
		};
	}

	OvMaths::FVector2 ToChildOffset(
		const OvMaths::FVector2& p_childTopLeft,
		const OvMaths::FVector2& p_childSize,
		const OvMaths::FVector2& p_layoutSize,
		const OvMaths::FVector2& p_pivot
	)
	{
		const float centerX = p_childTopLeft.x + p_childSize.x * 0.5f;
		const float centerY = p_childTopLeft.y + p_childSize.y * 0.5f;
		const auto halfSize = p_layoutSize * 0.5f;
		const OvMaths::FVector2 pivotOffset = {
			-p_pivot.x * halfSize.x,
			p_pivot.y * halfSize.y
		};

		return {
			centerX - halfSize.x + pivotOffset.x,
			halfSize.y - centerY + pivotOffset.y
		};
	}

	float GetHorizontalAlignmentOffset(
		OvCore::ECS::Components::UI::CLayoutGroup::EHorizontalAlignment p_alignment,
		float p_availableWidth,
		float p_contentWidth
	)
	{
		const float extraSpace = std::max(0.0f, p_availableWidth - p_contentWidth);

		switch (p_alignment)
		{
		case OvCore::ECS::Components::UI::CLayoutGroup::EHorizontalAlignment::LEFT:
			return 0.0f;
		case OvCore::ECS::Components::UI::CLayoutGroup::EHorizontalAlignment::RIGHT:
			return extraSpace;
		case OvCore::ECS::Components::UI::CLayoutGroup::EHorizontalAlignment::CENTER:
		default:
			return extraSpace * 0.5f;
		}
	}

	float GetVerticalAlignmentOffset(
		OvCore::ECS::Components::UI::CLayoutGroup::EVerticalAlignment p_alignment,
		float p_availableHeight,
		float p_contentHeight
	)
	{
		const float extraSpace = std::max(0.0f, p_availableHeight - p_contentHeight);

		switch (p_alignment)
		{
		case OvCore::ECS::Components::UI::CLayoutGroup::EVerticalAlignment::TOP:
			return 0.0f;
		case OvCore::ECS::Components::UI::CLayoutGroup::EVerticalAlignment::BOTTOM:
			return extraSpace;
		case OvCore::ECS::Components::UI::CLayoutGroup::EVerticalAlignment::CENTER:
		default:
			return extraSpace * 0.5f;
		}
	}

	OvMaths::FVector2 GetChildrenContentSize(
		const std::vector<OvCore::ECS::Components::UI::ClayLayoutChildResult>& p_children,
		OvCore::ECS::Components::UI::CLayoutGroup::EDirection p_direction,
		float p_spacing
	)
	{
		if (p_children.empty())
		{
			return OvMaths::FVector2::Zero;
		}

		OvMaths::FVector2 result = OvMaths::FVector2::Zero;

		for (const auto& child : p_children)
		{
			if (!child.valid)
			{
				continue;
			}

			if (p_direction == OvCore::ECS::Components::UI::CLayoutGroup::EDirection::HORIZONTAL)
			{
				result.x += child.size.x;
				result.y = std::max(result.y, child.size.y);
			}
			else
			{
				result.x = std::max(result.x, child.size.x);
				result.y += child.size.y;
			}
		}

		const auto validChildCount = static_cast<float>(std::count_if(p_children.begin(), p_children.end(), [](const auto& p_child)
		{
			return p_child.valid;
		}));
		const float gapSize = p_spacing * std::max(validChildCount - 1.0f, 0.0f);

		if (p_direction == OvCore::ECS::Components::UI::CLayoutGroup::EDirection::HORIZONTAL)
		{
			result.x += gapSize;
		}
		else
		{
			result.y += gapSize;
		}

		return result;
	}

	float GetValidChildCount(
		const std::vector<OvCore::ECS::Components::UI::ClayLayoutChildResult>& p_children
	)
	{
		return static_cast<float>(std::count_if(p_children.begin(), p_children.end(), [](const auto& p_child)
		{
			return p_child.valid;
		}));
	}

	void ApplyControlledSizing(
		OvCore::ECS::Components::UI::ClayLayoutResult& p_result,
		const OvCore::ECS::Components::UI::ClayLayoutSettings& p_settings
	)
	{
		if (
			!p_settings.controlChildrenWidth &&
			!p_settings.controlChildrenHeight &&
			!p_settings.forceExpandWidth &&
			!p_settings.forceExpandHeight
		)
		{
			return;
		}

		const auto childCount = GetValidChildCount(p_result.children);
		if (childCount <= 0.0f)
		{
			return;
		}

		const auto padding = ToClayPadding(p_settings.padding);
		const float spacing = static_cast<float>(ToClaySpacing(p_settings.spacing));
		const float availableWidth = std::max(0.0f, p_result.size.x - padding.left - padding.right);
		const float availableHeight = std::max(0.0f, p_result.size.y - padding.top - padding.bottom);

		float controlledWidth = 0.0f;
		if (p_settings.controlChildrenWidth || p_settings.forceExpandWidth)
		{
			if (p_settings.direction == OvCore::ECS::Components::UI::CLayoutGroup::EDirection::HORIZONTAL)
			{
				float occupiedWidth = 0.0f;
				for (const auto& child : p_result.children)
				{
					if (child.valid)
					{
						occupiedWidth += child.size.x;
					}
				}

				const float availableChildWidth = std::max(availableWidth - std::max(childCount - 1.0f, 0.0f) * spacing, 0.0f);
				controlledWidth = p_settings.controlChildrenWidth ?
					availableChildWidth / childCount :
					std::max((availableChildWidth - occupiedWidth) / childCount, 0.0f);
			}
			else
			{
				controlledWidth = availableWidth;
			}
		}

		float controlledHeight = 0.0f;
		if (p_settings.controlChildrenHeight || p_settings.forceExpandHeight)
		{
			if (p_settings.direction == OvCore::ECS::Components::UI::CLayoutGroup::EDirection::VERTICAL)
			{
				float occupiedHeight = 0.0f;
				for (const auto& child : p_result.children)
				{
					if (child.valid)
					{
						occupiedHeight += child.size.y;
					}
				}

				const float availableChildHeight = std::max(availableHeight - std::max(childCount - 1.0f, 0.0f) * spacing, 0.0f);
				controlledHeight = p_settings.controlChildrenHeight ?
					availableChildHeight / childCount :
					std::max((availableChildHeight - occupiedHeight) / childCount, 0.0f);
			}
			else
			{
				controlledHeight = availableHeight;
			}
		}

		for (auto& child : p_result.children)
		{
			if (!child.valid)
			{
				continue;
			}

			if (p_settings.controlChildrenWidth)
			{
				child.size.x = controlledWidth;
			}
			else if (p_settings.forceExpandWidth)
			{
				child.size.x = p_settings.direction == OvCore::ECS::Components::UI::CLayoutGroup::EDirection::HORIZONTAL ?
					child.size.x + controlledWidth :
					std::max(child.size.x, controlledWidth);
			}

			if (p_settings.controlChildrenHeight)
			{
				child.size.y = controlledHeight;
			}
			else if (p_settings.forceExpandHeight)
			{
				child.size.y = p_settings.direction == OvCore::ECS::Components::UI::CLayoutGroup::EDirection::VERTICAL ?
					child.size.y + controlledHeight :
					std::max(child.size.y, controlledHeight);
			}
		}
	}

	void ApplyAlignedOffsets(
		OvCore::ECS::Components::UI::ClayLayoutResult& p_result,
		const OvCore::ECS::Components::UI::ClayLayoutSettings& p_settings
	)
	{
		const auto padding = ToClayPadding(p_settings.padding);
		const float spacing = static_cast<float>(ToClaySpacing(p_settings.spacing));
		const float availableWidth = std::max(0.0f, p_result.size.x - padding.left - padding.right);
		const float availableHeight = std::max(0.0f, p_result.size.y - padding.top - padding.bottom);
		const auto contentSize = GetChildrenContentSize(p_result.children, p_settings.direction, spacing);

		if (p_settings.direction == OvCore::ECS::Components::UI::CLayoutGroup::EDirection::HORIZONTAL)
		{
			float childLeft = static_cast<float>(padding.left) +
				GetHorizontalAlignmentOffset(p_settings.horizontalAlignment, availableWidth, contentSize.x);

			for (auto& child : p_result.children)
			{
				if (!child.valid)
				{
					continue;
				}

				const float childTop = static_cast<float>(padding.top) +
					GetVerticalAlignmentOffset(p_settings.verticalAlignment, availableHeight, child.size.y);
				child.offset = ToChildOffset({ childLeft, childTop }, child.size, p_result.size, p_settings.pivot);
				childLeft += child.size.x + spacing;
			}
		}
		else
		{
			float childTop = static_cast<float>(padding.top) +
				GetVerticalAlignmentOffset(p_settings.verticalAlignment, availableHeight, contentSize.y);

			for (auto& child : p_result.children)
			{
				if (!child.valid)
				{
					continue;
				}

				const float childLeft = static_cast<float>(padding.left) +
					GetHorizontalAlignmentOffset(p_settings.horizontalAlignment, availableWidth, child.size.x);
				child.offset = ToChildOffset({ childLeft, childTop }, child.size, p_result.size, p_settings.pivot);
				childTop += child.size.y + spacing;
			}
		}
	}
}

struct OvCore::ECS::Components::UI::ClayLayoutSolverContext::Impl : ClaySolverRuntime
{
};

OvCore::ECS::Components::UI::ClayLayoutSolverContext::ClayLayoutSolverContext() :
m_impl(std::make_unique<Impl>())
{
}

OvCore::ECS::Components::UI::ClayLayoutSolverContext::~ClayLayoutSolverContext() = default;

OvCore::ECS::Components::UI::ClayLayoutSolverContext::ClayLayoutSolverContext(ClayLayoutSolverContext&&) noexcept = default;

OvCore::ECS::Components::UI::ClayLayoutSolverContext& OvCore::ECS::Components::UI::ClayLayoutSolverContext::operator=(ClayLayoutSolverContext&&) noexcept = default;

OvCore::ECS::Components::UI::ClayLayoutMeasurement OvCore::ECS::Components::UI::ClayLayoutSolver::Measure(
	ClayLayoutSolverContext& p_context,
	const ClayLayoutSettings& p_settings,
	const std::vector<ClayLayoutChildInput>& p_children
)
{
	ClayLayoutMeasurement measurement;
	measurement.settings = p_settings;
	measurement.settings.containerSize.x = ClampNonNegative(measurement.settings.containerSize.x);
	measurement.settings.containerSize.y = ClampNonNegative(measurement.settings.containerSize.y);

	auto* context = p_context.m_impl ? p_context.m_impl->GetContext(p_children.size()) : nullptr;
	if (!context)
	{
		return measurement;
	}

	const Clay_SizingAxis measuredWidthSizing = measurement.settings.containerSize.x > 0.0f ?
		MakeFixedSizing(measurement.settings.containerSize.x) :
		MakeFitSizing(kMinimumLayoutSize);
	const Clay_SizingAxis measuredHeightSizing = measurement.settings.containerSize.y > 0.0f ?
		MakeFixedSizing(measurement.settings.containerSize.y) :
		MakeFitSizing(kMinimumLayoutSize);

	const auto preferredPass = RunClayPass(
		context,
		measurement.settings,
		p_children,
		measuredWidthSizing,
		measuredHeightSizing,
		{ kMaximumLayoutSize, kMaximumLayoutSize },
		false
	);

	measurement.preferredSize = preferredPass.size;
	measurement.valid = true;
	return measurement;
}

OvCore::ECS::Components::UI::ClayLayoutSolution OvCore::ECS::Components::UI::ClayLayoutSolver::SolveLayout(
	ClayLayoutSolverContext& p_context,
	const ClayLayoutMeasurement& p_measurement,
	const std::vector<ClayLayoutChildInput>& p_children
)
{
	ClayLayoutSolution solution;
	solution.settings = p_measurement.settings;
	solution.preferredSize = p_measurement.preferredSize;

	if (!p_measurement.valid)
	{
		return solution;
	}

	auto* context = p_context.m_impl ? p_context.m_impl->GetContext(p_children.size()) : nullptr;
	if (!context)
	{
		return solution;
	}

	const auto& settings = p_measurement.settings;

	const Clay_SizingAxis finalWidthSizing = settings.containerSize.x > 0.0f ?
		MakeFixedSizing(settings.containerSize.x) :
		MakeFitSizing(p_measurement.preferredSize.x);
	const Clay_SizingAxis finalHeightSizing = settings.containerSize.y > 0.0f ?
		MakeFixedSizing(settings.containerSize.y) :
		MakeFitSizing(p_measurement.preferredSize.y);

	const auto finalPass = RunClayPass(
		context,
		settings,
		p_children,
		finalWidthSizing,
		finalHeightSizing,
		GetRootSize(settings.containerSize, p_measurement.preferredSize),
		true
	);

	solution.result.size = GetRootSize(settings.containerSize, p_measurement.preferredSize);
	solution.result.children.reserve(p_children.size());

	for (size_t i = 0; i < p_children.size(); ++i)
	{
		const auto childBox = i < finalPass.childBoxes.size() ? finalPass.childBoxes[i] : Clay_BoundingBox{};
		const bool childValid =
			i < finalPass.childFound.size() &&
			finalPass.childFound[i] &&
			childBox.width > 0.0f &&
			childBox.height > 0.0f;
		const auto childSize = childValid ?
			OvMaths::FVector2{ childBox.width, childBox.height } :
			p_children[i].preferredSize;
		const bool hasUsableChild = p_children[i].actor && childSize.x > 0.0f && childSize.y > 0.0f;

		solution.result.children.push_back({
			.actor = p_children[i].actor,
			.offset = OvMaths::FVector2::Zero,
			.size = childSize,
			.valid = hasUsableChild
		});
	}

	solution.valid = true;
	return solution;
}

OvCore::ECS::Components::UI::ClayLayoutResult OvCore::ECS::Components::UI::ClayLayoutSolver::Postprocess(
	const ClayLayoutSolution& p_solution
)
{
	if (!p_solution.valid)
	{
		return {};
	}

	auto result = p_solution.result;
	ApplyControlledSizing(result, p_solution.settings);
	ApplyAlignedOffsets(result, p_solution.settings);
	return result;
}

OvCore::ECS::Components::UI::ClayLayoutResult OvCore::ECS::Components::UI::ClayLayoutSolver::Solve(
	ClayLayoutSolverContext& p_context,
	const ClayLayoutSettings& p_settings,
	const std::vector<ClayLayoutChildInput>& p_children
)
{
	const auto measurement = Measure(p_context, p_settings, p_children);
	const auto solution = SolveLayout(p_context, measurement, p_children);
	return Postprocess(solution);
}
