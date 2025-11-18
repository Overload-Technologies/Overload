/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include <memory>
#include <optional>

#include <OvAudio/Data/SoundHandle.h>
#include <OvAudio/Data/SoundInstance.h>
#include <OvAudio/Resources/Sound.h>
#include <OvMaths/FVector3.h>
#include <OvMaths/FTransform.h>
#include <OvTools/Eventing/Event.h>
#include <OvTools/Utils/OptRef.h>
#include <OvTools/Utils/ReferenceOrValue.h>

namespace OvAudio::Core { class AudioEngine; }

namespace OvAudio::Entities
{
	/**
	* An audio source is an entity that can play a sound in a 3D space.
	*/
	class AudioSource
	{
	public:
		/**
		* AudioSource constructor (Internal transform management)
		* @param p_engine
		* @param p_transform
		*/
		AudioSource(
			Core::AudioEngine& p_engine,
			OvTools::Utils::OptRef<OvMaths::FTransform> p_transform = std::nullopt
		);

		/**
		* AudioSource destructor
		*/
		~AudioSource();

		/**
		* Returns the AudioSource transform
		*/
		const OvMaths::FTransform& GetTransform();

		/**
		* Apply every AudioSource settings to the currently tracked sound
		*/
		void ApplySourceSettingsToTrackedSound();

		/**
		* Returns true if the audio source has a sound instance
		*/
		bool HasSound() const;

		/**
		* Returns true if the audio source is currently playing
		*/
		bool IsPlaying() const;

		/**
		* Retrurns true if the audio source is currently paused
		*/
		bool IsPaused() const;

		/**
		* Returns the currently tracked sound if any, or nullptr
		*/
		std::weak_ptr<Data::SoundInstance> GetSoundInstance() const;

		/**
		* Defines the audio source volume
		* @param p_volume
		*/
		void SetVolume(float p_volume);

		/**
		* Defines the audio source pan (Left / Right)
		* @param p_pan
		*/
		void SetPan(float p_pan);

		/**
		* Defines if the audio source should loop
		* @param p_looped
		*/
		void SetLooped(bool p_looped);

		/**
		* Defines the audio source pitch (= frequency or playback speed)
		* @param p_pitch
		*/
		void SetPitch(float p_pitch);

		/**
		* Defines if the audio source should be spatialized or not
		* @param p_value
		*/
		void SetSpatial(bool p_value);

		/**
		* Defines the audio source attenuation threshold (Minimum distance before applying sound attenuation)
		* @param p_distance
		*/
		void SetAttenuationThreshold(float p_distance);

		/**
		* Returns the audio source volume
		*/
		float GetVolume() const;

		/**
		* Returns the audio source pan (Left / Right)
		*/
		float GetPan() const;

		/**
		* Returns if the audio source is looping
		*/
		bool IsLooped() const;

		/**
		* Returns the audio source pitch (= frequency or playback speed)
		*/
		float GetPitch() const;

		/**
		* Returns true if the audio source is spatialized
		*/
		bool IsSpatial() const;

		/**
		* Returns the audio source attenuation threshold (Minimum distance before applying sound attenuation)
		*/
		float GetAttenuationThreshold() const;

		/**
		* Play the given sound
		* @param p_sound
		*/
		void Play(const Resources::Sound& p_sound);

		/**
		* Resume the audio source
		*/
		void Resume();

		/**
		* Pause the audio source
		*/
		void Pause();

		/**
		* Stop the audio source
		*/
		void Stop();

		/**
		* Update the audio source
		*/
		void Update();

	public:
		static OvTools::Eventing::Event<AudioSource&> CreatedEvent;
		static OvTools::Eventing::Event<AudioSource&> DestroyedEvent;

	private:
		Core::AudioEngine& m_engine;
		OvTools::Utils::ReferenceOrValue<OvMaths::FTransform> m_transform;
		std::shared_ptr<Data::SoundInstance> m_instance;

		// Sound settings
		bool m_spatial = false;
		float m_volume = 1.0f;
		float m_pan = 0.0f;
		bool m_looped = false;
		float m_pitch = 1.0f;
		float m_attenuationThreshold = 1.0f;
	};
}