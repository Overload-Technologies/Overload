/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include <algorithm>
#include <cstddef>
#include <cmath>
#include <functional>
#include <limits>
#include <optional>
#include <vector>

#include <tinyxml2.h>

#include <OvCore/ECS/Actor.h>
#include <OvCore/ECS/Components/UI/CImage.h>
#include <OvCore/ECS/Components/UI/ClayLayoutSolver.h>
#include <OvCore/ECS/Components/UI/CLayoutGroup.h>
#include <OvCore/ECS/Components/UI/CText.h>
#include <OvCore/ECS/Components/UI/UITransformResolver.h>
#include <OvCore/Helpers/GUIDrawer.h>
#include <OvCore/Helpers/Serializer.h>

#include <OvUI/Widgets/Selection/ComboBox.h>

namespace
{
	constexpr float kMinimumSpacing = 0.0f;
	constexpr float kMinimumPadding = 0.0f;
	constexpr float kMaximumSpacing = static_cast<float>(std::numeric_limits<uint16_t>::max());
	constexpr float kMaximumPadding = static_cast<float>(std::numeric_limits<uint16_t>::max());

	float ClampSpacing(float p_value, float p_fallback)
	{
		return std::isfinite(p_value) ? std::clamp(p_value, kMinimumSpacing, kMaximumSpacing) : p_fallback;
	}

