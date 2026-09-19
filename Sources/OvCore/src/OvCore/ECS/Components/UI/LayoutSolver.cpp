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
#include <vector>

#include <OvCore/ECS/Actor.h>
#include <OvCore/ECS/Components/UI/LayoutSolver.h>

namespace
{
	constexpr float kMinimumLayoutSize = 1.0f;
	constexpr float kMinimumSpacing = 0.0f;
	constexpr float kMinimumPadding = 0.0f;
	constexpr float kMaximumSpacing = static_cast<float>(std::numeric_limits<uint16_t>::max());
	constexpr float kMaximumPadding = static_cast<float>(std::numeric_limits<uint16_t>::max());

	struct LayoutPadding
	{
		float left = 0.0f;
		float right = 0.0f;
		float top = 0.0f;
		float bottom = 0.0f;
	};

	float KeepFinite(float p_value, float p_fallback)
	{
		return std::isfinite(p_value) ? p_value : p_fallback;
	}

	float ClampNonNegative(float p_value)
	{
		return std::max(KeepFinite(p_value, 0.0f), 0.0f);
	}

	// Spacing and padding are kept whole so that slots always land on integer offsets
	float ToSpacing(float p_spacing)
	{
		return std::trunc(std::clamp(KeepFinite(p_spacing, kMinimumSpacing), kMinimumSpacing, kMaximumSpacing));
	}

	float ToPaddingAxis(float p_padding)
	{
		return std::trunc(std::clamp(KeepFinite(p_padding, kMinimumPadding), kMinimumPadding, kMaximumPadding));
	}

	LayoutPadding ToPadding(const OvMaths::FVector4& p_padding)
	{
		return {
			.left = ToPaddingAxis(p_padding.x),
			.right = ToPaddingAxis(p_padding.y),
			.top = ToPaddingAxis(p_padding.z),
			.bottom = ToPaddingAxis(p_padding.w)
		};
	}

	bool IsHorizontal(const OvCore::ECS::Components::UI::LayoutSettings& p_settings)
	{
		return p_settings.direction == OvCore::ECS::Components::UI::CLayoutGroup::EDirection::HORIZONTAL;
	}

	float GetHorizontalAlignmentFactor(
		OvCore::ECS::Components::UI::CLayoutGroup::EHorizontalAlignment p_alignment
	)
	{
		switch (p_alignment)
		{
		case OvCore::ECS::Components::UI::CLayoutGroup::EHorizontalAlignment::LEFT:
			return 0.0f;
		case OvCore::ECS::Components::UI::CLayoutGroup::EHorizontalAlignment::RIGHT:
			return 1.0f;
		case OvCore::ECS::Components::UI::CLayoutGroup::EHorizontalAlignment::CENTER:
		default:
			return 0.5f;
		}
	}

	float GetVerticalAlignmentFactor(
		OvCore::ECS::Components::UI::CLayoutGroup::EVerticalAlignment p_alignment
	)
	{
		switch (p_alignment)
		{
		case OvCore::ECS::Components::UI::CLayoutGroup::EVerticalAlignment::TOP:
			return 0.0f;
		case OvCore::ECS::Components::UI::CLayoutGroup::EVerticalAlignment::BOTTOM:
			return 1.0f;
		case OvCore::ECS::Components::UI::CLayoutGroup::EVerticalAlignment::CENTER:
		default:
			return 0.5f;
		}
	}

	OvMaths::FVector2 ToChildOffset(
		const OvMaths::FVector2& p_childTopLeft,
		const OvMaths::FVector2& p_childSize,
		const OvMaths::FVector2& p_layoutSize
	)
	{
		const float centerX = p_childTopLeft.x + p_childSize.x * 0.5f;
		const float centerY = p_childTopLeft.y + p_childSize.y * 0.5f;
		const auto halfSize = p_layoutSize * 0.5f;

		return {
			centerX - halfSize.x,
			halfSize.y - centerY
		};
	}

	// Raises the smallest slots first until every slot shares the same size, then splits what is
	// left evenly. Slots are never shrunk below the size they requested.
	void ExpandAlongMainAxis(std::vector<float>& p_sizes, float p_availableSize)
	{
		if (p_sizes.empty())
		{
			return;
		}

		float contentSize = 0.0f;
		for (const auto size : p_sizes)
		{
			contentSize += size;
		}

		float remainingSize = p_availableSize - contentSize;
		if (remainingSize <= 0.0f)
		{
			return;
		}

		std::vector<float> sortedSizes = p_sizes;
		std::sort(sortedSizes.begin(), sortedSizes.end());

		float level = sortedSizes.back();
		for (size_t i = 0; i < sortedSizes.size(); ++i)
		{
			const auto slotCount = static_cast<float>(i + 1);
			const bool isLastLevel = i + 1 == sortedSizes.size();
			const float levelCost = isLastLevel ?
				remainingSize :
				(sortedSizes[i + 1] - sortedSizes[i]) * slotCount;

			if (remainingSize <= levelCost)
			{
				level = sortedSizes[i] + remainingSize / slotCount;
				break;
			}

			remainingSize -= levelCost;
		}

		for (auto& size : p_sizes)
		{
			size = std::max(size, level);
		}
	}

