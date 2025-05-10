/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include <format>

#include <OvCore/ResourceManagement/TextureManager.h>
#include <OvRendering/Settings/DriverSettings.h>

#include <OvTools/Filesystem/IniFile.h>

namespace
{
	struct TextureMetaData
	{
		OvRendering::Settings::ETextureFilteringMode minFilter;
		OvRendering::Settings::ETextureFilteringMode magFilter;
		bool generateMipmap;
	};

	TextureMetaData LoadTextureMetadata(const std::string_view p_filePath)
	{
		using namespace OvRendering::Settings;
		using enum ETextureFilteringMode;

		const auto metaFile = OvTools::Filesystem::IniFile(std::format("{}.meta", p_filePath));

		return TextureMetaData{
			.minFilter = static_cast<ETextureFilteringMode>(metaFile.GetOrDefault("MIN_FILTER", static_cast<int>(LINEAR_MIPMAP_LINEAR))),
			.magFilter = static_cast<ETextureFilteringMode>(metaFile.GetOrDefault("MAG_FILTER", static_cast<int>(LINEAR))),
			.generateMipmap = metaFile.GetOrDefault("ENABLE_MIPMAPPING", true)
		};
	}
}

OvRendering::Resources::Texture* OvCore::ResourceManagement::TextureManager::CreateResource(const std::string & p_path)
{
	std::string realPath = GetRealPath(p_path);

	const auto metaData = LoadTextureMetadata(realPath);

	OvRendering::Resources::Texture* texture = OvRendering::Resources::Loaders::TextureLoader::Create(
		realPath,
		metaData.minFilter,
		metaData.magFilter,
		metaData.generateMipmap
	);

	if (texture)
		*reinterpret_cast<std::string*>(reinterpret_cast<char*>(texture) + offsetof(OvRendering::Resources::Texture, path)) = p_path; // Force the resource path to fit the given path

	return texture;
}

void OvCore::ResourceManagement::TextureManager::DestroyResource(OvRendering::Resources::Texture* p_resource)
{
	OvRendering::Resources::Loaders::TextureLoader::Destroy(p_resource);
}

void OvCore::ResourceManagement::TextureManager::ReloadResource(OvRendering::Resources::Texture* p_resource, const std::string& p_path)
{
	std::string realPath = GetRealPath(p_path);

	const auto metaData = LoadTextureMetadata(realPath);

	OvRendering::Resources::Loaders::TextureLoader::Reload(
		*p_resource,
		realPath,
		metaData.minFilter,
		metaData.magFilter,
		metaData.generateMipmap
	);
}
