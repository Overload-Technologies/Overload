/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <cmath>
#include <functional>
#include <limits>
#include <optional>
#include <vector>

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable: 4244 4305)
#endif

#include <clay.h>

#ifdef _MSC_VER
#pragma warning(pop)
#endif

#include <tinyxml2.h>

#include <OvCore/ECS/Actor.h>
#include <OvCore/ECS/Components/UI/CImage.h>
#include <OvCore/ECS/Components/UI/CLayoutGroup.h>
#include <OvCore/ECS/Components/UI/CText.h>
#include <OvCore/Helpers/GUIDrawer.h>
#include <OvCore/Helpers/Serializer.h>

#include <OvUI/Widgets/Selection/ComboBox.h>

namespace
{
	struct LayoutChild
	{
		OvCore::ECS::Actor* actor = nullptr;
		OvMaths::FVector2 size;
	};

	constexpr float kMinimumSpacing = 0.0f;
	constexpr float kMinimumPadding = 0.0f;
	constexpr float kMaximumSpacing = static_cast<float>(std::numeric_limits<uint16_t>::max());
	constexpr float kMaximumPadding = static_cast<float>(std::numeric_limits<uint16_t>::max());
	constexpr float kSizeUpdateEpsilon = 0.0001f;

	float ClampSpacing(float p_value, float p_fallback)
	{
		return std::isfinite(p_value) ? std::clamp(p_value, kMinimumSpacing, kMaximumSpacing) : p_fallback;
	}

	float ClampPadding(float p_value, float p_fallback)
	{
		return std::isfinite(p_value) ? std::clamp(p_value, kMinimumPadding, kMaximumPadding) : p_fallback;
	}

	bool IsNearlyEqual(float p_left, float p_right)
	{
		return std::abs(p_left - p_right) <= kSizeUpdateEpsilon;
	}

	bool ShouldUpdateControlledSize(
		const OvMaths::FVector2& p_currentSize,
		const OvMaths::FVector2& p_targetSize,
		bool p_controlWidth,
		bool p_controlHeight
	)
	{
		if (p_controlWidth && !IsNearlyEqual(p_currentSize.x, p_targetSize.x))
		{
			return true;
		}

		if (p_controlHeight && !IsNearlyEqual(p_currentSize.y, p_targetSize.y))
		{
			return true;
		}

		return false;
	}

	OvCore::ECS::Components::UI::CLayoutGroup::EDirection ToDirection(int p_value)
	{
		using EDirection = OvCore::ECS::Components::UI::CLayoutGroup::EDirection;

		switch (p_value)
		{
		case static_cast<int>(EDirection::VERTICAL):
			return EDirection::VERTICAL;
		case static_cast<int>(EDirection::HORIZONTAL):
		default:
			return EDirection::HORIZONTAL;
		}
	}

	OvCore::ECS::Components::UI::CLayoutGroup::EHorizontalAlignment ToHorizontalAlignment(int p_value)
	{
		using EHorizontalAlignment = OvCore::ECS::Components::UI::CLayoutGroup::EHorizontalAlignment;

		switch (p_value)
		{
		case static_cast<int>(EHorizontalAlignment::LEFT):
			return EHorizontalAlignment::LEFT;
		case static_cast<int>(EHorizontalAlignment::RIGHT):
			return EHorizontalAlignment::RIGHT;
		case static_cast<int>(EHorizontalAlignment::CENTER):
		default:
			return EHorizontalAlignment::CENTER;
		}
	}

	OvCore::ECS::Components::UI::CLayoutGroup::EVerticalAlignment ToVerticalAlignment(int p_value)
	{
		using EVerticalAlignment = OvCore::ECS::Components::UI::CLayoutGroup::EVerticalAlignment;

		switch (p_value)
		{
		case static_cast<int>(EVerticalAlignment::TOP):
			return EVerticalAlignment::TOP;
		case static_cast<int>(EVerticalAlignment::BOTTOM):
			return EVerticalAlignment::BOTTOM;
		case static_cast<int>(EVerticalAlignment::CENTER):
		default:
			return EVerticalAlignment::CENTER;
		}
	}

