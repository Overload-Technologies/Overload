/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include <algorithm>

#include <soloud.h>
#include <soloud_wav.h>

#include <OvAudio/Core/AudioEngine.h>
#include <OvDebug/Logger.h>

OvAudio::Core::AudioEngine::AudioEngine(const std::string & p_workingDirectory) : m_workingDirectory(p_workingDirectory)
{
	m_soloudEngine = std::make_unique<SoLoud::Soloud>();
	auto res = m_soloudEngine->init();

	if (!IsValid())
	{
		OVLOG_WARNING("Failed to create the audio engine. Playback requests will be ignored.");
		return;
	}

	using AudioSourceReceiver	= void(AudioEngine::*)(OvAudio::Entities::AudioSource&);
	using AudioListenerReceiver = void(AudioEngine::*)(OvAudio::Entities::AudioListener&);

	Entities::AudioSource::CreatedEvent		+= std::bind(static_cast<AudioSourceReceiver>(&AudioEngine::Consider), this, std::placeholders::_1);
	Entities::AudioSource::DestroyedEvent	+= std::bind(static_cast<AudioSourceReceiver>(&AudioEngine::Unconsider), this, std::placeholders::_1);
	Entities::AudioListener::CreatedEvent	+= std::bind(static_cast<AudioListenerReceiver>(&AudioEngine::Consider), this, std::placeholders::_1);
	Entities::AudioListener::DestroyedEvent	+= std::bind(static_cast<AudioListenerReceiver>(&AudioEngine::Unconsider), this, std::placeholders::_1);
}

OvAudio::Core::AudioEngine::~AudioEngine()
{
	if (IsValid())
	{
		m_soloudEngine->deinit();
	}
}

bool OvAudio::Core::AudioEngine::IsValid() const
{
	return m_soloudEngine.operator bool();
}

void OvAudio::Core::AudioEngine::Update()
{
	if (!IsValid())
	{
		return;
	}

	/* Update tracked sounds */
	std::for_each(m_audioSources.begin(), m_audioSources.end(), std::mem_fn(&Entities::AudioSource::UpdateTrackedSoundPosition));

	/* Defines the listener position using the last listener created (If any) */
	std::optional<std::pair<OvMaths::FVector3, OvMaths::FVector3>> listener = GetListenerInformation();
	if (listener.has_value())
	{
		const auto& pos = listener.value().first;
		const auto& at = listener.value().second;

		m_soloudEngine->set3dListenerPosition(pos.x, pos.y, pos.z);
		m_soloudEngine->set3dListenerAt(at.x, at.y, at.z);
	}
	else
	{
		m_soloudEngine->set3dListenerPosition(0.0f, 0.0f, 0.0f);
		m_soloudEngine->set3dListenerAt(0.0f, 0.0f, -1.0f);
	}
}

void OvAudio::Core::AudioEngine::Suspend()
{
	std::for_each(m_audioSources.begin(), m_audioSources.end(), [this](std::reference_wrapper<Entities::AudioSource> p_audioSource) {
		auto& source = p_audioSource.get();
		if (source.IsTrackingSound() && !m_soloudEngine->getPause(source.GetSoundHandle().value()))
		{
			m_suspendedAudioSources.push_back(p_audioSource);
			source.Pause();
		}
	});

	m_suspended = true;
}

void OvAudio::Core::AudioEngine::Unsuspend()
{
	std::for_each(m_suspendedAudioSources.begin(), m_suspendedAudioSources.end(), std::mem_fn(&Entities::AudioSource::Resume));
	m_suspendedAudioSources.clear();
	m_suspended = false;
}

bool OvAudio::Core::AudioEngine::IsSuspended() const
{
	return m_suspended;
}

std::optional<OvAudio::Data::SoundHandle> OvAudio::Core::AudioEngine::PlaySound(const OvAudio::Resources::Sound& p_sound, bool p_autoPlay, bool p_looped, bool p_track)
{
	if (!IsValid())
	{
		OVLOG_WARNING("Unable to play \"" + p_sound.path + "\". Audio engine is not valid");
		return std::nullopt;
	}

	auto handle = m_soloudEngine->play(*p_sound.sound);

	if (p_track)
	{
		if (m_soloudEngine->isValidVoiceHandle(handle))
		{
			return handle;
		}
		
		OVLOG_ERROR("Unable to play \"" + p_sound.path + "\"");
	}

	return std::nullopt;
}

std::optional<OvAudio::Data::SoundHandle> OvAudio::Core::AudioEngine::PlaySpatialSound(const OvAudio::Resources::Sound& p_sound, bool p_autoPlay, bool p_looped, const OvMaths::FVector3& p_position, bool p_track)
{
	if (!IsValid())
	{
		OVLOG_WARNING("Unable to play \"" + p_sound.path + "\". Audio engine is not valid");
		return std::nullopt;
	}

	auto handle = m_soloudEngine->play3d(*p_sound.sound, p_position.x, p_position.y, p_position.z);

	if (p_track)
	{
		if (m_soloudEngine->isValidVoiceHandle(handle))
		{
			return handle;
		}

		OVLOG_ERROR("Unable to play \"" + p_sound.path + "\"");
	}

	return std::nullopt;
}

std::optional<std::pair<OvMaths::FVector3, OvMaths::FVector3>> OvAudio::Core::AudioEngine::GetListenerInformation(bool p_considerDisabled) const
{
	for (auto listener : m_audioListeners)
	{
		if (listener.get().IsEnabled() || p_considerDisabled)
		{
			auto& transform = m_audioListeners.back().get().GetTransform();
			return
			{{
				transform.GetWorldPosition(),
				transform.GetWorldForward() * -1.0f
			}};
		}
	}

	return {};
}

SoLoud::Soloud& OvAudio::Core::AudioEngine::GetBackend() const
{
	// TODO: assert if no backend
	return *m_soloudEngine;
}

void OvAudio::Core::AudioEngine::Consider(OvAudio::Entities::AudioSource & p_audioSource)
{
	m_audioSources.push_back(std::ref(p_audioSource));
}

void OvAudio::Core::AudioEngine::Consider(OvAudio::Entities::AudioListener & p_audioListener)
{
	m_audioListeners.push_back(std::ref(p_audioListener));
}

void OvAudio::Core::AudioEngine::Unconsider(OvAudio::Entities::AudioSource & p_audioSource)
{
	auto found = std::find_if(m_audioSources.begin(), m_audioSources.end(), [&p_audioSource](std::reference_wrapper<Entities::AudioSource> element)
	{
		return std::addressof(p_audioSource) == std::addressof(element.get());
	});

	if (found != m_audioSources.end())
		m_audioSources.erase(found);

	if (m_suspended)
	{
		auto found = std::find_if(m_suspendedAudioSources.begin(), m_suspendedAudioSources.end(), [&p_audioSource](std::reference_wrapper<Entities::AudioSource> element)
		{
			return std::addressof(p_audioSource) == std::addressof(element.get());
		});

		if (found != m_suspendedAudioSources.end())
			m_suspendedAudioSources.erase(found);
	}
}

void OvAudio::Core::AudioEngine::Unconsider(OvAudio::Entities::AudioListener & p_audioListener)
{
	auto found = std::find_if(m_audioListeners.begin(), m_audioListeners.end(), [&p_audioListener](std::reference_wrapper<Entities::AudioListener> element)
	{
		return std::addressof(p_audioListener) == std::addressof(element.get());
	});

	if (found != m_audioListeners.end())
		m_audioListeners.erase(found);
}
