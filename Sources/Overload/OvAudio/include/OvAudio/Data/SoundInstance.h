/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include <OvAudio/Data/SoundHandle.h>
#include <OvAudio/Settings/EAttenuationModel.h>
#include <OvMaths/FVector3.h>

namespace SoLoud { class Soloud; }

namespace OvAudio::Data
{
	/**
	* Instance of a sound
	*/
	class SoundInstance
	{
	public:
		/**
		* Creates a sound instance from a sound handle
		* @param p_backend
		* @param p_handle
		* @param p_spatial
		*/
		SoundInstance(
			SoLoud::Soloud& p_backend,
			SoundHandle p_handle,
			bool p_spatial
		);

		/**
		* Sets the volume of the sound instance
		* @param p_volume
		*/
		void SetVolume(float p_volume);

		/**
		* Sets if the sound instance should loop
		* @param p_looped
		*/
		void SetLooped(bool p_looped);

		/**
		* Sets the pitch of the sound instance
		* @param p_pitch
		*/
		void SetPitch(float p_pitch);

		/**
		* Sets the pan of the sound instance
		* @param p_pan (-1 = Left, 0 = Center, 1 = Right)
		*/
		void SetPan(float p_pan);

		/**
		* Sets the attenuation model of the sound instance
		* @param p_model
		* @param p_factor
		*/
		void SetAttenuationModel(OvAudio::Settings::EAttenuationModel p_model, float p_factor = 1.0f);

		/**
		* Sets the minimum distance before applying sound attenuation
		* @param p_distance
		*/
		void SetAttenuationThreshold(float p_distance);

		/**
		* Updates the position of the sound
		* @param p_position
		* @param p_velocity
		*/
		void SetSpatialParameters(
			const OvMaths::FVector3& p_position,
			const OvMaths::FVector3& p_velocity
		) const;

		/**
		* Plays the sound instance
		*/
		void Play();

		/**
		* Pauses the sound instance
		*/
		void Pause();

		/**
		* Stops the sound instance
		*/
		void Stop();

		/**
		* Returns true if the sound instance is currently playing
		*/
		bool IsValid() const;

		/**
		* Returns true if the sound instance is currently paused
		*/
		bool IsPaused() const;

		/**
		* Returns true if the sound is spatialized
		*/
		bool IsSpatial() const;

		/**
		* Returns the handle of the sound instance
		*/
		SoundHandle GetHandle() const;

	private:
		void Validate() const;

	private:
		SoLoud::Soloud& m_backend;
		SoundHandle m_handle;
		bool m_spatial;
	};
}
