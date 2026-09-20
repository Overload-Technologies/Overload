/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include <OvCore/ECS/Components/UI/CVerticalLayout.h>

OvCore::ECS::Components::UI::CVerticalLayout::CVerticalLayout(ECS::Actor& p_owner) :
	CLayoutGroup(p_owner)
{
	SetDirection(EDirection::VERTICAL);
}

std::string OvCore::ECS::Components::UI::CVerticalLayout::GetName()
{
	return "Vertical Layout";
}

std::string OvCore::ECS::Components::UI::CVerticalLayout::GetTypeName()
{
	return std::string{ ComponentTraits<CVerticalLayout>::Name };
}

void OvCore::ECS::Components::UI::CVerticalLayout::SetDirection(EDirection)
{
	CLayoutGroup::SetDirection(EDirection::VERTICAL);
}

bool OvCore::ECS::Components::UI::CVerticalLayout::IsDirectionEditable() const
{
	return false;
}
