/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include "OvCore/ResourceManagement/SoundManager.h"

OvAudio::Resources::Sound* OvCore::ResourceManagement::SoundManager::CreateResource(const std::filesystem::path& p_path)
{
	std::string realPath = GetRealPath(p_path).string();
	OvAudio::Resources::Sound* sound = OvAudio::Resources::Loaders::SoundLoader::Create(realPath);
	if (sound)
	{
		const_cast<std::string&>(sound->path) = p_path.string(); // Force the resource path to fit the given path
	}

	return sound;
}

void OvCore::ResourceManagement::SoundManager::DestroyResource(OvAudio::Resources::Sound* p_resource)
{
	OvAudio::Resources::Loaders::SoundLoader::Destroy(p_resource);
}

void OvCore::ResourceManagement::SoundManager::ReloadResource(OvAudio::Resources::Sound* p_resource, const std::filesystem::path& p_path)
{
}
