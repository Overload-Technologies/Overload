/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include <soloud.h>
#include <soloud_wav.h>

#include <OvAudio/Core/AudioEngine.h>
#include <OvAudio/Entities/AudioSource.h>
#include <OvDebug/Assertion.h>

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

bool OvAudio::Entities::AudioSource::HasSound() const
{
	return m_instance && m_instance->IsValid();
}

bool OvAudio::Entities::AudioSource::IsPlaying() const
{
	return HasSound() && !IsPaused();
}

void OvAudio::Entities::AudioSource::SetSpatial(bool p_value)
{
	m_spatial = p_value;
	// TODO: Cannot currently change the spatialization mode of a sound instance
}

void OvAudio::Entities::AudioSource::SetAttenuationThreshold(float p_distance)
{
	m_attenuationThreshold = p_distance;

	if (HasSound())
	{
		m_instance->SetAttenuationThreshold(m_attenuationThreshold);
	}
}

void OvAudio::Entities::AudioSource::SetVolume(float p_volume)
{
	m_volume = p_volume;

	if (HasSound())
	{
		m_instance->SetVolume(m_volume);
	}
}

void OvAudio::Entities::AudioSource::SetPan(float p_pan)
{
	m_pan = p_pan;

	if (HasSound())
	{
		m_instance->SetPan(m_pan);
	}
}

void OvAudio::Entities::AudioSource::SetLooped(bool p_looped)
{
	m_looped = p_looped;

	if (HasSound())
	{
		m_instance->SetLooped(p_looped);
	}
}

void OvAudio::Entities::AudioSource::SetPitch(float p_pitch)
{
	m_pitch = p_pitch;

	if (HasSound())
	{
		m_instance->SetPitch(p_pitch);
	}
}

bool OvAudio::Entities::AudioSource::IsPaused() const
{
	OVASSERT(HasSound(), "Cannot check if the sound is paused if no sound is currently being tracked");
	return m_instance->IsPaused();
}

std::weak_ptr<OvAudio::Data::SoundInstance> OvAudio::Entities::AudioSource::GetSoundInstance() const
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
	Stop();

	if (m_spatial)
	{
		m_instance = m_engine.Play3D(
			p_sound,
			m_transform->GetWorldPosition(),
			OvMaths::FVector3::Zero, // TODO: Add support for non-zero velocity
			m_volume,
			true
		);

		m_instance->SetAttenuationThreshold(m_attenuationThreshold);
		m_instance->SetAttenuationModel(
			Settings::EAttenuationModel::EXPONENTIAL_DISTANCE // TODO: Expose attenuation model
		);
	}
	else
	{
		m_instance = m_engine.Play2D(p_sound, m_pan, m_volume, true);
	}

	m_instance->SetLooped(m_looped);
	m_instance->SetPitch(m_pitch);
	m_instance->Play();
}

void OvAudio::Entities::AudioSource::Resume()
{
	if (HasSound())
	{
		m_instance->Play();
	}
}

void OvAudio::Entities::AudioSource::Pause()
{
	if (HasSound())
	{
		m_instance->Pause();
	}
}

void OvAudio::Entities::AudioSource::Stop()
{
	if (HasSound())
	{
		m_instance->Stop();
	}
}

void OvAudio::Entities::AudioSource::Update()
{
	if (HasSound() && m_instance->IsSpatial())
	{
		m_instance->SetSpatialParameters(
			m_transform->GetWorldPosition(),
			OvMaths::FVector3::Zero // TODO: Add support for non-zero velocity
		);
	}
}
