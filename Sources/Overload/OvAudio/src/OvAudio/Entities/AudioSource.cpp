/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include <soloud.h>
#include <soloud_wav.h>

#include <OvAudio/Core/AudioEngine.h>
#include <OvAudio/Entities/AudioSource.h>

OvTools::Eventing::Event<OvAudio::Entities::AudioSource&> OvAudio::Entities::AudioSource::CreatedEvent;
OvTools::Eventing::Event<OvAudio::Entities::AudioSource&> OvAudio::Entities::AudioSource::DestroyedEvent;

OvAudio::Entities::AudioSource::AudioSource(Core::AudioEngine& p_engine, OvTools::Utils::OptRef<OvMaths::FTransform> p_transform) :
	m_engine(p_engine),
	m_transform(p_transform)
{
	CreatedEvent.Invoke(*this);
}

OvAudio::Entities::AudioSource::~AudioSource()
{
	Stop();
	DestroyedEvent.Invoke(*this);
}

const OvMaths::FTransform& OvAudio::Entities::AudioSource::GetTransform()
{
	return m_transform;
}

void OvAudio::Entities::AudioSource::ApplySourceSettingsToTrackedSound()
{
	m_instance->SetVolume(m_volume);
	m_instance->SetPan(m_pan);
	m_instance->SetLooped(m_looped);
	m_instance->SetPitch(m_pitch);
	m_instance->SetAttenuationThreshold(m_attenuationThreshold);
}

void OvAudio::Entities::AudioSource::SetSpatial(bool p_value)
{
	m_spatial = p_value;
}

void OvAudio::Entities::AudioSource::SetAttenuationThreshold(float p_distance)
{
	m_attenuationThreshold = p_distance;

	if (IsPlaying())
	{
		m_instance->SetAttenuationThreshold(m_attenuationThreshold);
	}
}

void OvAudio::Entities::AudioSource::SetVolume(float p_volume)
{
	m_volume = p_volume;

	if (IsPlaying())
	{
		m_instance->SetVolume(m_volume);
	}
}

void OvAudio::Entities::AudioSource::SetPan(float p_pan)
{
	m_pan = p_pan;

	if (IsPlaying())
	{
		m_instance->SetPan(m_pan);
	}
}

void OvAudio::Entities::AudioSource::SetLooped(bool p_looped)
{
	m_looped = p_looped;

	if (IsPlaying())
	{
		m_instance->SetLooped(p_looped);
	}
}

void OvAudio::Entities::AudioSource::SetPitch(float p_pitch)
{
	m_pitch = p_pitch;

	if (IsPlaying())
	{
		// TODO: Check if we should clamp pitch like with irrklang?
		// m_trackedSound->GetTrack()->setPlaybackSpeed(p_pitch < 0.01f ? 0.01f : p_pitch);
		m_instance->SetPitch(p_pitch);
	}
}

bool OvAudio::Entities::AudioSource::IsPlaying() const
{
	return m_instance && m_instance->IsPlaying();
}

OvTools::Utils::OptRef<OvAudio::Data::SoundInstance> OvAudio::Entities::AudioSource::GetSoundInstance() const
{
	return m_instance;
}

bool OvAudio::Entities::AudioSource::IsSpatial() const
{
	return m_spatial;
}

float OvAudio::Entities::AudioSource::GetAttenuationThreshold() const
{
	return m_attenuationThreshold;
}

float OvAudio::Entities::AudioSource::GetVolume() const
{
	return m_volume;
}

float OvAudio::Entities::AudioSource::GetPan() const
{
	return m_pan;
}

bool OvAudio::Entities::AudioSource::IsLooped() const
{
	return m_looped;
}

float OvAudio::Entities::AudioSource::GetPitch() const
{
	return m_pitch;
}

void OvAudio::Entities::AudioSource::Play(const Resources::Sound& p_sound)
{
	// Stops and destroys the previous sound (If any)
	Stop();

	// Play a sound and stores the instance
	if (m_spatial)
	{
		m_instance = m_engine.Play(p_sound, m_transform->GetWorldPosition());
	}
	else
	{
		m_instance = m_engine.Play(p_sound);
	}

	// Overrides sound settings with whatever is set in the AudioSource
	m_instance->SetVolume(m_volume);
	m_instance->SetPan(m_pan);
	m_instance->SetLooped(m_looped);
	m_instance->SetPitch(m_pitch);
	m_instance->SetAttenuationThreshold(m_attenuationThreshold);

	// TODO: Remove that? Or expose this setting?
	m_engine.GetBackend().set3dSourceAttenuation(m_instance->GetHandle(), SoLoud::AudioSource::ATTENUATION_MODELS::EXPONENTIAL_DISTANCE, 1.0f);

}

void OvAudio::Entities::AudioSource::Resume()
{
	if (IsPlaying())
	{
		m_instance->SetPause(false);
	}
}

void OvAudio::Entities::AudioSource::Pause()
{
	if (IsPlaying())
	{
		m_instance->SetPause(true);
	}
}

void OvAudio::Entities::AudioSource::Stop()
{
	if (IsPlaying())
	{
		m_instance->Stop();
	}
}
