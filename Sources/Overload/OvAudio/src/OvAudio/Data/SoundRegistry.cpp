/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include <OvAudio/Data/SoundRegistry.h>

OvAudio::Data::SoundInstance& OvAudio::Data::SoundRegistry::CreateInstance(SoLoud::Soloud& p_backend, SoundHandle p_handle)
{
	return m_sounds.emplace_back(p_backend, p_handle);
}
