/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include <vector>
#include <optional>


#include "OvAudio/Entities/AudioSource.h"
#include "OvAudio/Entities/AudioListener.h"

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
		* Returns the working directory
		*/
		const std::string& GetWorkingDirectory() const;

		/**
		* Returns the IrrKlang engine
		*/
		irrklang::ISoundEngine* GetIrrklangEngine() const;

		/**
		* Returns the current listener informations :
		* Format: std::tuple<Active, Position, Direction>
		* @parma p_considerDisabled
		*/
		std::optional<std::pair<OvMaths::FVector3, OvMaths::FVector3>> GetListenerInformation(bool p_considerDisabled = false) const;

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
		
		irrklang::ISoundEngine* m_irrklangEngine = nullptr;
	};
}