	OvMaths::FVector2 CalculateContentSize(
		const OvCore::ECS::Components::UI::LayoutSettings& p_settings,
		const std::vector<OvCore::ECS::Components::UI::LayoutChildInput>& p_children,
		const LayoutPadding& p_padding,
		float p_spacing
	)
	{
		const bool horizontal = IsHorizontal(p_settings);
		float mainAxisSize = 0.0f;
		float crossAxisSize = 0.0f;

		for (const auto& child : p_children)
		{
			const float childWidth = ClampNonNegative(child.preferredSize.x);
			const float childHeight = ClampNonNegative(child.preferredSize.y);

			mainAxisSize += horizontal ? childWidth : childHeight;
			crossAxisSize = std::max(crossAxisSize, horizontal ? childHeight : childWidth);
		}

		if (p_children.size() > 1)
		{
			mainAxisSize += p_spacing * static_cast<float>(p_children.size() - 1);
		}

		const float horizontalPadding = p_padding.left + p_padding.right;
		const float verticalPadding = p_padding.top + p_padding.bottom;

		return horizontal ?
			OvMaths::FVector2{ mainAxisSize + horizontalPadding, crossAxisSize + verticalPadding } :
			OvMaths::FVector2{ crossAxisSize + horizontalPadding, mainAxisSize + verticalPadding };
	}

	OvMaths::FVector2 GetLayoutSize(
		const OvMaths::FVector2& p_containerSize,
		const OvMaths::FVector2& p_contentSize
	)
	{
		return {
			p_containerSize.x > 0.0f ? p_containerSize.x : std::max(p_contentSize.x, kMinimumLayoutSize),
			p_containerSize.y > 0.0f ? p_containerSize.y : std::max(p_contentSize.y, kMinimumLayoutSize)
		};
	}

	std::vector<OvMaths::FVector2> CalculateSlotSizes(
		const OvCore::ECS::Components::UI::LayoutSettings& p_settings,
		const std::vector<OvCore::ECS::Components::UI::LayoutChildInput>& p_children,
		const OvMaths::FVector2& p_layoutSize,
		const LayoutPadding& p_padding,
		float p_spacing
	)
	{
		const bool horizontal = IsHorizontal(p_settings);
		const float availableWidth = std::max(p_layoutSize.x - (p_padding.left + p_padding.right), 0.0f);
		const float availableHeight = std::max(p_layoutSize.y - (p_padding.top + p_padding.bottom), 0.0f);

		std::vector<OvMaths::FVector2> slotSizes;
		slotSizes.reserve(p_children.size());

		for (const auto& child : p_children)
		{
			slotSizes.push_back({
				ClampNonNegative(child.preferredSize.x),
				ClampNonNegative(child.preferredSize.y)
			});
		}

		const bool expandMainAxis = horizontal ? p_settings.forceExpandWidth : p_settings.forceExpandHeight;
		const bool expandCrossAxis = horizontal ? p_settings.forceExpandHeight : p_settings.forceExpandWidth;

		if (expandMainAxis && !slotSizes.empty())
		{
			const float totalSpacing = slotSizes.size() > 1 ?
				p_spacing * static_cast<float>(slotSizes.size() - 1) :
				0.0f;
			const float availableMainAxisSize = std::max(
				(horizontal ? availableWidth : availableHeight) - totalSpacing,
				0.0f
			);

			std::vector<float> mainAxisSizes;
			mainAxisSizes.reserve(slotSizes.size());

			for (const auto& slotSize : slotSizes)
			{
				mainAxisSizes.push_back(horizontal ? slotSize.x : slotSize.y);
			}

			ExpandAlongMainAxis(mainAxisSizes, availableMainAxisSize);

			for (size_t i = 0; i < slotSizes.size(); ++i)
			{
				float& mainAxisSize = horizontal ? slotSizes[i].x : slotSizes[i].y;
				mainAxisSize = mainAxisSizes[i];
			}
		}

		if (expandCrossAxis)
		{
			const float availableCrossAxisSize = horizontal ? availableHeight : availableWidth;

			for (auto& slotSize : slotSizes)
			{
				float& crossAxisSize = horizontal ? slotSize.y : slotSize.x;
				crossAxisSize = std::max(crossAxisSize, availableCrossAxisSize);
			}
		}

		return slotSizes;
	}

