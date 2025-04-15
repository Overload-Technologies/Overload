/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include <soloud.h>

#include <OvAudio/Data/SoundInstance.h>
#include <OvAudio/Core/AudioEngine.h>
#include <OvDebug/Assertion.h>

OvAudio::Data::SoundInstance::SoundInstance(SoLoud::Soloud& p_backend, SoundHandle p_handle) :
	m_backend(p_backend),
	m_handle(p_handle)
{
}

void OvAudio::Data::SoundInstance::SetVolume(float p_volume)
{
	Validate();
	m_backend.setVolume(m_handle, p_volume);
}

void OvAudio::Data::SoundInstance::SetPan(float p_pan)
{
	Validate();
	m_backend.setPan(m_handle, p_pan); // TODO: Check if pan should be negative here (wasn't with irrklang)
}

void OvAudio::Data::SoundInstance::SetLooped(bool p_looped)
{
	Validate();
	m_backend.setLooping(m_handle, p_looped);
}

void OvAudio::Data::SoundInstance::SetPitch(float p_pitch)
{
	Validate();
	m_backend.setRelativePlaySpeed(m_handle, p_pitch); // TODO: should we clamp pitch like with irrklang?
}

void OvAudio::Data::SoundInstance::SetAttenuationThreshold(float p_distance)
{
	Validate();
	m_backend.set3dSourceMinMaxDistance(m_handle, p_distance, 1000.0f);
}

void OvAudio::Data::SoundInstance::SetPause(bool p_pause)
{
	Validate();
	m_backend.setPause(m_handle, p_pause);
}

void OvAudio::Data::SoundInstance::Stop()
{
	Validate();
	m_backend.stop(m_handle);
	m_backend.destroyVoiceGroup(m_handle);
}

bool OvAudio::Data::SoundInstance::IsValid() const
{
	return m_backend.isValidVoiceHandle(m_handle);
}

bool OvAudio::Data::SoundInstance::IsPaused() const
{
	Validate();
	return m_backend.getPause(m_handle);
}

OvAudio::Data::SoundHandle OvAudio::Data::SoundInstance::GetHandle() const
{
	return m_handle;
}

void OvAudio::Data::SoundInstance::SetSpatialParameters(
	const OvMaths::FVector3& p_position,
	const OvMaths::FVector3& p_velocity
) const
{
	m_backend.set3dSourceParameters(
		m_handle,
		p_position.x, p_position.y, p_position.z,
		p_velocity.x, p_velocity.y, p_velocity.z);
}

void OvAudio::Data::SoundInstance::Validate() const
{
	OVASSERT(IsValid(), "Sound instance is not valid");
}
