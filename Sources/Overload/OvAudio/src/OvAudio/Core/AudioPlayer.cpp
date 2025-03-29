/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include <OvDebug/Logger.h>

#include "OvAudio/Core/AudioPlayer.h"

OvAudio::Core::AudioPlayer::AudioPlayer(AudioEngine& p_audioEngine) : m_audioEngine(p_audioEngine)
{
}

std::unique_ptr<OvAudio::Tracking::SoundTracker> OvAudio::Core::AudioPlayer::PlaySound(const Resources::Sound& p_sound, bool p_autoPlay, bool p_looped, bool p_track)
{
	std::unique_ptr<Tracking::SoundTracker> result;

	if (!m_audioEngine.IsValid())
	{
		OVLOG_WARNING("Unable to play \"" + p_sound.path + "\". Audio engine is not valid");
		return result;
	}

	irrklang::ISound* sound = m_audioEngine.GetIrrklangEngine()->play2D((m_audioEngine.GetWorkingDirectory() + p_sound.path).c_str(), p_looped, p_autoPlay, p_track);

	if (p_track)
	{
		if (sound)
			result = std::make_unique<Tracking::SoundTracker>(sound);
		else
			OVLOG_ERROR("Unable to play \"" + p_sound.path + "\"");
	}

	return result;
}

std::unique_ptr<OvAudio::Tracking::SoundTracker> OvAudio::Core::AudioPlayer::PlaySpatialSound(const Resources::Sound& p_sound, bool p_autoPlay, bool p_looped, const OvMaths::FVector3& p_position, bool p_track)
{
	std::unique_ptr<Tracking::SoundTracker> result;

	if (!m_audioEngine.IsValid())
	{
		OVLOG_WARNING("Unable to play \"" + p_sound.path + "\". Audio engine is not valid");
		return result;
	}

	irrklang::ISound* sound = m_audioEngine.GetIrrklangEngine()->play3D((m_audioEngine.GetWorkingDirectory() + p_sound.path).c_str(), reinterpret_cast<const irrklang::vec3df&>(p_position), p_looped, p_autoPlay, p_track);

	if (p_track)
	{
		if (sound)
			result = std::make_unique<Tracking::SoundTracker>(sound);
		else
			OVLOG_ERROR("Unable to play \"" + p_sound.path + "\"");
	}

	return result;
}
