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
	// Normalize a path for consistent key usage across platforms.
	// - Treat backslashes as separators (important on POSIX where '\\' isn't a separator)
	// - Apply lexically_normal to collapse '.' and '..'
	inline std::filesystem::path NormalizeKey(const std::filesystem::path& p)
	{
		std::string s = p.string();
		std::replace(s.begin(), s.end(), '\\', '/');
		std::filesystem::path norm = std::filesystem::path{s}.lexically_normal();
		return norm;
	}
}

namespace OvCore::ResourceManagement
{
	template<typename T>
	inline T* AResourceManager<T>::LoadResource(const std::filesystem::path & p_path)
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
	inline void AResourceManager<T>::UnloadResource(const std::filesystem::path & p_path)
	{
		if (auto resource = GetResource(p_path, false); resource)
		{
			DestroyResource(resource);
			UnregisterResource(p_path);
		}
	}

	template<typename T>
	inline bool AResourceManager<T>::MoveResource(const std::filesystem::path & p_previousPath, const std::filesystem::path & p_newPath)
	{
		if (auto toMove = GetResource(p_previousPath, false); toMove && !IsResourceRegistered(p_newPath))
		{
			RegisterResource(p_newPath, toMove);
			UnregisterResource(p_previousPath);
			return true;
		}

		return false;
	}

	template<typename T>
	inline void AResourceManager<T>::ReloadResource(const std::filesystem::path& p_path)
	{
		if (auto resource = GetResource(p_path, false); resource)
		{
			ReloadResource(resource, p_path);
		}
	}

	template<typename T>
	inline bool AResourceManager<T>::IsResourceRegistered(const std::filesystem::path & p_path)
	{
		return m_resources.find(NormalizeKey(p_path)) != m_resources.end();
	}

	template<typename T>
	inline void AResourceManager<T>::UnloadResources()
	{
		for (auto&[key, value] : m_resources)
			DestroyResource(value);

		m_resources.clear();
	}

	template<typename T>
	inline T* AResourceManager<T>::RegisterResource(const std::filesystem::path& p_path, T* p_instance)
	{
		auto key = NormalizeKey(p_path);

		if (auto resource = GetResource(p_path, false); resource)
		{
			DestroyResource(resource);
		}

		m_resources[key] = p_instance;

		return p_instance;
	}

	template<typename T>
	inline void AResourceManager<T>::UnregisterResource(const std::filesystem::path & p_path)
	{
		m_resources.erase(NormalizeKey(p_path));
	}

	template<typename T>
	inline T* AResourceManager<T>::GetResource(const std::filesystem::path& p_path, bool p_tryToLoadIfNotFound)
	{
		auto key = NormalizeKey(p_path);
		
		if (auto resource = m_resources.find(key); resource != m_resources.end())
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
	inline T* AResourceManager<T>::operator[](const std::filesystem::path & p_path)
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
	inline std::unordered_map<std::filesystem::path, T*>& AResourceManager<T>::GetResources()
	{
		return m_resources;
	}

	template<typename T>
	inline std::filesystem::path AResourceManager<T>::GetRealPath(const std::filesystem::path& p_path) const
	{
		// Keep support for the engine prefix ':'
		const std::string s = p_path.string();
		std::filesystem::path path;
		if (!s.empty() && s.front() == ':')
		{
			// Remove the ':' prefix and normalize separators
			std::string sub = s.substr(1);
			std::replace(sub.begin(), sub.end(), '\\', '/');
			path = __ENGINE_ASSETS_PATH / std::filesystem::path{sub};
		}
		else
		{
			std::string sub = s;
			std::replace(sub.begin(), sub.end(), '\\', '/');
			path = __PROJECT_ASSETS_PATH / std::filesystem::path{sub};
		}

		return path.lexically_normal();
	}
}