/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include <filesystem>
#include <format>
#include <optional>

#include <OvDebug/Assertion.h>
#include <OvCore/Global/ServiceLocator.h>
#include <OvCore/ResourceManagement/ModelManager.h>
#include <OvCore/ResourceManagement/TextureManager.h>
#include <OvRendering/Resources/Parsers/EmbeddedAssetPath.h>
#include <OvRendering/Settings/DriverSettings.h>

#include <OvTools/Filesystem/IniFile.h>

namespace
{
	struct TextureMetadata
	{
		OvRendering::Settings::ETextureFilteringMode minFilter = OvRendering::Settings::ETextureFilteringMode::LINEAR_MIPMAP_LINEAR;
		OvRendering::Settings::ETextureFilteringMode magFilter = OvRendering::Settings::ETextureFilteringMode::LINEAR;
		OvRendering::Settings::ETextureWrapMode horizontalWrap = OvRendering::Settings::ETextureWrapMode::REPEAT;
		OvRendering::Settings::ETextureWrapMode verticalWrap = OvRendering::Settings::ETextureWrapMode::REPEAT;
		bool generateMipmap = true;
	};

	TextureMetadata LoadTextureMetadata(const std::string_view p_filePath)
	{
		const auto metaFile = OvTools::Filesystem::IniFile(std::format("{}.meta", p_filePath));
		auto metadata = TextureMetadata{};

		metadata.minFilter = static_cast<OvRendering::Settings::ETextureFilteringMode>(metaFile.GetOrDefault("MIN_FILTER", static_cast<int>(metadata.minFilter)));
		metadata.magFilter = static_cast<OvRendering::Settings::ETextureFilteringMode>(metaFile.GetOrDefault("MAG_FILTER", static_cast<int>(metadata.magFilter)));
		metadata.horizontalWrap = static_cast<OvRendering::Settings::ETextureWrapMode>(metaFile.GetOrDefault("HORIZONTAL_WRAP", static_cast<int>(metadata.horizontalWrap)));
		metadata.verticalWrap = static_cast<OvRendering::Settings::ETextureWrapMode>(metaFile.GetOrDefault("VERTICAL_WRAP", static_cast<int>(metadata.verticalWrap)));
		metadata.generateMipmap = metaFile.GetOrDefault("ENABLE_MIPMAPPING", metadata.generateMipmap);

		return metadata;
	}

	struct EmbeddedTextureContext
	{
		const OvRendering::Resources::EmbeddedTextureData& textureData;
	};

	std::optional<EmbeddedTextureContext> ResolveEmbeddedTextureContext(const std::filesystem::path& p_path)
	{
		using namespace OvRendering::Resources::Parsers;

		const auto embeddedAssetPath = ParseEmbeddedAssetPath(p_path.string());
		if (!embeddedAssetPath)
		{
			return std::nullopt;
		}

		const auto textureIndex = ParseEmbeddedTextureIndex(embeddedAssetPath->assetName);
		if (!textureIndex)
		{
			return std::nullopt;
		}

		auto* model = OvCore::Global::ServiceLocator::Get<OvCore::ResourceManagement::ModelManager>().GetResource(embeddedAssetPath->modelPath);
		if (!model)
		{
			return std::nullopt;
		}

		const auto embeddedTexture = model->GetEmbeddedTexture(textureIndex.value());
		if (!embeddedTexture)
		{
			return std::nullopt;
		}

		return EmbeddedTextureContext{
			.textureData = embeddedTexture.value()
		};
	}

