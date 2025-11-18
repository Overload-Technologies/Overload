/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include <string>

#include "OvAudio/Resources/Sound.h"

namespace OvAudio::Resources::Loaders
{
	/**
	* Handle the Mesh creation and destruction
	*/
	class SoundLoader
	{
	public:
		/**
		* Disabled constructor
		*/
		SoundLoader() = delete;

		/**
		* Create a sound
		* @param p_filepath
		*/
		static Sound* Create(const std::string& p_filepath);

		/**
		* Reload a sound
		* @param p_sound
		* @param p_path
		*/
		static void Reload(Sound& p_sound, const std::string& p_path);

		/**
		* Destroy a sound
		* @param p_soundInstance
		*/
		static bool Destroy(Sound*& p_soundInstance);
	};
}