	std::vector<OvCore::ECS::Components::UI::LayoutChildResult> ProjectSlotsToChildren(
		const OvCore::ECS::Components::UI::LayoutSettings& p_settings,
		const std::vector<OvCore::ECS::Components::UI::LayoutChildInput>& p_children,
		const std::vector<OvMaths::FVector2>& p_slotSizes,
		const OvMaths::FVector2& p_layoutSize,
		const LayoutPadding& p_padding,
		float p_spacing
	)
	{
		using LayoutChildResult = OvCore::ECS::Components::UI::LayoutChildResult;

		const bool horizontal = IsHorizontal(p_settings);
		const float horizontalAlignment = GetHorizontalAlignmentFactor(p_settings.horizontalAlignment);
		const float verticalAlignment = GetVerticalAlignmentFactor(p_settings.verticalAlignment);
		const float availableWidth = std::max(p_layoutSize.x - (p_padding.left + p_padding.right), 0.0f);
		const float availableHeight = std::max(p_layoutSize.y - (p_padding.top + p_padding.bottom), 0.0f);

		float mainAxisContentSize = 0.0f;
		for (const auto& slotSize : p_slotSizes)
		{
			mainAxisContentSize += horizontal ? slotSize.x : slotSize.y;
		}

		if (p_slotSizes.size() > 1)
		{
			mainAxisContentSize += p_spacing * static_cast<float>(p_slotSizes.size() - 1);
		}

		const float mainAxisAvailableSize = horizontal ? availableWidth : availableHeight;
		const float mainAxisAlignment = horizontal ? horizontalAlignment : verticalAlignment;
		float mainAxisCursor =
			(horizontal ? p_padding.left : p_padding.top) +
			std::max(mainAxisAvailableSize - mainAxisContentSize, 0.0f) * mainAxisAlignment;

		std::vector<LayoutChildResult> results;
		results.reserve(p_children.size());

		for (size_t i = 0; i < p_children.size(); ++i)
		{
			const auto& slotSize = p_slotSizes[i];
			OvMaths::FVector2 slotTopLeft;

			if (horizontal)
			{
				slotTopLeft = {
					mainAxisCursor,
					p_padding.top + std::max(availableHeight - slotSize.y, 0.0f) * verticalAlignment
				};
				mainAxisCursor += slotSize.x + p_spacing;
			}
			else
			{
				slotTopLeft = {
					p_padding.left + std::max(availableWidth - slotSize.x, 0.0f) * horizontalAlignment,
					mainAxisCursor
				};
				mainAxisCursor += slotSize.y + p_spacing;
			}

			const OvMaths::FVector2 childSize = {
				p_settings.controlChildrenWidth ? slotSize.x : ClampNonNegative(p_children[i].preferredSize.x),
				p_settings.controlChildrenHeight ? slotSize.y : ClampNonNegative(p_children[i].preferredSize.y)
			};
			const OvMaths::FVector2 childTopLeft = {
				slotTopLeft.x + std::max(slotSize.x - childSize.x, 0.0f) * horizontalAlignment,
				slotTopLeft.y + std::max(slotSize.y - childSize.y, 0.0f) * verticalAlignment
			};

			results.push_back({
				.actor = p_children[i].actor,
				.offset = ToChildOffset(childTopLeft, childSize, p_layoutSize),
				.size = childSize,
				.valid = p_children[i].actor && childSize.x > 0.0f && childSize.y > 0.0f
			});
		}

		return results;
	}
}

OvCore::ECS::Components::UI::LayoutResult OvCore::ECS::Components::UI::LayoutSolver::Solve(
	const LayoutSettings& p_settings,
	const std::vector<LayoutChildInput>& p_children
)
{
	LayoutSettings settings = p_settings;
	settings.containerSize.x = ClampNonNegative(settings.containerSize.x);
	settings.containerSize.y = ClampNonNegative(settings.containerSize.y);

	const auto padding = ToPadding(settings.padding);
	const float spacing = ToSpacing(settings.spacing);
	const auto contentSize = CalculateContentSize(settings, p_children, padding, spacing);

	LayoutResult result;
	result.size = GetLayoutSize(settings.containerSize, contentSize);

	const auto slotSizes = CalculateSlotSizes(settings, p_children, result.size, padding, spacing);
	result.children = ProjectSlotsToChildren(settings, p_children, slotSizes, result.size, padding, spacing);

	return result;
}
