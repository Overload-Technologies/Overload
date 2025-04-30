/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include <soloud.h>

#include <OvAudio/Core/AudioEngine.h>
#include <OvAudio/Data/SoundInstance.h>
#include <OvDebug/Assertion.h>
#include <OvTools/Utils/EnumMapper.h>

using SoLoudAttenuationType = std::underlying_type<SoLoud::AudioSource::ATTENUATION_MODELS>::type;

template <>
struct OvTools::Utils::MappingFor<OvAudio::Settings::EAttenuationModel, SoLoudAttenuationType>
{
	using enum SoLoud::AudioSource::ATTENUATION_MODELS;
	using EnumType = OvAudio::Settings::EAttenuationModel;
	using type = std::tuple<
		EnumValuePair<EnumType::NONE, NO_ATTENUATION>,
		EnumValuePair<EnumType::INVERSE_DISTANCE, INVERSE_DISTANCE>,
		EnumValuePair<EnumType::LINEAR_DISTANCE, LINEAR_DISTANCE>,
		EnumValuePair<EnumType::EXPONENTIAL_DISTANCE, EXPONENTIAL_DISTANCE>
	>;
};

namespace
{
	constexpr SoLoudAttenuationType GetAttenuationModelValue(OvAudio::Settings::EAttenuationModel p_model)
	{
		return OvTools::Utils::ToValueImpl<OvAudio::Settings::EAttenuationModel, SoLoudAttenuationType>(p_model);
	}
}

OvAudio::Data::SoundInstance::SoundInstance(SoLoud::Soloud& p_backend, SoundHandle p_handle, bool p_spatial) :
	m_backend(p_backend),
	m_handle(p_handle),
	m_spatial(p_spatial)
{
}

void OvAudio::Data::SoundInstance::SetVolume(float p_volume)
{
	Validate();
	m_backend.setVolume(m_handle, p_volume);
}

void OvAudio::Data::SoundInstance::SetLooped(bool p_looped)
{
	Validate();
	m_backend.setLooping(m_handle, p_looped);
}

void OvAudio::Data::SoundInstance::SetPitch(float p_pitch)
{
	Validate();
	m_backend.setRelativePlaySpeed(m_handle, p_pitch);
}

void OvAudio::Data::SoundInstance::SetPan(float p_pan)
{
	if (!m_spatial)
	{
		Validate();
		m_backend.setPan(m_handle, p_pan);
	}
}

void OvAudio::Data::SoundInstance::SetAttenuationModel(OvAudio::Settings::EAttenuationModel p_model, float p_factor)
{
	using namespace OvTools::Utils;

	if (m_spatial)
	{
		Validate();
		m_backend.set3dSourceAttenuation(
			m_handle,
			GetAttenuationModelValue(p_model),
			p_factor
		);
	}
}

void OvAudio::Data::SoundInstance::SetAttenuationThreshold(float p_distance)
{
	// Matches irrKlang default value for compatibility.
	// Don't change this value if you don't know what you are doing:
	// This value causes the sound to stop attenuating after it reaches the max distance.
	constexpr float kMaxDistance = 1000000000.0f;

	if (m_spatial)
	{
		Validate();
		m_backend.set3dSourceMinMaxDistance(m_handle, p_distance, kMaxDistance);
	}
}

void OvAudio::Data::SoundInstance::SetSpatialParameters(
	const OvMaths::FVector3& p_position,
	const OvMaths::FVector3& p_velocity
) const
{
	if (m_spatial)
	{
		Validate();
		m_backend.set3dSourceParameters(
			m_handle,
			p_position.x, p_position.y, p_position.z,
			p_velocity.x, p_velocity.y, p_velocity.z);
	}
}

void OvAudio::Data::SoundInstance::Play()
{
	Validate();
	m_backend.setPause(m_handle, false);
}

void OvAudio::Data::SoundInstance::Pause()
{
	Validate();
	m_backend.setPause(m_handle, true);
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

bool OvAudio::Data::SoundInstance::IsSpatial() const
{
	return m_spatial;
}

OvAudio::Data::SoundHandle OvAudio::Data::SoundInstance::GetHandle() const
{
	return m_handle;
}

void OvAudio::Data::SoundInstance::Validate() const
{
	OVASSERT(IsValid(), "Sound instance is not valid");
}