	std::optional<OvMaths::FVector2> GetLayoutSize(const OvCore::ECS::Actor& p_child)
	{
		OvMaths::FVector2 elementSize = OvMaths::FVector2::Zero;
		bool hasElementSize = false;

		if (const auto* image = p_child.GetComponent<OvCore::ECS::Components::UI::CImage>(); image)
		{
			elementSize = image->GetSize();
			hasElementSize = true;
		}
		else if (const auto* text = p_child.GetComponent<OvCore::ECS::Components::UI::CText>(); text)
		{
			elementSize = text->GetSize();
			hasElementSize = true;
		}

		if (p_child.transform.HasActiveUIData())
		{
			const auto size = p_child.transform.GetUIEffectiveSize(elementSize);
			if (size.x > 0.0f && size.y > 0.0f)
			{
				return size;
			}
		}

		if (hasElementSize && elementSize.x > 0.0f && elementSize.y > 0.0f)
		{
			return elementSize;
		}

		return std::nullopt;
	}

	std::vector<LayoutChild> CollectLayoutChildren(OvCore::ECS::Actor& p_owner)
	{
		std::vector<LayoutChild> layoutChildren;

		for (const auto child : p_owner.GetChildren())
		{
			if (!child || !child->IsActive())
			{
				continue;
			}

			const auto size = GetLayoutSize(*child);
			if (!size)
			{
				continue;
			}

			layoutChildren.push_back({ child, size.value() });
		}

		return layoutChildren;
	}

	uint16_t ToClaySpacing(float p_spacing)
	{
		return static_cast<uint16_t>(ClampSpacing(p_spacing, kMinimumSpacing));
	}

	Clay_Padding ToClayPadding(const OvMaths::FVector4& p_padding)
	{
		return {
			.left = static_cast<uint16_t>(ClampPadding(p_padding.x, kMinimumPadding)),
			.right = static_cast<uint16_t>(ClampPadding(p_padding.y, kMinimumPadding)),
			.top = static_cast<uint16_t>(ClampPadding(p_padding.z, kMinimumPadding)),
			.bottom = static_cast<uint16_t>(ClampPadding(p_padding.w, kMinimumPadding))
		};
	}

	OvMaths::FVector2 GetIntrinsicLayoutSize(
		const std::vector<LayoutChild>& p_children,
		OvCore::ECS::Components::UI::CLayoutGroup::EDirection p_direction,
		float p_spacing,
		Clay_Padding p_padding
	)
	{
		OvMaths::FVector2 result = OvMaths::FVector2::Zero;

		for (size_t i = 0; i < p_children.size(); ++i)
		{
			const auto& childSize = p_children[i].size;

			if (p_direction == OvCore::ECS::Components::UI::CLayoutGroup::EDirection::HORIZONTAL)
			{
				result.x += childSize.x;
				result.y = std::max(result.y, childSize.y);
			}
			else
			{
				result.x = std::max(result.x, childSize.x);
				result.y += childSize.y;
			}

			if (i > 0)
			{
				if (p_direction == OvCore::ECS::Components::UI::CLayoutGroup::EDirection::HORIZONTAL)
				{
					result.x += p_spacing;
				}
				else
				{
					result.y += p_spacing;
				}
			}
		}

		result.x += static_cast<float>(p_padding.left + p_padding.right);
		result.y += static_cast<float>(p_padding.top + p_padding.bottom);

		return {
			std::max(result.x, 1.0f),
			std::max(result.y, 1.0f)
		};
	}

	OvMaths::FVector2 GetLayoutSize(
		const OvCore::ECS::Actor& p_owner,
		const std::vector<LayoutChild>& p_children,
		OvCore::ECS::Components::UI::CLayoutGroup::EDirection p_direction,
		float p_spacing,
		Clay_Padding p_padding
	)
	{
		return p_owner.transform.GetUIEffectiveSize(
			GetIntrinsicLayoutSize(p_children, p_direction, p_spacing, p_padding)
		);
	}

