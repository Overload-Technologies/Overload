/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include <filesystem>
#include <functional>

namespace OvCore::ECS
{
	class Actor;
}

namespace OvCore::SceneSystem
{
	/**
	* Utility functions to serialize and instantiate prefab files.
	*/
	class PrefabOperations
	{
	public:
		/**
		* Disabled constructor
		*/
		PrefabOperations() = delete;

		/**
		* Save an actor hierarchy to a prefab file.
		* @param p_rootActor
		* @param p_outputPath
		*/
		static bool SaveToFile(OvCore::ECS::Actor& p_rootActor, const std::filesystem::path& p_outputPath);

		/**
		* Instantiate a prefab file using the provided actor factory.
		* @param p_prefabPath
		* @param p_createActor
		*/
		static OvCore::ECS::Actor* InstantiateFromFile(
			const std::filesystem::path& p_prefabPath,
			const std::function<OvCore::ECS::Actor&(void)>& p_createActor
		);
	};
}
