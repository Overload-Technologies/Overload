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
	* Handle the creation of the Audio context
	* Will take care of the consideration of AudioSources and AudioListeners
	*/
	class AudioEngine
	{
	public:
		/**
		* Constructor of the AudioEngine
		*/
		AudioEngine(const std::string& p_workingDirectory);

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
		* Play a sound in 2D and return a SoundTracker if tracking is asked
		* @param p_sound
		* @param p_autoPlay
		* @param p_looped
		* @param p_track
		*/
		std::optional<Data::SoundHandle> PlaySound(const Resources::Sound& p_sound, bool p_autoPlay = true, bool p_looped = false, bool p_track = false);

		/**
		* Play a sound in 3D and return a SoundTracker if tracking is asked
		* @param p_sound
		* @param p_autoPlay
		* @param p_looped
		* @param p_position
		* @param p_track
		*/
		std::optional<Data::SoundHandle> PlaySpatialSound(const Resources::Sound& p_sound, bool p_autoPlay = true, bool p_looped = false, const OvMaths::FVector3& p_position = { 0.0f, 0.0f, 0.0f }, bool p_track = false);

		/**
		* Returns the current listener informations :
		* Format: std::tuple<Active, Position, Direction>
		* @parma p_considerDisabled
		*/
		std::optional<std::pair<OvMaths::FVector3, OvMaths::FVector3>> GetListenerInformation(bool p_considerDisabled = false) const;

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
		const std::string m_workingDirectory;
		bool m_suspended = false;

		std::vector<std::reference_wrapper<Entities::AudioSource>> m_audioSources;
		std::vector<std::reference_wrapper<Entities::AudioSource>> m_suspendedAudioSources;
		std::vector<std::reference_wrapper<Entities::AudioListener>> m_audioListeners;
		
		std::unique_ptr<SoLoud::Soloud> m_soloudEngine;
	};
}