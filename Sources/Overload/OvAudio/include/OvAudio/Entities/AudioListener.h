/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once


#include <OvAudio/Entities/AudioSource.h>

#include <OvMaths/FTransform.h>
#include <OvMaths/FVector3.h>

namespace OvAudio::Entities
{
	/**
	* Represents the ears of your application.
	* You can have multiple ones but only the last created will be considered by the AudioEngine
	*/
	class AudioListener
	{
	public:
		/**
		* AudioListener constructor
		* @param p_transform
		*/
		AudioListener(OvTools::Utils::OptRef<OvMaths::FTransform> p_transform = std::nullopt);

		/**
		* AudioListener destructor
		*/
		~AudioListener();

		/**
		* Returns the AudioListener FTransform
		*/
		OvMaths::FTransform& GetTransform();

		/**
		* Enable or disable the audio listener
		* @param p_enable
		*/
		void SetEnabled(bool p_enable);

		/**
		* Returns true if the audio listener is enabled
		*/
		bool IsEnabled() const;

	private:
		OvTools::Utils::ReferenceOrValue<OvMaths::FTransform> m_transform;
		bool m_enabled = true;

	public:
		static OvTools::Eventing::Event<AudioListener&>	CreatedEvent;
		static OvTools::Eventing::Event<AudioListener&>	DestroyedEvent;
	};
}