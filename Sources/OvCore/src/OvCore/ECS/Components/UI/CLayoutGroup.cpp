/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include <algorithm>
#include <cstdint>
#include <cstddef>
#include <cmath>
#include <functional>
#include <limits>
#include <optional>
#include <unordered_map>
#include <vector>

#include <tinyxml2.h>

#include <OvCore/ECS/Actor.h>
#include <OvCore/ECS/Components/UI/ClayLayoutSolver.h>
#include <OvCore/ECS/Components/UI/CImage.h>
#include <OvCore/ECS/Components/UI/CLayoutGroup.h>
#include <OvCore/ECS/Components/UI/CText.h>
#include <OvCore/Helpers/GUIDrawer.h>
#include <OvCore/Helpers/Serializer.h>

#include <OvUI/Widgets/Selection/ComboBox.h>

namespace
{
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

	std::vector<OvCore::ECS::Components::UI::ClayLayoutChildInput> CollectLayoutChildren(OvCore::ECS::Actor& p_owner)
	{
		std::vector<OvCore::ECS::Components::UI::ClayLayoutChildInput> layoutChildren;

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

	OvCore::ECS::Actor* FindDirectChildByID(OvCore::ECS::Actor& p_owner, int64_t p_childID)
	{
		for (auto* child : p_owner.GetChildren())
		{
			if (child && child->GetID() == p_childID)
			{
				return child;
			}
		}

		return nullptr;
	}

	void CaptureDrivenChildSize(
		OvCore::ECS::Actor& p_child,
		OvCore::ECS::Components::UI::LayoutDrivenChildSize& p_drivenSize,
		bool p_appliesWidth,
		bool p_appliesHeight
	)
	{
		if (p_child.transform.HasActiveUIData() && !p_drivenSize.hasTransformSize)
		{
			p_drivenSize.transformSize = p_child.transform.GetUISize();
			p_drivenSize.hasTransformSize = true;
		}

		if (auto* image = p_child.GetComponent<OvCore::ECS::Components::UI::CImage>(); image && !p_drivenSize.hasImageSize)
		{
			p_drivenSize.imageSize = image->GetSize();
			p_drivenSize.hasImageSize = true;
		}

		if (p_appliesWidth)
		{
			p_drivenSize.width = true;
		}

		if (p_appliesHeight)
		{
			p_drivenSize.height = true;
		}
	}

	void ApplyControlledSizeToChildren(
		const std::vector<OvCore::ECS::Components::UI::ClayLayoutChildResult>& p_children,
		bool p_controlChildrenWidth,
		bool p_controlChildrenHeight,
		bool p_forceExpandWidth,
		bool p_forceExpandHeight,
		std::unordered_map<int64_t, OvCore::ECS::Components::UI::LayoutDrivenChildSize>& p_drivenChildSizes
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
			if (!child.actor || !child.valid)
			{
				continue;
			}

			auto& drivenSize = p_drivenChildSizes[child.actor->GetID()];
			CaptureDrivenChildSize(*child.actor, drivenSize, appliesWidth, appliesHeight);

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
	return ClayLayoutSolver::Solve(CreateLayoutSettings(*this, owner), CollectLayoutChildren(owner)).size;
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
	const bool wasDrivingWidth = m_controlChildrenWidth || m_forceExpandWidth;
	m_controlChildrenWidth = p_controlChildrenWidth;
	const bool isDrivingWidth = m_controlChildrenWidth || m_forceExpandWidth;

	if (wasDrivingWidth && !isDrivingWidth)
	{
		RestoreReleasedChildrenSizes(true, false);
	}
}

bool OvCore::ECS::Components::UI::CLayoutGroup::GetControlChildrenWidth() const
{
	return m_controlChildrenWidth;
}

void OvCore::ECS::Components::UI::CLayoutGroup::SetControlChildrenHeight(bool p_controlChildrenHeight)
{
	const bool wasDrivingHeight = m_controlChildrenHeight || m_forceExpandHeight;
	m_controlChildrenHeight = p_controlChildrenHeight;
	const bool isDrivingHeight = m_controlChildrenHeight || m_forceExpandHeight;

	if (wasDrivingHeight && !isDrivingHeight)
	{
		RestoreReleasedChildrenSizes(false, true);
	}
}

bool OvCore::ECS::Components::UI::CLayoutGroup::GetControlChildrenHeight() const
{
	return m_controlChildrenHeight;
}

void OvCore::ECS::Components::UI::CLayoutGroup::SetForceExpandWidth(bool p_forceExpandWidth)
{
	const bool wasDrivingWidth = m_controlChildrenWidth || m_forceExpandWidth;
	m_forceExpandWidth = p_forceExpandWidth;
	const bool isDrivingWidth = m_controlChildrenWidth || m_forceExpandWidth;

	if (wasDrivingWidth && !isDrivingWidth)
	{
		RestoreReleasedChildrenSizes(true, false);
	}
}

bool OvCore::ECS::Components::UI::CLayoutGroup::GetForceExpandWidth() const
{
	return m_forceExpandWidth;
}

void OvCore::ECS::Components::UI::CLayoutGroup::SetForceExpandHeight(bool p_forceExpandHeight)
{
	const bool wasDrivingHeight = m_controlChildrenHeight || m_forceExpandHeight;
	m_forceExpandHeight = p_forceExpandHeight;
	const bool isDrivingHeight = m_controlChildrenHeight || m_forceExpandHeight;

	if (wasDrivingHeight && !isDrivingHeight)
	{
		RestoreReleasedChildrenSizes(false, true);
	}
}

bool OvCore::ECS::Components::UI::CLayoutGroup::GetForceExpandHeight() const
{
	return m_forceExpandHeight;
}

bool OvCore::ECS::Components::UI::CLayoutGroup::IsDirectionEditable() const
{
	return true;
}

void OvCore::ECS::Components::UI::CLayoutGroup::RestoreReleasedChildrenSizes(bool p_restoreWidth, bool p_restoreHeight)
{
	if ((!p_restoreWidth && !p_restoreHeight) || m_drivenChildSizes.empty())
	{
		return;
	}

	for (auto it = m_drivenChildSizes.begin(); it != m_drivenChildSizes.end();)
	{
		auto& drivenSize = it->second;
		auto* child = FindDirectChildByID(owner, it->first);

		if (child)
		{
			if (drivenSize.hasImageSize)
			{
				if (auto* image = child->GetComponent<CImage>())
				{
					auto nextSize = image->GetSize();
					if (p_restoreWidth && drivenSize.width)
					{
						nextSize.x = drivenSize.imageSize.x;
					}
					if (p_restoreHeight && drivenSize.height)
					{
						nextSize.y = drivenSize.imageSize.y;
					}

					if (ShouldUpdateControlledSize(image->GetSize(), nextSize, p_restoreWidth, p_restoreHeight))
					{
						image->SetSize(nextSize);
					}
				}
			}

			if (drivenSize.hasTransformSize && child->transform.HasUIData())
			{
				auto nextSize = child->transform.GetUISize();
				if (p_restoreWidth && drivenSize.width)
				{
					nextSize.x = drivenSize.transformSize.x;
				}
				if (p_restoreHeight && drivenSize.height)
				{
					nextSize.y = drivenSize.transformSize.y;
				}

				if (ShouldUpdateControlledSize(child->transform.GetUISize(), nextSize, p_restoreWidth, p_restoreHeight))
				{
					child->transform.SetUISize(nextSize);
				}
			}
		}

		if (p_restoreWidth)
		{
			drivenSize.width = false;
		}
		if (p_restoreHeight)
		{
			drivenSize.height = false;
		}

		if (!drivenSize.width && !drivenSize.height)
		{
			it = m_drivenChildSizes.erase(it);
		}
		else
		{
			++it;
		}
	}
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
	const auto layoutChildren = CollectLayoutChildren(owner);

	if (layoutChildren.empty())
	{
		return {};
	}

	const auto layoutResult = ClayLayoutSolver::Solve(CreateLayoutSettings(*this, owner), layoutChildren);

	std::vector<ChildOffset> offsets;
	offsets.reserve(layoutResult.children.size());

	for (const auto& child : layoutResult.children)
	{
		offsets.emplace_back(child.actor, child.offset);
	}

	return offsets;
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

	const auto layoutResult = ClayLayoutSolver::Solve(CreateLayoutSettings(*this, owner), layoutChildren);
	ApplyControlledSizeToChildren(
		layoutResult.children,
		m_controlChildrenWidth,
		m_controlChildrenHeight,
		m_forceExpandWidth,
		m_forceExpandHeight,
		m_drivenChildSizes
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
