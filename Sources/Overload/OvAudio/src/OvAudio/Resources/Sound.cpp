/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include <OvAudio/Resources/Sound.h>

#include <soloud_wav.h>

OvAudio::Resources::Sound::Sound(const std::string& p_path) : path(p_path)
{
	sound = new SoLoud::Wav();
	sound->load(p_path.c_str());
}

OvAudio::Resources::Sound::~Sound()
{
	delete sound;
}
