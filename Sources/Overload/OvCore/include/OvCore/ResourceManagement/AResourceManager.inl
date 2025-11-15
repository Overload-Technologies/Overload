/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include <algorithm>

#include "OvCore/ResourceManagement/AResourceManager.h"

namespace
{
	std::string SanitizePath(const std::string& p_path)
	{
		std::string sanitizedPath = p_path;
		std::replace(sanitizedPath.begin(), sanitizedPath.end(), '/', std::filesystem::path::preferred_separator);
		std::replace(sanitizedPath.begin(), sanitizedPath.end(), '\\', std::filesystem::path::preferred_separator);
		return sanitizedPath;
	}
}

namespace OvCore::ResourceManagement
{
	template<typename T>
	inline T* AResourceManager<T>::LoadResource(const std::string & p_path)
	{
		auto sanitizedPath = SanitizePath(p_path);

		if (auto resource = GetResource(sanitizedPath, false); resource)
			return resource;
		else
		{
			auto newResource = CreateResource(sanitizedPath);
			if (newResource)
				return RegisterResource(sanitizedPath, newResource);
			else
				return nullptr;
		}
	}

	template<typename T>
	inline void AResourceManager<T>::UnloadResource(const std::string & p_path)
	{
		auto sanitizedPath = SanitizePath(p_path);

		if (auto resource = GetResource(sanitizedPath, false); resource)
		{
			DestroyResource(resource);
			UnregisterResource(sanitizedPath);
		}
	}

	template<typename T>
	inline bool AResourceManager<T>::MoveResource(const std::string & p_previousPath, const std::string & p_newPath)
	{
		auto sanitizedPreviousPath = SanitizePath(p_previousPath);
		auto sanitizedNewPath = SanitizePath(p_newPath);

		if (IsResourceRegistered(sanitizedPreviousPath) && !IsResourceRegistered(sanitizedNewPath))
		{
			T* toMove = m_resources.at(sanitizedPreviousPath);
			UnregisterResource(sanitizedPreviousPath);
			RegisterResource(sanitizedNewPath, toMove);
			return true;
		}

		return false;
	}

	template<typename T>
	inline void AResourceManager<T>::ReloadResource(const std::string& p_path)
	{
		auto sanitizedPath = SanitizePath(p_path);
		
		if (auto resource = GetResource(sanitizedPath, false); resource)
		{
			ReloadResource(resource, sanitizedPath);
		}
	}

	template<typename T>
	inline bool AResourceManager<T>::IsResourceRegistered(const std::string & p_path)
	{
		auto sanitizedPath = SanitizePath(p_path);
		return m_resources.find(sanitizedPath) != m_resources.end();
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
		auto sanitizedPath = SanitizePath(p_path);

		if (auto resource = GetResource(sanitizedPath, false); resource)
		{
			DestroyResource(resource);
		}

		m_resources[sanitizedPath] = p_instance;

		return p_instance;
	}

	template<typename T>
	inline void AResourceManager<T>::UnregisterResource(const std::string & p_path)
	{
		auto sanitizedPath = SanitizePath(p_path);
		m_resources.erase(sanitizedPath);
	}

	template<typename T>
	inline T* AResourceManager<T>::GetResource(const std::string& p_path, bool p_tryToLoadIfNotFound)
	{
		auto sanitizedPath = SanitizePath(p_path);
		
		if (auto resource = m_resources.find(sanitizedPath); resource != m_resources.end())
		{
			return resource->second;
		}
		else if (p_tryToLoadIfNotFound)
		{
			return LoadResource(sanitizedPath);
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