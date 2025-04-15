#pragma once
/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include <vector>

#include <OvAudio/Data/SoundInstance.h>

namespace OvAudio::Data
{
	/**
	* Stores all the sound instances and manages their lifecycle
	*/
	class SoundRegistry
	{
	public:
		SoundInstance& CreateInstance(SoLoud::Soloud& p_backend, SoundHandle p_handle);

	private:
		std::vector<SoundInstance> m_sounds;
	};
}