	OvRendering::Resources::Texture* CreateEmbeddedTexture(
		const std::filesystem::path& p_resourcePath,
		const EmbeddedTextureContext& p_context
	)
	{
		const auto settings = TextureMetadata{};
		const auto& textureData = p_context.textureData;
		using SourceType = OvRendering::Resources::EmbeddedTextureData::ESourceType;

		OvRendering::Resources::Texture* texture = nullptr;

		switch (textureData.sourceType)
		{
		case SourceType::EXTERNAL_FILE:
			OVASSERT(false, "External embedded textures should resolve to regular texture resources.");
			break;

		case SourceType::EMBEDDED_COMPRESSED:
			if (!textureData.compressedData.empty())
			{
				texture = OvRendering::Resources::Loaders::TextureLoader::CreateFromEncodedMemory(
					textureData.compressedData.data(),
					textureData.compressedData.size(),
					settings.minFilter,
					settings.magFilter,
					settings.horizontalWrap,
					settings.verticalWrap,
					settings.generateMipmap
				);
			}
			break;

		case SourceType::EMBEDDED_RAW_RGBA8:
			if (!textureData.rawRGBAData.empty() && textureData.width > 0 && textureData.height > 0)
			{
				texture = OvRendering::Resources::Loaders::TextureLoader::CreateFromMemory(
					textureData.rawRGBAData.data(),
					textureData.width,
					textureData.height,
					settings.minFilter,
					settings.magFilter,
					settings.horizontalWrap,
					settings.verticalWrap,
					settings.generateMipmap
				);
			}
			break;
		}

		if (texture)
		{
			const_cast<std::string&>(texture->path) = p_resourcePath.string();
		}

		return texture;
	}

	void ReloadEmbeddedTexture(
		OvRendering::Resources::Texture& p_texture,
		const EmbeddedTextureContext& p_context
	)
	{
		const auto settings = TextureMetadata{};
		const auto& textureData = p_context.textureData;
		using SourceType = OvRendering::Resources::EmbeddedTextureData::ESourceType;

		switch (textureData.sourceType)
		{
		case SourceType::EXTERNAL_FILE:
			OVASSERT(false, "External embedded textures should resolve to regular texture resources.");
			break;

		case SourceType::EMBEDDED_COMPRESSED:
			if (!textureData.compressedData.empty())
			{
				OvRendering::Resources::Loaders::TextureLoader::ReloadFromEncodedMemory(
					p_texture,
					textureData.compressedData.data(),
					textureData.compressedData.size(),
					settings.minFilter,
					settings.magFilter,
					settings.horizontalWrap,
					settings.verticalWrap,
					settings.generateMipmap
				);
			}
			break;

		case SourceType::EMBEDDED_RAW_RGBA8:
			if (!textureData.rawRGBAData.empty() && textureData.width > 0 && textureData.height > 0)
			{
				OvRendering::Resources::Loaders::TextureLoader::ReloadFromMemory(
					p_texture,
					textureData.rawRGBAData.data(),
					textureData.width,
					textureData.height,
					settings.minFilter,
					settings.magFilter,
					settings.horizontalWrap,
					settings.verticalWrap,
					settings.generateMipmap
				);
			}
			break;
		}
	}
}

OvRendering::Resources::Texture* OvCore::ResourceManagement::TextureManager::CreateResource(const std::filesystem::path & p_path)
{
	if (const auto embeddedTextureContext = ResolveEmbeddedTextureContext(p_path))
	{
		return CreateEmbeddedTexture(p_path, embeddedTextureContext.value());
	}

	std::string realPath = GetRealPath(p_path).string();

	const auto metadata = LoadTextureMetadata(realPath);

	OvRendering::Resources::Texture* texture = OvRendering::Resources::Loaders::TextureLoader::Create(
		realPath,
		metadata.minFilter,
		metadata.magFilter,
		metadata.horizontalWrap,
		metadata.verticalWrap,
		metadata.generateMipmap
	);

	if (texture)
	{
		const_cast<std::string&>(texture->path) = p_path.string(); // Force the resource path to fit the given path
	}

	return texture;
}

void OvCore::ResourceManagement::TextureManager::DestroyResource(OvRendering::Resources::Texture* p_resource)
{
	OvRendering::Resources::Loaders::TextureLoader::Destroy(p_resource);
}

void OvCore::ResourceManagement::TextureManager::ReloadResource(OvRendering::Resources::Texture* p_resource, const std::filesystem::path& p_path)
{
	if (const auto embeddedTextureContext = ResolveEmbeddedTextureContext(p_path))
	{
		ReloadEmbeddedTexture(*p_resource, embeddedTextureContext.value());
		return;
	}

	std::string realPath = GetRealPath(p_path).string();

	const auto metadata = LoadTextureMetadata(realPath);

	OvRendering::Resources::Loaders::TextureLoader::Reload(
		*p_resource,
		realPath,
		metadata.minFilter,
		metadata.magFilter,
		metadata.horizontalWrap,
		metadata.verticalWrap,
		metadata.generateMipmap
	);
}
