#pragma once

#include <any>
#include <cstdint>
#include <optional>

namespace OvCore::Assets
{
	using GUID = uint32_t;

	GUID GenerateGUID()
	{
		static GUID id = 0;
		return ++id;
	}

	using AssetID = GUID;

	/**
	* Can be seen as a reference to something on the disk (File).
	*/
	class File
	{
	public:
		File(const std::string_view p_path) : m_path(p_path) {}
		bool Exists() const { return std::filesystem::exists(m_path); }
		void MoveTo(const std::string_view p_newPath) { m_path = p_newPath; }
		void Delete() { std::filesystem::remove(m_path); }
		void SaveToDisk() { _Write(_Serialize()); }
		void LoadFromDisk() { _Deserialize(_Read()); }
		const std::string& GetPath() const { return m_path; }

	protected:
		std::string _Serialize() { return std::string{}; }
		void _Deserialize(const std::string& p_data) { /*...*/ }

	private:
		void _Write(const std::string& p_content) {}; // Write to disk
		std::string _Read() {}; // Read from disk

	protected:
		std::string m_path;
	};
	
	/**
	* An asset is a file that can be loaded and used in the engine, and is identified by a GUID.
	*/
	class AAssetBase : public File
	{
	public:
		// Make sure only the asset registry can create assets
		AAssetBase(const std::string_view p_path) :
			m_id(GenerateGUID()),
			File(p_path)
		{
		}

		void Reload()
		{
			Unload();
			Load();
		}

		// Synchronous loading
		virtual void Load() = 0;
		virtual void Unload() = 0;
		virtual bool IsLoaded() const = 0;

	private:
		AssetID m_id;
		std::string m_path;
	};

	class Mock
	{
	public:
		Mock(int x, int y) {}
	};

	// Is this even needed?
	class MockFactory
	{
	public:
		Mock&& Create(int x, int y)
		{
			return Mock(x, y);
		}

		Mock&& CreateFromPath(std::string p_path)
		{
			// Load file, parse, etc...
			return Mock(0, 0);
		}
	};

	template<class UnderlyingType>
	class Asset : public AAssetBase
	{
	public:
		Asset(const std::string_view p_path) : AAssetBase(p_path) {}

		// Template specialized for each asset type
		void Load() override;

		void Unload() override
		{
			m_underlying.reset();
		}

		bool IsLoaded() const override
		{
			return m_underlying.has_value();
		}

	private:
		std::optional<UnderlyingType> m_underlying; // Ideally this stays hidden, no getter.
	};

	// Template specialization for Mock
	void Asset<Mock>::Load()
	{
		auto factory = MockFactory{};
		m_underlying = std::make_optional<Mock>(factory.Create(1, 2));
	}
}

#include <iostream>
#include <OvCore/Global/ServiceLocator.h>

namespace
{
	// Example use case
	int main()
	{
		using namespace OvCore::Assets;
		using namespace OvCore::Global;

		// Somewhere at init
		MockFactory factory;
		ServiceLocator::Provide(factory);

		// This should be done in the asset registry
		{
			const auto asset = Asset<Mock>{ "path/to/asset.ovmock" };
		}

		// The asset registry should only return a reference to the asset
		const AssetRef<Mock> mockAssetRef; // Let's assume this is returned by the asset registry

		// Check if the asset reference is valid (not expired)
		if (mockAssetRef.IsValid()) // same as `if (mockAssetRef)` with operator bool. This only checks that the reference is valid, not that the asset is loaded.
		{
			// -> to access the underlying MockAsset&
			Asset<Mock>& asset = mockAssetRef.Get(); // Shouldn't fail since IsValid() returned true

			// If the asset is not loaded, this will load it.
			// A fallback strategy could be added for asynchronous loading.
			// e.g. while the asset is loading, use a placeholder asset.
			Mock& mock = asset.Get();

			// TODO: Consider using some sort of monad to handle the double indirection.
		}

		// Should print "Is loaded: false"
		std::cout << std::format("Is loaded: {}", asset.IsLoaded()) << std::endl;

		if (auto& mockInstance = mockAssetRef->GetUnderlying())
		{

		}

		// Constructor a mock instance
		auto mockInstance = OvCore::Global::ServiceLocator::Get<MockFactory>().Create(1, 2);


		return 0;
	}
}