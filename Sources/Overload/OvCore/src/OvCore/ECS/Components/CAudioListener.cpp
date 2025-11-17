/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include <OvCore/ECS/Actor.h>
#include <OvCore/ECS/Components/CAudioListener.h>

OvCore::ECS::Components::CAudioListener::CAudioListener(ECS::Actor& p_owner) :
	AComponent(p_owner),
	m_audioListener(owner.transform.GetFTransform())
{
	m_audioListener.SetEnabled(false);
}

std::string OvCore::ECS::Components::CAudioListener::GetName()
{
	return "Audio Listener";
}

std::string OvCore::ECS::Components::CAudioListener::GetTypeName()
{
	return std::string{ComponentTraits<CAudioListener>::Name};
}

void OvCore::ECS::Components::CAudioListener::OnSerialize(tinyxml2::XMLDocument& p_doc, tinyxml2::XMLNode* p_node)
{
}

void OvCore::ECS::Components::CAudioListener::OnDeserialize(tinyxml2::XMLDocument& p_doc, tinyxml2::XMLNode* p_node)
{
}

void OvCore::ECS::Components::CAudioListener::OnInspector(OvUI::Internal::WidgetContainer& p_root)
{

}

void OvCore::ECS::Components::CAudioListener::OnEnable()
{
	m_audioListener.SetEnabled(true);
}

void OvCore::ECS::Components::CAudioListener::OnDisable()
{
	m_audioListener.SetEnabled(false);
}
