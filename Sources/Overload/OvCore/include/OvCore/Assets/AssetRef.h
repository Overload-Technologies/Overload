#pragma once

#include <concepts>
#include <optional>
#include <OvCore/Assets/Asset.h>
#include <OvCore/Global/ServiceLocator.h>

namespace OvCore::Assets
{
	template<typename T>
	concept AssetType = std::is_base_of<Asset, T>::value;

	/**
	* A simple asset reference. Doesn't need the underlying asset to be loaded.
	*/
	struct GenericAssetRef
	{
		AssetID id;
	};

	/**
	* Provide utility functions to manage the underlying asset.
	*/
	template<AssetType T>
	class AssetRef : public GenericAssetRef
	{
	public:
		AssetID id;

		/**
		* Returns the underlying asset.
		*/
		std::optional<T> Get()
		{
			auto& assetRegistry = OvCore::Global::ServiceLocator::Get<AssetRegistry>();
			return assetRegistry.GetAsset(id);
			return static_cast<T&>(m_assets[id]);
		}

		bool IsExpired() const
		{
			auto& assetRegistry = OvCore::Global::ServiceLocator::Get<AssetRegistry>();
			return assetRegistry.
		}
	};
}
