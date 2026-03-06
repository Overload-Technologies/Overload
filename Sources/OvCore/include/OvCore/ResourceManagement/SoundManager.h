/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include <OvAudio/Resources/Loaders/SoundLoader.h>

#include "OvCore/ResourceManagement/AResourceManager.h"

namespace OvCore::ResourceManagement
{
	/**
	* ResourceManager of sounds
	*/
	class SoundManager : public AResourceManager<OvAudio::Resources::Sound>
	{
	public:
		/**
		* Create the resource identified by the given path
		* @param p_path
		*/
		virtual OvAudio::Resources::Sound* CreateResource(const std::filesystem::path& p_path) override;

		/**
		* Destroy the given resource
		* @param p_resource
		*/
		virtual void DestroyResource(OvAudio::Resources::Sound* p_resource) override;

		/**
		* Reload the given resource
		* @param p_resource
		* @param p_path
		*/
		virtual void ReloadResource(OvAudio::Resources::Sound* p_resource, const std::filesystem::path& p_path) override;
	};
}