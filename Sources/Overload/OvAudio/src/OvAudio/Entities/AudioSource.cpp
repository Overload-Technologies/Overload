/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include <soloud.h>

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
	DestroyedEvent.Invoke(*this);
	StopAndDestroyTrackedSound();
}

void OvAudio::Entities::AudioSource::UpdateTrackedSoundPosition()
{
	if (IsTrackingSound())
	{
		const auto worldPos = m_transform->GetWorldPosition();
		m_engine.GetBackend().set3dSourcePosition(m_handle.value(), worldPos.x, worldPos.y, worldPos.z);
	}
}

void OvAudio::Entities::AudioSource::ApplySourceSettingsToTrackedSound()
{
	// TODO: assert if no handle
	auto& backend = m_engine.GetBackend();
	backend.setVolume(m_handle.value(), m_volume);
	backend.setPan(m_handle.value(), m_pan * -1.0f); // TODO: Check if pan should be negative here (wasn't with irrklang)
	backend.setLooping(m_handle.value(), m_looped);
	backend.setRelativePlaySpeed(m_handle.value(), m_pitch); // TODO: should we clamp pitch like with irrklang?
	backend.set3dSourceMinMaxDistance(m_handle.value(), m_attenuationThreshold, std::numeric_limits<float>::infinity());
}

void OvAudio::Entities::AudioSource::SetSpatial(bool p_value)
{
	m_spatial = p_value;
}

void OvAudio::Entities::AudioSource::SetAttenuationThreshold(float p_distance)
{
	m_attenuationThreshold = p_distance;

	if (IsTrackingSound())
	{
		m_engine.GetBackend().set3dSourceMinMaxDistance(m_handle.value(), m_attenuationThreshold, std::numeric_limits<float>::infinity());
	}
}

void OvAudio::Entities::AudioSource::SetVolume(float p_volume)
{
	m_volume = p_volume;

	if (IsTrackingSound())
	{
		m_engine.GetBackend().setVolume(m_handle.value(), m_volume);
	}
}

void OvAudio::Entities::AudioSource::SetPan(float p_pan)
{
	m_pan = p_pan;

	if (IsTrackingSound())
	{
		m_engine.GetBackend().setPan(m_handle.value(), m_pan * -1.0f); // TODO: Check if pan should be negative here (wasn't with irrklang)
	}
}

void OvAudio::Entities::AudioSource::SetLooped(bool p_looped)
{
	m_looped = p_looped;

	if (IsTrackingSound())
	{
		m_engine.GetBackend().setLooping(m_handle.value(), m_looped);
	}
}

void OvAudio::Entities::AudioSource::SetPitch(float p_pitch)
{
	m_pitch = p_pitch;

	if (IsTrackingSound())
	{
		// m_trackedSound->GetTrack()->setPlaybackSpeed(p_pitch < 0.01f ? 0.01f : p_pitch);
		m_engine.GetBackend().setRelativePlaySpeed(m_handle.value(), m_pitch); // TODO: should we clamp pitch like with irrklang?
	}
}

bool OvAudio::Entities::AudioSource::IsTrackingSound() const
{
	return m_handle.has_value();
}

std::optional<OvAudio::Data::SoundHandle> OvAudio::Entities::AudioSource::GetSoundHandle() const
{
	return m_handle.value();
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

bool OvAudio::Entities::AudioSource::IsFinished() const
{
	if (IsTrackingSound())
	{
		return m_engine.GetBackend().isValidVoiceHandle(m_handle.value());
	}
	{
		return true;
	}
}

void OvAudio::Entities::AudioSource::Play(const Resources::Sound& p_sound)
{
	/* Stops and destroy the previous sound (If any) */
	StopAndDestroyTrackedSound();
	
	/* Play the sound and store a tracker to the sound into memory */
	m_handle =
		m_spatial ?
		m_engine.PlaySpatialSound(p_sound, false, m_looped, m_transform->GetWorldPosition(), true) :
		m_engine.PlaySound(p_sound, false, m_looped, true);

	/* If the sound tracker is non-null, apply AudioSource settings to the sound (Not every settings because some are already set with AudioPlayer::PlaySound method) */
	if (IsTrackingSound())
	{
		auto& backend = m_engine.GetBackend();
		backend.setVolume(m_handle.value(), m_volume);
		backend.setPan(m_handle.value(), m_pan * -1.0f); // TODO: Check if pan should be negative here (wasn't with irrklang)
		backend.setRelativePlaySpeed(m_handle.value(), m_pitch); // TODO: should we clamp pitch like with irrklang?
		backend.set3dSourceMinMaxDistance(m_handle.value(), m_attenuationThreshold, std::numeric_limits<float>::infinity());
		backend.setPause(m_handle.value(), false);
	}
}

void OvAudio::Entities::AudioSource::Resume()
{
	if (IsTrackingSound())
	{
		m_engine.GetBackend().setPause(m_handle.value(), false);
	}
}

void OvAudio::Entities::AudioSource::Pause()
{
	if (IsTrackingSound())
	{
		m_engine.GetBackend().setPause(m_handle.value(), true);
	}
}

void OvAudio::Entities::AudioSource::Stop()
{
	if (IsTrackingSound())
	{
		m_engine.GetBackend().stop(m_handle.value());
	}
}

void OvAudio::Entities::AudioSource::StopAndDestroyTrackedSound()
{
	if (IsTrackingSound())
	{
		m_engine.GetBackend().stop(m_handle.value());
		m_engine.GetBackend().destroyVoiceGroup(m_handle.value());
	}
}