	float ClampPadding(float p_value, float p_fallback)
	{
		return std::isfinite(p_value) ? std::clamp(p_value, kMinimumPadding, kMaximumPadding) : p_fallback;
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
		else if (const auto* layout = p_child.GetComponent<OvCore::ECS::Components::UI::CLayoutGroup>(); layout)
		{
			elementSize = layout->GetComputedSize();
			hasElementSize = true;
		}

		if (OvCore::ECS::Components::UI::UITransformResolver::HasActiveUIData(p_child))
		{
			const auto size = OvCore::ECS::Components::UI::UITransformResolver::GetEffectiveSize(p_child.transform, elementSize);
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

	OvCore::ECS::Components::UI::ClayLayoutSettings CreateLayoutSettings(
		const OvCore::ECS::Components::UI::CLayoutGroup& p_layout,
		const OvCore::ECS::Actor& p_owner
	)
	{
		return {
			.direction = p_layout.GetDirection(),
			.spacing = p_layout.GetSpacing(),
			.padding = p_layout.GetPadding(),
			.horizontalAlignment = p_layout.GetHorizontalAlignment(),
			.verticalAlignment = p_layout.GetVerticalAlignment(),
			.controlChildrenWidth = p_layout.GetControlChildrenWidth(),
			.controlChildrenHeight = p_layout.GetControlChildrenHeight(),
			.forceExpandWidth = p_layout.GetForceExpandWidth(),
			.forceExpandHeight = p_layout.GetForceExpandHeight(),
			.containerSize = p_owner.transform.GetUISize(),
			.pivot = p_owner.transform.GetUIPivot()
		};
	}
}

OvCore::ECS::Components::UI::CLayoutGroup::CLayoutGroup(ECS::Actor& p_owner) :
AComponent(p_owner)
{
	m_layoutSolverContext = std::make_unique<ClayLayoutSolverContext>();
	owner.transform.EnableUIData();
}

OvCore::ECS::Components::UI::CLayoutGroup::~CLayoutGroup() = default;

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
	InvalidateLayoutCache();
}

OvCore::ECS::Components::UI::CLayoutGroup::EDirection OvCore::ECS::Components::UI::CLayoutGroup::GetDirection() const
{
	return m_direction;
}

void OvCore::ECS::Components::UI::CLayoutGroup::SetSpacing(float p_spacing)
{
	m_spacing = ClampSpacing(p_spacing, m_spacing);
	InvalidateLayoutCache();
}

float OvCore::ECS::Components::UI::CLayoutGroup::GetSpacing() const
{
	return m_spacing;
}

OvMaths::FVector2 OvCore::ECS::Components::UI::CLayoutGroup::GetComputedSize() const
{
	return GetResolvedLayout().size;
}

void OvCore::ECS::Components::UI::CLayoutGroup::SetPadding(const OvMaths::FVector4& p_padding)
{
	m_padding.x = ClampPadding(p_padding.x, m_padding.x);
	m_padding.y = ClampPadding(p_padding.y, m_padding.y);
	m_padding.z = ClampPadding(p_padding.z, m_padding.z);
	m_padding.w = ClampPadding(p_padding.w, m_padding.w);
	InvalidateLayoutCache();
}

const OvMaths::FVector4& OvCore::ECS::Components::UI::CLayoutGroup::GetPadding() const
{
	return m_padding;
}

void OvCore::ECS::Components::UI::CLayoutGroup::SetHorizontalAlignment(EHorizontalAlignment p_alignment)
{
	m_horizontalAlignment = ToHorizontalAlignment(static_cast<int>(p_alignment));
	InvalidateLayoutCache();
}

OvCore::ECS::Components::UI::CLayoutGroup::EHorizontalAlignment OvCore::ECS::Components::UI::CLayoutGroup::GetHorizontalAlignment() const
{
	return m_horizontalAlignment;
}

void OvCore::ECS::Components::UI::CLayoutGroup::SetVerticalAlignment(EVerticalAlignment p_alignment)
{
	m_verticalAlignment = ToVerticalAlignment(static_cast<int>(p_alignment));
	InvalidateLayoutCache();
}

OvCore::ECS::Components::UI::CLayoutGroup::EVerticalAlignment OvCore::ECS::Components::UI::CLayoutGroup::GetVerticalAlignment() const
{
	return m_verticalAlignment;
}

void OvCore::ECS::Components::UI::CLayoutGroup::SetControlChildrenWidth(bool p_controlChildrenWidth)
{
	m_controlChildrenWidth = p_controlChildrenWidth;
	InvalidateLayoutCache();
}

bool OvCore::ECS::Components::UI::CLayoutGroup::GetControlChildrenWidth() const
{
	return m_controlChildrenWidth;
}

void OvCore::ECS::Components::UI::CLayoutGroup::SetControlChildrenHeight(bool p_controlChildrenHeight)
{
	m_controlChildrenHeight = p_controlChildrenHeight;
	InvalidateLayoutCache();
}

bool OvCore::ECS::Components::UI::CLayoutGroup::GetControlChildrenHeight() const
{
	return m_controlChildrenHeight;
}

void OvCore::ECS::Components::UI::CLayoutGroup::SetForceExpandWidth(bool p_forceExpandWidth)
{
	m_forceExpandWidth = p_forceExpandWidth;
	InvalidateLayoutCache();
}

bool OvCore::ECS::Components::UI::CLayoutGroup::GetForceExpandWidth() const
{
	return m_forceExpandWidth;
}

void OvCore::ECS::Components::UI::CLayoutGroup::SetForceExpandHeight(bool p_forceExpandHeight)
{
	m_forceExpandHeight = p_forceExpandHeight;
	InvalidateLayoutCache();
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

	if (const auto childLayout = GetChildLayout(p_child); childLayout && childLayout->valid)
	{
		return childLayout->offset;
	}

	return OvMaths::FVector2::Zero;
}

std::optional<OvCore::ECS::Components::UI::CLayoutGroup::ChildLayout> OvCore::ECS::Components::UI::CLayoutGroup::GetChildLayout(const ECS::Actor& p_child) const
{
	if (p_child.GetParent() != &owner)
	{
		return std::nullopt;
	}

	for (const auto& childLayout : GetResolvedLayout().children)
	{
		if (childLayout.actor == &p_child)
		{
			return childLayout;
		}
	}

	return std::nullopt;
}

std::vector<OvCore::ECS::Components::UI::CLayoutGroup::ChildOffset> OvCore::ECS::Components::UI::CLayoutGroup::GetChildOffsets() const
{
	const auto& childLayouts = GetResolvedLayout().children;

	std::vector<ChildOffset> offsets;
	offsets.reserve(childLayouts.size());

	for (const auto& childLayout : childLayouts)
	{
		offsets.emplace_back(childLayout.actor, childLayout.offset);
	}

	return offsets;
}

std::vector<OvCore::ECS::Components::UI::CLayoutGroup::ChildLayout> OvCore::ECS::Components::UI::CLayoutGroup::GetChildLayouts() const
{
	return GetResolvedLayout().children;
}

OvCore::ECS::Components::UI::CLayoutGroup::LayoutCacheInput OvCore::ECS::Components::UI::CLayoutGroup::BuildLayoutCacheInput() const
{
	LayoutCacheInput input;
	const auto settings = CreateLayoutSettings(*this, owner);

	input.signature = {
		.direction = settings.direction,
		.spacing = settings.spacing,
		.padding = settings.padding,
		.horizontalAlignment = settings.horizontalAlignment,
		.verticalAlignment = settings.verticalAlignment,
		.controlChildrenWidth = settings.controlChildrenWidth,
		.controlChildrenHeight = settings.controlChildrenHeight,
		.forceExpandWidth = settings.forceExpandWidth,
		.forceExpandHeight = settings.forceExpandHeight,
		.containerSize = settings.containerSize,
		.pivot = settings.pivot
	};

	const auto& children = owner.GetChildren();
	input.children.reserve(children.size());
	input.signature.children.reserve(children.size());

	for (const auto child : children)
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

		input.children.push_back({
			.actor = child,
			.preferredSize = size.value()
		});
		input.signature.children.push_back({
			.actor = child,
			.preferredSize = size.value()
		});
	}

