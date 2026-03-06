/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include <algorithm>
#include <format>
#include <ranges>

#include <soloud.h>
#include <soloud_wav.h>

#include <OvAudio/Core/AudioEngine.h>
#include <OvDebug/Assertion.h>
#include <OvDebug/Logger.h>

OvAudio::Core::AudioEngine::AudioEngine()
{
	m_backend = std::make_unique<SoLoud::Soloud>();

	// If initialization failed
	if (m_backend->init() != SoLoud::SOLOUD_ERRORS::SO_NO_ERROR)
	{
		OVLOG_ERROR("Failed to initialize the audio engine. Playback requests will be ignored.");
		m_backend.reset();
		return;
	}

	using AudioSourceReceiver = void(AudioEngine::*)(OvAudio::Entities::AudioSource&);
	using AudioListenerReceiver = void(AudioEngine::*)(OvAudio::Entities::AudioListener&);

	Entities::AudioSource::CreatedEvent += std::bind(static_cast<AudioSourceReceiver>(&AudioEngine::Consider), this, std::placeholders::_1);
	Entities::AudioSource::DestroyedEvent += std::bind(static_cast<AudioSourceReceiver>(&AudioEngine::Unconsider), this, std::placeholders::_1);
	Entities::AudioListener::CreatedEvent += std::bind(static_cast<AudioListenerReceiver>(&AudioEngine::Consider), this, std::placeholders::_1);
	Entities::AudioListener::DestroyedEvent += std::bind(static_cast<AudioListenerReceiver>(&AudioEngine::Unconsider), this, std::placeholders::_1);
}

OvAudio::Core::AudioEngine::~AudioEngine()
{
	if (IsValid())
	{
		m_backend->deinit();
	}
}

bool OvAudio::Core::AudioEngine::IsValid() const
{
	return m_backend.operator bool();
}

void OvAudio::Core::AudioEngine::Update()
{
	if (!IsValid())
	{
		return;
	}

	for (const auto& source : m_audioSources)
	{
		source.get().Update();
	}

	// Defines the listener position using the last listener created (If any)
	const auto listener = FindMainListener();

	if (listener.has_value())
	{
		const auto& transform = listener->GetTransform();
		const auto& pos = transform.GetWorldPosition();
		const auto at = transform.GetWorldForward() * -1.0f;
		m_backend->set3dListenerPosition(pos.x, pos.y, pos.z);
		m_backend->set3dListenerAt(at.x, at.y, at.z);
	}
	else
	{
		m_backend->set3dListenerPosition(0.0f, 0.0f, 0.0f);
		m_backend->set3dListenerAt(0.0f, 0.0f, -1.0f);
	}

	m_backend->update3dAudio();
}

void OvAudio::Core::AudioEngine::Suspend()
{
	for (auto& audioSourceRef : m_audioSources)
	{
		auto& source = audioSourceRef.get();
		if (source.HasSound() && !source.IsPaused())
		{
			m_suspendedAudioSources.push_back(audioSourceRef);
			source.Pause();
		}
	}

	m_suspended = true;
}

void OvAudio::Core::AudioEngine::Unsuspend()
{
	for (auto& audioSourceRef : m_audioSources)
	{
		auto& source = audioSourceRef.get();
		if (source.HasSound() && source.IsPaused())
		{
			source.Resume();
		}
	}

	m_suspendedAudioSources.clear();
	m_suspended = false;
}

bool OvAudio::Core::AudioEngine::IsSuspended() const
{
	return m_suspended;
}

std::shared_ptr<OvAudio::Data::SoundInstance> OvAudio::Core::AudioEngine::Play2D(const Resources::Sound& p_sound, float p_pan, std::optional<float> p_volume, bool p_startPaused)
{
	if (!IsValid())
	{
		OVLOG_WARNING(std::format("Unable to play {}. Audio engine is not valid", p_sound.path));
		return {};
	}

	const auto handle = m_backend->play(
		*p_sound.audioData,
		p_volume.value_or(-1.0f),
		p_pan,
		p_startPaused
	);

	return std::make_shared<Data::SoundInstance>(
		*m_backend,
		handle,
		false
	);
}

std::shared_ptr<OvAudio::Data::SoundInstance> OvAudio::Core::AudioEngine::Play3D(
	const Resources::Sound& p_sound,
	const OvMaths::FVector3& p_position,
	const OvMaths::FVector3& p_velocity,
	std::optional<float> p_volume,
	bool p_startPaused
)
{
	if (!IsValid())
	{
		OVLOG_WARNING(std::format("Unable to play {}. Audio engine is not valid", p_sound.path));
		return {};
	}

	const auto handle = m_backend->play3d(
		*p_sound.audioData,
		p_position.x, p_position.y, p_position.z,
		p_velocity.x, p_velocity.y, p_velocity.z,
		p_volume.value_or(-1.0f),
		p_startPaused
	);

	return std::make_shared<Data::SoundInstance>(
		*m_backend,
		handle,
		true
	);
}

OvTools::Utils::OptRef<OvAudio::Entities::AudioListener> OvAudio::Core::AudioEngine::FindMainListener(bool p_includeDisabled) const
{
	for (auto& listener : m_audioListeners | std::views::reverse)
	{
		if (p_includeDisabled || listener.get().IsEnabled())
		{
			return listener.get();
		}
	}

	return std::nullopt;
}

SoLoud::Soloud& OvAudio::Core::AudioEngine::GetBackend() const
{
	OVASSERT(IsValid(), "Audio engine is not valid");
	return *m_backend;
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
