/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include <algorithm>

#include "OvCore/ResourceManagement/AResourceManager.h"

namespace OvCore::ResourceManagement
{
	template<typename T>
	inline T* AResourceManager<T>::LoadResource(const std::string & p_path)
	{
		if (auto resource = GetResource(p_path, false); resource)
			return resource;
		else
		{
			auto newResource = CreateResource(p_path);
			if (newResource)
				return RegisterResource(p_path, newResource);
			else
				return nullptr;
		}
	}

	template<typename T>
	inline void AResourceManager<T>::UnloadResource(const std::string & p_path)
	{
		if (auto resource = GetResource(p_path, false); resource)
		{
			DestroyResource(resource);
			UnregisterResource(p_path);
		}
	}

	template<typename T>
	inline bool AResourceManager<T>::MoveResource(const std::string & p_previousPath, const std::string & p_newPath)
	{
		if (IsResourceRegistered(p_previousPath) && !IsResourceRegistered(p_newPath))
		{
			T* toMove = m_resources.at(p_previousPath);
			UnregisterResource(p_previousPath);
			RegisterResource(p_newPath, toMove);
			return true;
		}

		return false;
	}

	template<typename T>
	inline void AResourceManager<T>::ReloadResource(const std::string& p_path)
	{
		if (auto resource = GetResource(p_path, false); resource)
		{
			ReloadResource(resource, p_path);
		}
	}

	template<typename T>
	inline bool AResourceManager<T>::IsResourceRegistered(const std::string & p_path)
	{
		return m_resources.find(p_path) != m_resources.end();
	}

	template<typename T>
	inline void AResourceManager<T>::UnloadResources()
	{
		for (auto&[key, value] : m_resources)
			DestroyResource(value);

		m_resources.clear();
	}

	template<typename T>
	inline T* AResourceManager<T>::RegisterResource(const std::string& p_path, T* p_instance)
	{
		if (auto resource = GetResource(p_path, false); resource)
			DestroyResource(resource);

		m_resources[p_path] = p_instance;

		return p_instance;
	}

	template<typename T>
	inline void AResourceManager<T>::UnregisterResource(const std::string & p_path)
	{
		m_resources.erase(p_path);
	}

	template<typename T>
	inline T* AResourceManager<T>::GetResource(const std::string& p_path, bool p_tryToLoadIfNotFound)
	{
		if (auto resource = m_resources.find(p_path); resource != m_resources.end())
		{
			return resource->second;
		}
		else if (p_tryToLoadIfNotFound)
		{
			return LoadResource(p_path);
		}

		return nullptr;
	}

	template<typename T>
	inline T* AResourceManager<T>::operator[](const std::string & p_path)
	{
		return GetResource(p_path);
	}

	template<typename T>
	inline void AResourceManager<T>::ProvideAssetPaths(
		const std::filesystem::path& p_projectAssetsPath,
		const std::filesystem::path& p_engineAssetsPath
	)
	{
		__PROJECT_ASSETS_PATH = p_projectAssetsPath;
		__ENGINE_ASSETS_PATH = p_engineAssetsPath;
	}

	template<typename T>
	inline std::unordered_map<std::string, T*>& AResourceManager<T>::GetResources()
	{
		return m_resources;
	}

	template<typename T>
	inline std::string AResourceManager<T>::GetRealPath(const std::string& p_path) const
	{
		std::filesystem::path path;

		if (p_path.starts_with(':')) // The path is an engine path
		{
			path = __ENGINE_ASSETS_PATH / p_path.substr(1);
		}
		else // The path is a project path
		{
			path = __PROJECT_ASSETS_PATH / p_path;
		}

		return path.string();
	}
}