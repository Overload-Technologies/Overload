/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include <OvAudio/Entities/AudioListener.h>

OvTools::Eventing::Event<OvAudio::Entities::AudioListener&>	OvAudio::Entities::AudioListener::CreatedEvent;
OvTools::Eventing::Event<OvAudio::Entities::AudioListener&>	OvAudio::Entities::AudioListener::DestroyedEvent;

OvAudio::Entities::AudioListener::AudioListener(OvTools::Utils::OptRef<OvMaths::FTransform> p_transform) :
	m_transform(p_transform)
{
	CreatedEvent.Invoke(*this);
}

OvAudio::Entities::AudioListener::~AudioListener()
{
	DestroyedEvent.Invoke(*this);
}

OvMaths::FTransform& OvAudio::Entities::AudioListener::GetTransform()
{
	return m_transform;
}

void OvAudio::Entities::AudioListener::SetEnabled(bool p_enable)
{
	m_enabled = p_enable;
}

bool OvAudio::Entities::AudioListener::IsEnabled() const
{
	return m_enabled;
}