	void ApplyControlledChildrenSizing(
		std::vector<LayoutChild>& p_children,
		OvCore::ECS::Components::UI::CLayoutGroup::EDirection p_direction,
		bool p_controlChildrenWidth,
		bool p_controlChildrenHeight,
		bool p_forceExpandWidth,
		bool p_forceExpandHeight,
		const OvMaths::FVector2& p_layoutSize,
		uint16_t p_spacing,
		Clay_Padding p_padding
	)
	{
		if (p_children.empty())
		{
			return;
		}

		if (!p_controlChildrenWidth && !p_controlChildrenHeight && !p_forceExpandWidth && !p_forceExpandHeight)
		{
			return;
		}

		const auto childCount = static_cast<float>(p_children.size());
		const float spacing = static_cast<float>(p_spacing);
		const float horizontalPadding = static_cast<float>(p_padding.left + p_padding.right);
		const float verticalPadding = static_cast<float>(p_padding.top + p_padding.bottom);
		const float availableWidth = std::max(p_layoutSize.x - horizontalPadding, 0.0f);
		const float availableHeight = std::max(p_layoutSize.y - verticalPadding, 0.0f);

		float controlledWidth = 0.0f;
		if (p_controlChildrenWidth || p_forceExpandWidth)
		{
			if (p_direction == OvCore::ECS::Components::UI::CLayoutGroup::EDirection::HORIZONTAL)
			{
				float occupiedWidth = 0.0f;
				for (const auto& child : p_children)
				{
					occupiedWidth += child.size.x;
				}

				const float availableChildWidth = std::max(availableWidth - std::max(childCount - 1.0f, 0.0f) * spacing, 0.0f);
				controlledWidth = p_controlChildrenWidth ?
					availableChildWidth / childCount :
					std::max((availableChildWidth - occupiedWidth) / childCount, 0.0f);
			}
			else
			{
				controlledWidth = availableWidth;
			}
		}

		float controlledHeight = 0.0f;
		if (p_controlChildrenHeight || p_forceExpandHeight)
		{
			if (p_direction == OvCore::ECS::Components::UI::CLayoutGroup::EDirection::VERTICAL)
			{
				float occupiedHeight = 0.0f;
				for (const auto& child : p_children)
				{
					occupiedHeight += child.size.y;
				}

				const float availableChildHeight = std::max(availableHeight - std::max(childCount - 1.0f, 0.0f) * spacing, 0.0f);
				controlledHeight = p_controlChildrenHeight ?
					availableChildHeight / childCount :
					std::max((availableChildHeight - occupiedHeight) / childCount, 0.0f);
			}
			else
			{
				controlledHeight = availableHeight;
			}
		}

		for (auto& child : p_children)
		{
			if (p_controlChildrenWidth)
			{
				child.size.x = controlledWidth;
			}
			else if (p_forceExpandWidth)
			{
				child.size.x = p_direction == OvCore::ECS::Components::UI::CLayoutGroup::EDirection::HORIZONTAL ?
					child.size.x + controlledWidth :
					std::max(child.size.x, controlledWidth);
			}

			if (p_controlChildrenHeight)
			{
				child.size.y = controlledHeight;
			}
			else if (p_forceExpandHeight)
			{
				child.size.y = p_direction == OvCore::ECS::Components::UI::CLayoutGroup::EDirection::VERTICAL ?
					child.size.y + controlledHeight :
					std::max(child.size.y, controlledHeight);
			}
		}
	}

