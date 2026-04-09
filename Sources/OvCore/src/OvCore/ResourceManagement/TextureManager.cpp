/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include <filesystem>
#include <format>
#include <optional>

#include <OvCore/Global/ServiceLocator.h>
#include <OvCore/ResourceManagement/ModelManager.h>
#include <OvCore/ResourceManagement/TextureManager.h>
#include <OvRendering/Resources/Parsers/EmbeddedAssetPath.h>
#include <OvRendering/Settings/DriverSettings.h>

#include <OvTools/Filesystem/IniFile.h>
#include <OvTools/Utils/PathParser.h>

namespace
{
	struct TextureMetaData
	{
		OvRendering::Settings::ETextureFilteringMode minFilter;
		OvRendering::Settings::ETextureFilteringMode magFilter;
		OvRendering::Settings::ETextureWrapMode horizontalWrap;
		OvRendering::Settings::ETextureWrapMode verticalWrap;
		bool generateMipmap;
	};

	TextureMetaData GetDefaultTextureMetadata()
	{
		using namespace OvRendering::Settings;
		using enum ETextureFilteringMode;
		using enum ETextureWrapMode;

		return TextureMetaData{
			.minFilter = LINEAR_MIPMAP_LINEAR,
			.magFilter = LINEAR,
			.horizontalWrap = REPEAT,
			.verticalWrap = REPEAT,
			.generateMipmap = true
		};
	}

	TextureMetaData LoadTextureMetadata(const std::string_view p_filePath)
	{
		using namespace OvRendering::Settings;
		using enum ETextureFilteringMode;
		using enum ETextureWrapMode;

		const auto metaFile = OvTools::Filesystem::IniFile(std::format("{}.meta", p_filePath));

		return TextureMetaData{
			.minFilter = static_cast<ETextureFilteringMode>(metaFile.GetOrDefault("MIN_FILTER", static_cast<int>(LINEAR_MIPMAP_LINEAR))),
			.magFilter = static_cast<ETextureFilteringMode>(metaFile.GetOrDefault("MAG_FILTER", static_cast<int>(LINEAR))),
			.horizontalWrap = static_cast<ETextureWrapMode>(metaFile.GetOrDefault("HORIZONTAL_WRAP", static_cast<int>(REPEAT))),
			.verticalWrap = static_cast<ETextureWrapMode>(metaFile.GetOrDefault("VERTICAL_WRAP", static_cast<int>(REPEAT))),
			.generateMipmap = metaFile.GetOrDefault("ENABLE_MIPMAPPING", true)
		};
	}

	struct EmbeddedTextureContext
	{
		std::string modelPath;
		const OvRendering::Resources::EmbeddedTextureData* textureData = nullptr;
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

		const auto* embeddedTexture = model->GetEmbeddedTexture(textureIndex.value());
		if (!embeddedTexture)
		{
			return std::nullopt;
		}

		return EmbeddedTextureContext{
			.modelPath = embeddedAssetPath->modelPath,
			.textureData = embeddedTexture
		};
	}

	std::string ResolveRuntimeExternalTexturePath(const std::string& p_sourcePath, const std::string& p_modelRealPath)
	{
		auto sourcePath = std::filesystem::path{ OvTools::Utils::PathParser::MakeNonWindowsStyle(p_sourcePath) };
		if (sourcePath.is_relative())
		{
			sourcePath = std::filesystem::path{ p_modelRealPath }.parent_path() / sourcePath;
		}

		std::error_code errorCode;
		sourcePath = std::filesystem::weakly_canonical(sourcePath, errorCode);
		if (errorCode)
		{
			sourcePath = sourcePath.lexically_normal();
		}

		return sourcePath.string();
	}

	OvRendering::Resources::Texture* CreateEmbeddedTexture(
		const std::filesystem::path& p_resourcePath,
		const EmbeddedTextureContext& p_context,
		const std::string& p_modelRealPath
	)
	{
		const auto settings = GetDefaultTextureMetadata();
		const auto& textureData = *p_context.textureData;
		using SourceType = OvRendering::Resources::EmbeddedTextureData::ESourceType;

		OvRendering::Resources::Texture* texture = nullptr;

		switch (textureData.sourceType)
		{
		case SourceType::EXTERNAL_FILE:
			if (!textureData.sourcePath.empty())
			{
				const auto realTexturePath = ResolveRuntimeExternalTexturePath(textureData.sourcePath, p_modelRealPath);
				texture = OvRendering::Resources::Loaders::TextureLoader::Create(
					realTexturePath,
					settings.minFilter,
					settings.magFilter,
					settings.horizontalWrap,
					settings.verticalWrap,
					settings.generateMipmap
				);
			}
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
		const EmbeddedTextureContext& p_context,
		const std::string& p_modelRealPath
	)
	{
		const auto settings = GetDefaultTextureMetadata();
		const auto& textureData = *p_context.textureData;
		using SourceType = OvRendering::Resources::EmbeddedTextureData::ESourceType;

		switch (textureData.sourceType)
		{
		case SourceType::EXTERNAL_FILE:
			if (!textureData.sourcePath.empty())
			{
				const auto realTexturePath = ResolveRuntimeExternalTexturePath(textureData.sourcePath, p_modelRealPath);
				OvRendering::Resources::Loaders::TextureLoader::Reload(
					p_texture,
					realTexturePath,
					settings.minFilter,
					settings.magFilter,
					settings.horizontalWrap,
					settings.verticalWrap,
					settings.generateMipmap
				);
			}
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
		const auto modelRealPath = GetRealPath(embeddedTextureContext->modelPath).string();
		return CreateEmbeddedTexture(p_path, embeddedTextureContext.value(), modelRealPath);
	}

	std::string realPath = GetRealPath(p_path).string();

	const auto metaData = LoadTextureMetadata(realPath);

	OvRendering::Resources::Texture* texture = OvRendering::Resources::Loaders::TextureLoader::Create(
		realPath,
		metaData.minFilter,
		metaData.magFilter,
		metaData.horizontalWrap,
		metaData.verticalWrap,
		metaData.generateMipmap
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
		const auto modelRealPath = GetRealPath(embeddedTextureContext->modelPath).string();
		ReloadEmbeddedTexture(*p_resource, embeddedTextureContext.value(), modelRealPath);
		return;
	}

	std::string realPath = GetRealPath(p_path).string();

	const auto metaData = LoadTextureMetadata(realPath);

	OvRendering::Resources::Loaders::TextureLoader::Reload(
		*p_resource,
		realPath,
		metaData.minFilter,
		metaData.magFilter,
		metaData.horizontalWrap,
		metaData.verticalWrap,
		metaData.generateMipmap
	);
}
