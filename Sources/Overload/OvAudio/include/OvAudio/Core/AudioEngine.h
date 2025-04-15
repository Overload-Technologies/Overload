/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include <optional>
#include <vector>

#include <OvAudio/Data/SoundHandle.h>
#include <OvAudio/Entities/AudioSource.h>
#include <OvAudio/Entities/AudioListener.h>

namespace SoLoud
{
	class Soloud;
}

namespace OvAudio::Core
{
	/**
	* AudioEngine is the main class of the audio system.
	* It's responsible for initializing the audio backend and managing the audio sources and listeners.
	*/
	class AudioEngine
	{
	public:
		/**
		* Constructor of the AudioEngine
		*/
		AudioEngine();

		/**
		* Destructor of the AudioEngine
		*/
		~AudioEngine();

		/**
		* Returns true if the AudioEngine is valid (properly initialized and available)
		*/
		bool IsValid() const;

		/**
		* Update AudioSources and AudioListeners
		*/
		void Update();

		/**
		* Suspend every sounds. Keeps every sound state (Pause and play) to Unsuspend them correctly
		*/
		void Suspend();

		/**
		* Suspend every sounds. Resume previously played sounds
		*/
		void Unsuspend();

		/**
		* Returns true if the AudioEngine is currently suspended
		*/
		bool IsSuspended() const;

		/**
		* Play a sound in and return a SoundInstance if successful
		* @param p_sound
		* @param p_position (if set, the sound will be spatialized)
		*/
		std::shared_ptr<OvAudio::Data::SoundInstance> Play(
			const Resources::Sound& p_sound,
			OvTools::Utils::OptRef<const OvMaths::FVector3> p_position = std::nullopt
		);

		/**
		* Returns the main listener
		* @param p_includeDisabled
		*/
		OvTools::Utils::OptRef<OvAudio::Entities::AudioListener> FindMainListener(bool p_includeDisabled = false) const;

		/**
		* Returns a reference to the backend engine
		*/
		SoLoud::Soloud& GetBackend() const;

	private:
		void Consider(Entities::AudioSource& p_audioSource);
		void Consider(Entities::AudioListener& p_audioListener);

		void Unconsider(Entities::AudioSource& p_audioSource);
		void Unconsider(Entities::AudioListener& p_audioListener);

	private:
		bool m_suspended = false;

		std::vector<std::reference_wrapper<Entities::AudioSource>> m_audioSources;
		std::vector<std::reference_wrapper<Entities::AudioSource>> m_suspendedAudioSources;
		std::vector<std::reference_wrapper<Entities::AudioListener>> m_audioListeners;

		std::unique_ptr<SoLoud::Soloud> m_backend;
	};
}