	void ApplyControlledSizeToChildren(
		const std::vector<LayoutChild>& p_children,
		bool p_controlChildrenWidth,
		bool p_controlChildrenHeight,
		bool p_forceExpandWidth,
		bool p_forceExpandHeight
	)
	{
		const bool appliesWidth = p_controlChildrenWidth || p_forceExpandWidth;
		const bool appliesHeight = p_controlChildrenHeight || p_forceExpandHeight;

		if (!appliesWidth && !appliesHeight)
		{
			return;
		}

		for (const auto& child : p_children)
		{
			if (!child.actor)
			{
				continue;
			}

			if (auto* image = child.actor->GetComponent<OvCore::ECS::Components::UI::CImage>())
			{
				auto nextSize = image->GetSize();
				if (appliesWidth)
				{
					nextSize.x = child.size.x;
				}
				if (appliesHeight)
				{
					nextSize.y = child.size.y;
				}

				if (ShouldUpdateControlledSize(image->GetSize(), nextSize, appliesWidth, appliesHeight))
				{
					image->SetSize(nextSize);
				}
			}

			if (auto* text = child.actor->GetComponent<OvCore::ECS::Components::UI::CText>())
			{
				auto nextExtents = text->GetExtents();
				if (appliesWidth)
				{
					nextExtents.x = child.size.x;
				}
				if (appliesHeight)
				{
					nextExtents.y = child.size.y;
				}

				if (ShouldUpdateControlledSize(text->GetExtents(), nextExtents, appliesWidth, appliesHeight))
				{
					text->SetExtents(nextExtents);
				}
			}

			if (child.actor->transform.HasActiveUIData())
			{
				auto nextSize = child.actor->transform.GetUISize();
				if (appliesWidth)
				{
					nextSize.x = child.size.x;
				}
				if (appliesHeight)
				{
					nextSize.y = child.size.y;
				}

				if (ShouldUpdateControlledSize(child.actor->transform.GetUISize(), nextSize, appliesWidth, appliesHeight))
				{
					child.actor->transform.SetUISize(nextSize);
				}
			}
		}
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
		const std::vector<LayoutChild>& p_children,
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

		const float gapSize = p_spacing * static_cast<float>(p_children.size() - 1);
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

	OvMaths::FVector2 ToChildOffset(
		const OvMaths::FVector2& p_childTopLeft,
		const OvMaths::FVector2& p_childSize,
		const OvMaths::FVector2& p_layoutSize
	)
	{
		const float centerX = p_childTopLeft.x + p_childSize.x * 0.5f;
		const float centerY = p_childTopLeft.y + p_childSize.y * 0.5f;

		return {
			centerX - p_layoutSize.x * 0.5f,
			p_layoutSize.y * 0.5f - centerY
		};
	}

	OvMaths::FVector2 GetLayoutPivotOffset(const OvCore::ECS::Actor& p_owner, const OvMaths::FVector2& p_layoutSize)
	{
		const auto& pivot = p_owner.transform.GetUIPivot();
		const auto halfSize = p_layoutSize * 0.5f;

		return {
			-pivot.x * halfSize.x,
			pivot.y * halfSize.y
		};
	}

	std::vector<OvCore::ECS::Components::UI::CLayoutGroup::ChildOffset> ResolveChildOffsets(
		const std::vector<LayoutChild>& p_children,
		const OvMaths::FVector2& p_layoutSize,
		OvCore::ECS::Components::UI::CLayoutGroup::EDirection p_direction,
		uint16_t p_spacing,
		Clay_Padding p_padding,
		OvCore::ECS::Components::UI::CLayoutGroup::EHorizontalAlignment p_horizontalAlignment,
		OvCore::ECS::Components::UI::CLayoutGroup::EVerticalAlignment p_verticalAlignment,
		const OvMaths::FVector2& p_layoutPivotOffset
	)
	{
		std::vector<OvCore::ECS::Components::UI::CLayoutGroup::ChildOffset> offsets;
		offsets.reserve(p_children.size());

		const float spacing = static_cast<float>(p_spacing);
		const float availableWidth = std::max(0.0f, p_layoutSize.x - p_padding.left - p_padding.right);
		const float availableHeight = std::max(0.0f, p_layoutSize.y - p_padding.top - p_padding.bottom);
		const OvMaths::FVector2 contentSize = GetChildrenContentSize(p_children, p_direction, spacing);

		if (p_direction == OvCore::ECS::Components::UI::CLayoutGroup::EDirection::HORIZONTAL)
		{
			float childLeft = static_cast<float>(p_padding.left) + GetHorizontalAlignmentOffset(p_horizontalAlignment, availableWidth, contentSize.x);

			for (const auto& child : p_children)
			{
				const float childTop = static_cast<float>(p_padding.top) + GetVerticalAlignmentOffset(p_verticalAlignment, availableHeight, child.size.y);
				const OvMaths::FVector2 offset = ToChildOffset({ childLeft, childTop }, child.size, p_layoutSize) + p_layoutPivotOffset;
				offsets.emplace_back(child.actor, offset);
				childLeft += child.size.x + spacing;
			}
		}
		else
		{
			float childTop = static_cast<float>(p_padding.top) + GetVerticalAlignmentOffset(p_verticalAlignment, availableHeight, contentSize.y);

			for (const auto& child : p_children)
			{
				const float childLeft = static_cast<float>(p_padding.left) + GetHorizontalAlignmentOffset(p_horizontalAlignment, availableWidth, child.size.x);
				const OvMaths::FVector2 offset = ToChildOffset({ childLeft, childTop }, child.size, p_layoutSize) + p_layoutPivotOffset;
				offsets.emplace_back(child.actor, offset);
				childTop += child.size.y + spacing;
			}
		}

		return offsets;
	}
}

OvCore::ECS::Components::UI::CLayoutGroup::CLayoutGroup(ECS::Actor& p_owner) :
AComponent(p_owner)
{
	owner.transform.EnableUIData();
}

std::string OvCore::ECS::Components::UI::CLayoutGroup::GetName()
{
	return "Layout Group";
}

std::string OvCore::ECS::Components::UI::CLayoutGroup::GetTypeName()
{
	return std::string{ComponentTraits<CLayoutGroup>::Name};
}

void OvCore::ECS::Components::UI::CLayoutGroup::SetDirection(EDirection p_direction)
{
	m_direction = ToDirection(static_cast<int>(p_direction));
}

OvCore::ECS::Components::UI::CLayoutGroup::EDirection OvCore::ECS::Components::UI::CLayoutGroup::GetDirection() const
{
	return m_direction;
}

void OvCore::ECS::Components::UI::CLayoutGroup::SetSpacing(float p_spacing)
{
	m_spacing = ClampSpacing(p_spacing, m_spacing);
}

float OvCore::ECS::Components::UI::CLayoutGroup::GetSpacing() const
{
	return m_spacing;
}

OvMaths::FVector2 OvCore::ECS::Components::UI::CLayoutGroup::GetComputedSize() const
{
	const auto spacing = ToClaySpacing(m_spacing);
	const auto padding = ToClayPadding(m_padding);
	return GetLayoutSize(
		owner,
		CollectLayoutChildren(owner),
		m_direction,
		static_cast<float>(spacing),
		padding
	);
}

void OvCore::ECS::Components::UI::CLayoutGroup::SetPadding(const OvMaths::FVector4& p_padding)
{
	m_padding.x = ClampPadding(p_padding.x, m_padding.x);
	m_padding.y = ClampPadding(p_padding.y, m_padding.y);
	m_padding.z = ClampPadding(p_padding.z, m_padding.z);
	m_padding.w = ClampPadding(p_padding.w, m_padding.w);
}

const OvMaths::FVector4& OvCore::ECS::Components::UI::CLayoutGroup::GetPadding() const
{
	return m_padding;
}

void OvCore::ECS::Components::UI::CLayoutGroup::SetHorizontalAlignment(EHorizontalAlignment p_alignment)
{
	m_horizontalAlignment = ToHorizontalAlignment(static_cast<int>(p_alignment));
}

OvCore::ECS::Components::UI::CLayoutGroup::EHorizontalAlignment OvCore::ECS::Components::UI::CLayoutGroup::GetHorizontalAlignment() const
{
	return m_horizontalAlignment;
}

void OvCore::ECS::Components::UI::CLayoutGroup::SetVerticalAlignment(EVerticalAlignment p_alignment)
{
	m_verticalAlignment = ToVerticalAlignment(static_cast<int>(p_alignment));
}

OvCore::ECS::Components::UI::CLayoutGroup::EVerticalAlignment OvCore::ECS::Components::UI::CLayoutGroup::GetVerticalAlignment() const
{
	return m_verticalAlignment;
}

void OvCore::ECS::Components::UI::CLayoutGroup::SetControlChildrenWidth(bool p_controlChildrenWidth)
{
	m_controlChildrenWidth = p_controlChildrenWidth;
}

bool OvCore::ECS::Components::UI::CLayoutGroup::GetControlChildrenWidth() const
{
	return m_controlChildrenWidth;
}

void OvCore::ECS::Components::UI::CLayoutGroup::SetControlChildrenHeight(bool p_controlChildrenHeight)
{
	m_controlChildrenHeight = p_controlChildrenHeight;
}

bool OvCore::ECS::Components::UI::CLayoutGroup::GetControlChildrenHeight() const
{
	return m_controlChildrenHeight;
}

void OvCore::ECS::Components::UI::CLayoutGroup::SetForceExpandWidth(bool p_forceExpandWidth)
{
	m_forceExpandWidth = p_forceExpandWidth;
}

bool OvCore::ECS::Components::UI::CLayoutGroup::GetForceExpandWidth() const
{
	return m_forceExpandWidth;
}

void OvCore::ECS::Components::UI::CLayoutGroup::SetForceExpandHeight(bool p_forceExpandHeight)
{
	m_forceExpandHeight = p_forceExpandHeight;
}

bool OvCore::ECS::Components::UI::CLayoutGroup::GetForceExpandHeight() const
{
	return m_forceExpandHeight;
}

bool OvCore::ECS::Components::UI::CLayoutGroup::IsDirectionEditable() const
{
	return true;
}

OvMaths::FVector2 OvCore::ECS::Components::UI::CLayoutGroup::GetChildOffset(const ECS::Actor& p_child) const
{
	if (p_child.GetParent() != &owner)
	{
		return OvMaths::FVector2::Zero;
	}

	for (const auto& [child, offset] : GetChildOffsets())
	{
		if (child == &p_child)
		{
			return offset;
		}
	}

	return OvMaths::FVector2::Zero;
}

std::vector<OvCore::ECS::Components::UI::CLayoutGroup::ChildOffset> OvCore::ECS::Components::UI::CLayoutGroup::GetChildOffsets() const
{
	auto layoutChildren = CollectLayoutChildren(owner);

	if (layoutChildren.empty())
	{
		return {};
	}

	const auto spacing = ToClaySpacing(m_spacing);
	const auto padding = ToClayPadding(m_padding);
	const auto layoutSize = GetLayoutSize(owner, layoutChildren, m_direction, static_cast<float>(spacing), padding);
	ApplyControlledChildrenSizing(
		layoutChildren,
		m_direction,
		m_controlChildrenWidth,
		m_controlChildrenHeight,
		m_forceExpandWidth,
		m_forceExpandHeight,
		layoutSize,
		spacing,
		padding
	);

	const auto layoutPivotOffset = GetLayoutPivotOffset(owner, layoutSize);
	return ResolveChildOffsets(
		layoutChildren,
		layoutSize,
		m_direction,
		spacing,
		padding,
		m_horizontalAlignment,
		m_verticalAlignment,
		layoutPivotOffset
	);
}

void OvCore::ECS::Components::UI::CLayoutGroup::ApplyControlledChildrenSizes()
{
	if (!m_controlChildrenWidth && !m_controlChildrenHeight && !m_forceExpandWidth && !m_forceExpandHeight)
	{
		return;
	}

	auto layoutChildren = CollectLayoutChildren(owner);
	if (layoutChildren.empty())
	{
		return;
	}

	const auto spacing = ToClaySpacing(m_spacing);
	const auto padding = ToClayPadding(m_padding);
	const auto layoutSize = GetLayoutSize(owner, layoutChildren, m_direction, static_cast<float>(spacing), padding);

	ApplyControlledChildrenSizing(
		layoutChildren,
		m_direction,
		m_controlChildrenWidth,
		m_controlChildrenHeight,
		m_forceExpandWidth,
		m_forceExpandHeight,
		layoutSize,
		spacing,
		padding
	);
	ApplyControlledSizeToChildren(
		layoutChildren,
		m_controlChildrenWidth,
		m_controlChildrenHeight,
		m_forceExpandWidth,
		m_forceExpandHeight
	);
}

void OvCore::ECS::Components::UI::CLayoutGroup::OnSerialize(tinyxml2::XMLDocument& p_doc, tinyxml2::XMLNode* p_node)
{
	Helpers::Serializer::SerializeInt(p_doc, p_node, "direction", static_cast<int>(m_direction));
	Helpers::Serializer::SerializeFloat(p_doc, p_node, "spacing", m_spacing);
	Helpers::Serializer::SerializeVec4(p_doc, p_node, "padding", m_padding);
	Helpers::Serializer::SerializeInt(p_doc, p_node, "horizontal_alignment", static_cast<int>(m_horizontalAlignment));
	Helpers::Serializer::SerializeInt(p_doc, p_node, "vertical_alignment", static_cast<int>(m_verticalAlignment));
	Helpers::Serializer::SerializeBoolean(p_doc, p_node, "control_children_width", m_controlChildrenWidth);
	Helpers::Serializer::SerializeBoolean(p_doc, p_node, "control_children_height", m_controlChildrenHeight);
	Helpers::Serializer::SerializeBoolean(p_doc, p_node, "force_expand_width", m_forceExpandWidth);
	Helpers::Serializer::SerializeBoolean(p_doc, p_node, "force_expand_height", m_forceExpandHeight);
}

void OvCore::ECS::Components::UI::CLayoutGroup::OnDeserialize(tinyxml2::XMLDocument& p_doc, tinyxml2::XMLNode* p_node)
{
	if (p_node->FirstChildElement("direction"))
	{
		auto direction = static_cast<int>(m_direction);
		Helpers::Serializer::DeserializeInt(p_doc, p_node, "direction", direction);
		SetDirection(ToDirection(direction));
	}

	if (p_node->FirstChildElement("spacing"))
	{
		auto spacing = m_spacing;
		Helpers::Serializer::DeserializeFloat(p_doc, p_node, "spacing", spacing);
		SetSpacing(spacing);
	}

	if (p_node->FirstChildElement("padding"))
	{
		auto padding = m_padding;
		Helpers::Serializer::DeserializeVec4(p_doc, p_node, "padding", padding);
		SetPadding(padding);
	}

	if (p_node->FirstChildElement("horizontal_alignment"))
	{
		auto horizontalAlignment = static_cast<int>(m_horizontalAlignment);
		Helpers::Serializer::DeserializeInt(p_doc, p_node, "horizontal_alignment", horizontalAlignment);
		SetHorizontalAlignment(ToHorizontalAlignment(horizontalAlignment));
	}

	if (p_node->FirstChildElement("vertical_alignment"))
	{
		auto verticalAlignment = static_cast<int>(m_verticalAlignment);
		Helpers::Serializer::DeserializeInt(p_doc, p_node, "vertical_alignment", verticalAlignment);
		SetVerticalAlignment(ToVerticalAlignment(verticalAlignment));
	}

	if (p_node->FirstChildElement("control_children_width"))
	{
		auto controlChildrenWidth = m_controlChildrenWidth;
		Helpers::Serializer::DeserializeBoolean(p_doc, p_node, "control_children_width", controlChildrenWidth);
		SetControlChildrenWidth(controlChildrenWidth);
	}

	if (p_node->FirstChildElement("control_children_height"))
	{
		auto controlChildrenHeight = m_controlChildrenHeight;
		Helpers::Serializer::DeserializeBoolean(p_doc, p_node, "control_children_height", controlChildrenHeight);
		SetControlChildrenHeight(controlChildrenHeight);
	}

	if (p_node->FirstChildElement("force_expand_width"))
	{
		auto forceExpandWidth = m_forceExpandWidth;
		Helpers::Serializer::DeserializeBoolean(p_doc, p_node, "force_expand_width", forceExpandWidth);
		SetForceExpandWidth(forceExpandWidth);
	}

	if (p_node->FirstChildElement("force_expand_height"))
	{
		auto forceExpandHeight = m_forceExpandHeight;
		Helpers::Serializer::DeserializeBoolean(p_doc, p_node, "force_expand_height", forceExpandHeight);
		SetForceExpandHeight(forceExpandHeight);
	}
}

void OvCore::ECS::Components::UI::CLayoutGroup::OnInspector(OvUI::Internal::WidgetContainer& p_root)
{
	if (IsDirectionEditable())
	{
		Helpers::GUIDrawer::CreateTitle(p_root, "Direction");
		auto& direction = p_root.CreateWidget<OvUI::Widgets::Selection::ComboBox>(static_cast<int>(GetDirection()));
		direction.choices.emplace(static_cast<int>(EDirection::HORIZONTAL), "Horizontal");
		direction.choices.emplace(static_cast<int>(EDirection::VERTICAL), "Vertical");
		direction.ValueChangedEvent += [this](int p_choice)
		{
			SetDirection(ToDirection(p_choice));
		};
	}

	Helpers::GUIDrawer::CreateTitle(p_root, "Horizontal Alignment");
	auto& horizontalAlignment = p_root.CreateWidget<OvUI::Widgets::Selection::ComboBox>(static_cast<int>(GetHorizontalAlignment()));
	horizontalAlignment.choices.emplace(static_cast<int>(EHorizontalAlignment::LEFT), "Left");
	horizontalAlignment.choices.emplace(static_cast<int>(EHorizontalAlignment::CENTER), "Center");
	horizontalAlignment.choices.emplace(static_cast<int>(EHorizontalAlignment::RIGHT), "Right");
	horizontalAlignment.ValueChangedEvent += [this](int p_choice)
	{
		SetHorizontalAlignment(ToHorizontalAlignment(p_choice));
	};

	Helpers::GUIDrawer::CreateTitle(p_root, "Vertical Alignment");
	auto& verticalAlignment = p_root.CreateWidget<OvUI::Widgets::Selection::ComboBox>(static_cast<int>(GetVerticalAlignment()));
	verticalAlignment.choices.emplace(static_cast<int>(EVerticalAlignment::TOP), "Top");
	verticalAlignment.choices.emplace(static_cast<int>(EVerticalAlignment::CENTER), "Center");
	verticalAlignment.choices.emplace(static_cast<int>(EVerticalAlignment::BOTTOM), "Bottom");
	verticalAlignment.ValueChangedEvent += [this](int p_choice)
	{
		SetVerticalAlignment(ToVerticalAlignment(p_choice));
	};

	Helpers::GUIDrawer::DrawScalar<float>(
		p_root,
		"Spacing",
		std::bind(&CLayoutGroup::GetSpacing, this),
		std::bind(&CLayoutGroup::SetSpacing, this, std::placeholders::_1),
		1.0f,
		kMinimumSpacing,
		kMaximumSpacing
	);

	Helpers::GUIDrawer::DrawVec4(
		p_root,
		"Padding",
		[this]() { return GetPadding(); },
		[this](OvMaths::FVector4 p_value) { SetPadding(p_value); },
		1.0f,
		kMinimumPadding,
		kMaximumPadding
	);

	Helpers::GUIDrawer::DrawBoolean(
		p_root,
		"Control Children Width",
		[this]() { return GetControlChildrenWidth(); },
		[this](bool p_value) { SetControlChildrenWidth(p_value); }
	);

	Helpers::GUIDrawer::DrawBoolean(
		p_root,
		"Control Children Height",
		[this]() { return GetControlChildrenHeight(); },
		[this](bool p_value) { SetControlChildrenHeight(p_value); }
	);

	Helpers::GUIDrawer::DrawBoolean(
		p_root,
		"Force Expand Width",
		[this]() { return GetForceExpandWidth(); },
		[this](bool p_value) { SetForceExpandWidth(p_value); }
	);

	Helpers::GUIDrawer::DrawBoolean(
		p_root,
		"Force Expand Height",
		[this]() { return GetForceExpandHeight(); },
		[this](bool p_value) { SetForceExpandHeight(p_value); }
	);
}
