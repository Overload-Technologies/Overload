/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include <OvCore/ECS/Components/UI/CHorizontalLayout.h>

OvCore::ECS::Components::UI::CHorizontalLayout::CHorizontalLayout(ECS::Actor& p_owner) :
	CLayoutGroup(p_owner)
{
	SetDirection(EDirection::HORIZONTAL);
}

std::string OvCore::ECS::Components::UI::CHorizontalLayout::GetName()
{
	return "Horizontal Layout";
}

std::string OvCore::ECS::Components::UI::CHorizontalLayout::GetTypeName()
{
	return std::string{ ComponentTraits<CHorizontalLayout>::Name };
}

void OvCore::ECS::Components::UI::CHorizontalLayout::SetDirection(EDirection)
{
	CLayoutGroup::SetDirection(EDirection::HORIZONTAL);
}

bool OvCore::ECS::Components::UI::CHorizontalLayout::IsDirectionEditable() const
{
	return false;
}
