/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include <OvAudio/Data/SoundHandle.h>

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
		*/
		SoundInstance(SoLoud::Soloud& p_backend, SoundHandle p_handle);

		/**
		* Sets the volume of the sound instance
		* @param p_volume
		*/
		void SetVolume(float p_volume);

		/**
		* Sets the pan of the sound instance
		* @param p_pan (-1 = Left, 0 = Center, 1 = Right)
		*/
		void SetPan(float p_pan);

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
		* Sets the minimum distance before applying sound attenuation
		* @param p_distance
		*/
		void SetAttenuationThreshold(float p_distance);

		/**
		* Sets the pause state of the sound instance
		* @param p_pause
		*/
		void SetPause(bool p_pause);

		/**
		* Stops the sound instance
		*/
		void Stop();

		/**
		* Returns true if the sound instance is currently playing
		*/
		bool IsPlaying() const;

		/**
		* Returns the handle of the sound instance
		*/
		SoundHandle GetHandle() const;

	private:
		SoLoud::Soloud& m_backend;
		SoundHandle m_handle;
	};
}
