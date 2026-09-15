#pragma once

#include <filesystem>
#include <unordered_map>

#include <OvCore/Assets/Asset.h>
#include <OvCore/Assets/AssetRef.h>

namespace OvCore::Assets
{
	class AssetRegistry
	{
	public:
		/**
		* Discover assets in the given path and register them in the asset registry.
		* @note This is quite expensive, and should probably be called only once at the beginning of the application.
		* @param p_path
		* @param p_recursive
		*/
		uint32_t DiscoverAssets(const std::filesystem::path& p_path, bool p_recursive = true);

		/**
		* Register a single asset in the asset registry.
		* @param p_path
		*/
		GenericAssetRef RegisterAssetAtPath(const std::filesystem::path& p_path);

		/**
		* Returns a reference to the asset with the given ID.
		* @param p_id
		*/
		Asset& GetAsset(uint32_t p_id)
		{
			auto it = m_assets.find(p_id);
			if (it != m_assets.end())
			{
				return it->second;
			}
			else
			{
				throw std::runtime_error("Asset not found");
			}
		}

	private:
		std::unordered_map<uint32_t, Asset> m_assets;
	};
}
