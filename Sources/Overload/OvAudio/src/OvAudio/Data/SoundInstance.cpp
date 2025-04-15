/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include <soloud.h>

#include <OvAudio/Data/SoundInstance.h>
#include <OvAudio/Core/AudioEngine.h>

OvAudio::Data::SoundInstance::SoundInstance(SoLoud::Soloud& p_backend, SoundHandle p_handle) :
	m_backend(p_backend),
	m_handle(p_handle)
{
}

void OvAudio::Data::SoundInstance::SetVolume(float p_volume)
{
	m_backend.setVolume(m_handle, p_volume);
}

void OvAudio::Data::SoundInstance::SetPan(float p_pan)
{
	m_backend.setPan(m_handle, p_pan); // TODO: Check if pan should be negative here (wasn't with irrklang)
}

void OvAudio::Data::SoundInstance::SetLooped(bool p_looped)
{
	m_backend.setLooping(m_handle, p_looped);
}

void OvAudio::Data::SoundInstance::SetPitch(float p_pitch)
{
	m_backend.setRelativePlaySpeed(m_handle, p_pitch); // TODO: should we clamp pitch like with irrklang?
}

void OvAudio::Data::SoundInstance::SetAttenuationThreshold(float p_distance)
{
	m_backend.set3dSourceMinMaxDistance(m_handle, p_distance, 1000.0f);
}

void OvAudio::Data::SoundInstance::SetPause(bool p_pause)
{
	m_backend.setPause(m_handle, p_pause);
}

void OvAudio::Data::SoundInstance::Stop()
{
	m_backend.stop(m_handle);
	m_backend.destroyVoiceGroup(m_handle);
}

bool OvAudio::Data::SoundInstance::IsPlaying() const
{
	return m_backend.isValidVoiceHandle(m_handle);
}

OvAudio::Data::SoundHandle OvAudio::Data::SoundInstance::GetHandle() const
{
	return m_handle;
}