	return input;
}

const OvCore::ECS::Components::UI::CLayoutGroup::LayoutCache& OvCore::ECS::Components::UI::CLayoutGroup::GetResolvedLayout() const
{
	const auto input = BuildLayoutCacheInput();

	if (m_layoutCache.valid && HasSameLayoutSignature(m_layoutCache.signature, input.signature))
	{
		return m_layoutCache;
	}

	std::vector<ClayLayoutChildInput> layoutChildren;
	layoutChildren.reserve(input.children.size());

	for (const auto& child : input.children)
	{
		layoutChildren.push_back({
			.actor = child.actor,
			.preferredSize = child.preferredSize
		});
	}

	const auto layoutResult = ClayLayoutSolver::Solve(*m_layoutSolverContext, CreateLayoutSettings(*this, owner), layoutChildren);

	m_layoutCache.valid = true;
	m_layoutCache.signature = input.signature;
	m_layoutCache.size = layoutResult.size;
	m_layoutCache.children.clear();
	m_layoutCache.children.reserve(layoutResult.children.size());

	for (const auto& child : layoutResult.children)
	{
		m_layoutCache.children.push_back({
			.actor = child.actor,
			.offset = child.offset,
			.size = child.size,
			.hasDirectWidth = GetControlChildrenWidth() || GetForceExpandWidth(),
			.hasDirectHeight = GetControlChildrenHeight() || GetForceExpandHeight(),
			.valid = child.valid
		});
	}

	return m_layoutCache;
}

void OvCore::ECS::Components::UI::CLayoutGroup::InvalidateLayoutCache() const
{
	m_layoutCache.valid = false;
	m_layoutCache.children.clear();
}

bool OvCore::ECS::Components::UI::CLayoutGroup::HasSameLayoutSignature(
	const OvCore::ECS::Components::UI::CLayoutGroup::LayoutCacheSignature& p_lhs,
	const OvCore::ECS::Components::UI::CLayoutGroup::LayoutCacheSignature& p_rhs
)
{
	if (
		p_lhs.direction != p_rhs.direction ||
		p_lhs.spacing != p_rhs.spacing ||
		p_lhs.padding.x != p_rhs.padding.x ||
		p_lhs.padding.y != p_rhs.padding.y ||
		p_lhs.padding.z != p_rhs.padding.z ||
		p_lhs.padding.w != p_rhs.padding.w ||
		p_lhs.horizontalAlignment != p_rhs.horizontalAlignment ||
		p_lhs.verticalAlignment != p_rhs.verticalAlignment ||
		p_lhs.controlChildrenWidth != p_rhs.controlChildrenWidth ||
		p_lhs.controlChildrenHeight != p_rhs.controlChildrenHeight ||
		p_lhs.forceExpandWidth != p_rhs.forceExpandWidth ||
		p_lhs.forceExpandHeight != p_rhs.forceExpandHeight ||
		p_lhs.containerSize.x != p_rhs.containerSize.x ||
		p_lhs.containerSize.y != p_rhs.containerSize.y ||
		p_lhs.pivot.x != p_rhs.pivot.x ||
		p_lhs.pivot.y != p_rhs.pivot.y ||
		p_lhs.children.size() != p_rhs.children.size()
	)
	{
		return false;
	}

	for (size_t i = 0; i < p_lhs.children.size(); ++i)
	{
		const auto& leftChild = p_lhs.children[i];
		const auto& rightChild = p_rhs.children[i];

		if (
			leftChild.actor != rightChild.actor ||
			leftChild.preferredSize.x != rightChild.preferredSize.x ||
			leftChild.preferredSize.y != rightChild.preferredSize.y
		)
		{
			return false;
		}
	}

	return true;
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
