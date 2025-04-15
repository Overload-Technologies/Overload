/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include <OvAudio/Resources/Sound.h>

#include <soloud_wav.h>

OvAudio::Resources::Sound::Sound(const std::string& p_path, std::unique_ptr<SoLoud::Wav>&& p_audioData) :
	path(p_path),
	audioData(std::move(p_audioData))
{
}
