/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include <any>
#include <filesystem>
#include <unordered_map>

namespace OvCore::ResourceManagement
{
	/**
	* Handle the management of various resources of variable type
	*/
	template<typename T>
	class AResourceManager
	{
	public:
		/**
		* Handle the creation of a resource and register it
		* @param p_path
		*/
		T* LoadResource(const std::filesystem::path& p_path);

		/**
		* Handle the destruction of a resource and unregister it
		* @param p_path
		*/
		void UnloadResource(const std::filesystem::path& p_path);

		/**
		* Move a resource to the new given id (path)
		* @param p_previousPath
		* @param p_newPath
		*/
		bool MoveResource(const std::filesystem::path& p_previousPath, const std::filesystem::path& p_newPath);

		/**
		* Reload a resource
		* @param p_path
		*/
		void ReloadResource(const std::filesystem::path& p_path);

		/**
		* Return true if the resource exists (= Is registered)
		* @param p_path
		*/
		bool IsResourceRegistered(const std::filesystem::path& p_path);

		/**
		* Destroy and unregister every resources
		*/
		void UnloadResources();

		/**
		* Register a resource and associate it with a given path.
		* After this method is called, the memory managment of the resource
		* belong the the Resource Manager (Pointer memory deallocation)
		* @param p_path
		* @param p_instance
		*/
		T* RegisterResource(const std::filesystem::path& p_path, T* p_instance);

		/**
		* Unregister a resource (You have to destroy the resource before calling this method
		* to prevent memory leaks)
		* @param p_path
		*/
		void UnregisterResource(const std::filesystem::path& p_path);

		/**
		* Return the instance linked to the given path or try to load it if not registered.
		* Otherwise it will return nullptr.
		* @param p_path
		* @param p_tryToLoadIfNotFound
		*/
		T* GetResource(const std::filesystem::path& p_path, bool p_tryToLoadIfNotFound = true);

		/**
		* Operator overload to get an instance linked to the given path.
		* @note See GetResource for more informations
		* @param p_path
		*/
		T* operator[](const std::filesystem::path& p_path);

		/**
		* Provide asset paths for the resource managers
		* @important This method MUST be called before trying to access any resource from any resource manager
		* @param p_projectAssetsPath
		* @param p_engineAssetsPath
		*/
		static void ProvideAssetPaths(
			const std::filesystem::path& p_projectAssetsPath,
			const std::filesystem::path& p_engineAssetsPath
		);

		/**
		* Returns the resource map
		*/
		std::unordered_map<std::filesystem::path, T*>& GetResources();

	protected:
		virtual T* CreateResource(const std::filesystem::path& p_path) = 0;
		virtual void DestroyResource(T* p_resource) = 0;
		virtual void ReloadResource(T* p_resource, const std::filesystem::path& p_path) = 0;
		std::filesystem::path GetRealPath(const std::filesystem::path& p_path) const;

	private:
		inline static std::filesystem::path __PROJECT_ASSETS_PATH;
		inline static std::filesystem::path __ENGINE_ASSETS_PATH;

		std::unordered_map<std::filesystem::path, T*> m_resources;
	};
}

#include "OvCore/ResourceManagement/AResourceManager.inl"