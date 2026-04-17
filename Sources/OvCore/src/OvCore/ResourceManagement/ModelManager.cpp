/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include "OvCore/ResourceManagement/ModelManager.h"

#include <OvCore/Global/ServiceLocator.h>
#include <OvCore/ResourceManagement/MaterialManager.h>
#include <OvCore/ResourceManagement/TextureManager.h>
#include <OvRendering/Resources/Parsers/EmbeddedAssetPath.h>
#include <OvTools/Filesystem/IniFile.h>
#include <OvTools/Utils/PathParser.h>

namespace
{
	struct ModelMetadata
	{
		OvRendering::Resources::Parsers::EModelParserFlags parserFlags = OvRendering::Resources::Parsers::EModelParserFlags::NONE;
		bool generateEmbeddedAssets = true;
	};

	ModelMetadata GetAssetMetadata(const std::string& p_path)
	{
		auto metaFile = OvTools::Filesystem::IniFile(p_path + ".meta");
		ModelMetadata modelMetadata;

		if (metaFile.GetOrDefault("CALC_TANGENT_SPACE",			true))	modelMetadata.parserFlags |= OvRendering::Resources::Parsers::EModelParserFlags::CALC_TANGENT_SPACE;
		if (metaFile.GetOrDefault("JOIN_IDENTICAL_VERTICES",		true))	modelMetadata.parserFlags |= OvRendering::Resources::Parsers::EModelParserFlags::JOIN_IDENTICAL_VERTICES;
		if (metaFile.GetOrDefault("MAKE_LEFT_HANDED",				false))	modelMetadata.parserFlags |= OvRendering::Resources::Parsers::EModelParserFlags::MAKE_LEFT_HANDED;
		if (metaFile.GetOrDefault("TRIANGULATE",					true))	modelMetadata.parserFlags |= OvRendering::Resources::Parsers::EModelParserFlags::TRIANGULATE;
		if (metaFile.GetOrDefault("REMOVE_COMPONENT",				false))	modelMetadata.parserFlags |= OvRendering::Resources::Parsers::EModelParserFlags::REMOVE_COMPONENT;
		if (metaFile.GetOrDefault("GEN_NORMALS",					false))	modelMetadata.parserFlags |= OvRendering::Resources::Parsers::EModelParserFlags::GEN_NORMALS;
		if (metaFile.GetOrDefault("GEN_SMOOTH_NORMALS",			true))	modelMetadata.parserFlags |= OvRendering::Resources::Parsers::EModelParserFlags::GEN_SMOOTH_NORMALS;
		if (metaFile.GetOrDefault("SPLIT_LARGE_MESHES",			false))	modelMetadata.parserFlags |= OvRendering::Resources::Parsers::EModelParserFlags::SPLIT_LARGE_MESHES;
		if (metaFile.GetOrDefault("PRE_TRANSFORM_VERTICES",		false))	modelMetadata.parserFlags |= OvRendering::Resources::Parsers::EModelParserFlags::PRE_TRANSFORM_VERTICES;
		if (metaFile.GetOrDefault("LIMIT_BONE_WEIGHTS",			true))	modelMetadata.parserFlags |= OvRendering::Resources::Parsers::EModelParserFlags::LIMIT_BONE_WEIGHTS;
		if (metaFile.GetOrDefault("VALIDATE_DATA_STRUCTURE",		false))	modelMetadata.parserFlags |= OvRendering::Resources::Parsers::EModelParserFlags::VALIDATE_DATA_STRUCTURE;
		if (metaFile.GetOrDefault("IMPROVE_CACHE_LOCALITY",		true))	modelMetadata.parserFlags |= OvRendering::Resources::Parsers::EModelParserFlags::IMPROVE_CACHE_LOCALITY;
		if (metaFile.GetOrDefault("REMOVE_REDUNDANT_MATERIALS",	false))	modelMetadata.parserFlags |= OvRendering::Resources::Parsers::EModelParserFlags::REMOVE_REDUNDANT_MATERIALS;
		if (metaFile.GetOrDefault("FIX_INFACING_NORMALS",			false))	modelMetadata.parserFlags |= OvRendering::Resources::Parsers::EModelParserFlags::FIX_INFACING_NORMALS;
		if (metaFile.GetOrDefault("SORT_BY_PTYPE",					false))	modelMetadata.parserFlags |= OvRendering::Resources::Parsers::EModelParserFlags::SORT_BY_PTYPE;
		if (metaFile.GetOrDefault("FIND_DEGENERATES",				false))	modelMetadata.parserFlags |= OvRendering::Resources::Parsers::EModelParserFlags::FIND_DEGENERATES;
		if (metaFile.GetOrDefault("FIND_INVALID_DATA",				true))	modelMetadata.parserFlags |= OvRendering::Resources::Parsers::EModelParserFlags::FIND_INVALID_DATA;
		if (metaFile.GetOrDefault("GEN_UV_COORDS",					true))	modelMetadata.parserFlags |= OvRendering::Resources::Parsers::EModelParserFlags::GEN_UV_COORDS;
		if (metaFile.GetOrDefault("TRANSFORM_UV_COORDS",			false))	modelMetadata.parserFlags |= OvRendering::Resources::Parsers::EModelParserFlags::TRANSFORM_UV_COORDS;
		if (metaFile.GetOrDefault("FIND_INSTANCES",				true))	modelMetadata.parserFlags |= OvRendering::Resources::Parsers::EModelParserFlags::FIND_INSTANCES;
		if (metaFile.GetOrDefault("OPTIMIZE_MESHES",				true))	modelMetadata.parserFlags |= OvRendering::Resources::Parsers::EModelParserFlags::OPTIMIZE_MESHES;
		if (metaFile.GetOrDefault("OPTIMIZE_GRAPH",				false))	modelMetadata.parserFlags |= OvRendering::Resources::Parsers::EModelParserFlags::OPTIMIZE_GRAPH;
		if (metaFile.GetOrDefault("FLIP_UVS",						false))	modelMetadata.parserFlags |= OvRendering::Resources::Parsers::EModelParserFlags::FLIP_UVS;
		if (metaFile.GetOrDefault("FLIP_WINDING_ORDER",			false))	modelMetadata.parserFlags |= OvRendering::Resources::Parsers::EModelParserFlags::FLIP_WINDING_ORDER;
		if (metaFile.GetOrDefault("SPLIT_BY_BONE_COUNT",			false))	modelMetadata.parserFlags |= OvRendering::Resources::Parsers::EModelParserFlags::SPLIT_BY_BONE_COUNT;
		if (metaFile.GetOrDefault("DEBONE",						false))	modelMetadata.parserFlags |= OvRendering::Resources::Parsers::EModelParserFlags::DEBONE;
		if (metaFile.GetOrDefault("GLOBAL_SCALE",					true))	modelMetadata.parserFlags |= OvRendering::Resources::Parsers::EModelParserFlags::GLOBAL_SCALE;
		if (metaFile.GetOrDefault("EMBED_TEXTURES",				false))	modelMetadata.parserFlags |= OvRendering::Resources::Parsers::EModelParserFlags::EMBED_TEXTURES;
		if (metaFile.GetOrDefault("FORCE_GEN_NORMALS",				false))	modelMetadata.parserFlags |= OvRendering::Resources::Parsers::EModelParserFlags::FORCE_GEN_NORMALS;
		if (metaFile.GetOrDefault("DROP_NORMALS",					false))	modelMetadata.parserFlags |= OvRendering::Resources::Parsers::EModelParserFlags::DROP_NORMALS;
		if (metaFile.GetOrDefault("GEN_BOUNDING_BOXES",			false))	modelMetadata.parserFlags |= OvRendering::Resources::Parsers::EModelParserFlags::GEN_BOUNDING_BOXES;

		modelMetadata.generateEmbeddedAssets = metaFile.GetOrDefault("GENERATE_EMBEDDED_ASSETS", true);

		return modelMetadata;
	}

	template<typename TResourceManager, typename TIndexParser>
	void ReloadEmbeddedResourcesForModelByType(
		TResourceManager& p_resourceManager,
		const std::string& p_modelPath,
		TIndexParser p_parseIndex
	)
	{
		const std::string normalizedModelPath = OvTools::Utils::PathParser::MakeNonWindowsStyle(p_modelPath);

		for (auto& [resourcePath, resource] : p_resourceManager.GetResources())
		{
			(void)resource;
			const auto embeddedAssetPath = OvRendering::Resources::Parsers::ParseEmbeddedAssetPath(resourcePath.string());
			if (
				!embeddedAssetPath ||
				OvTools::Utils::PathParser::MakeNonWindowsStyle(embeddedAssetPath->modelPath) != normalizedModelPath
			)
			{
				continue;
			}

			if (!p_parseIndex(embeddedAssetPath->assetName))
			{
				continue;
			}

			p_resourceManager.AResourceManager::ReloadResource(resourcePath);
		}
	}

	void ReloadEmbeddedModelResources(const std::string& p_modelPath)
	{
		ReloadEmbeddedResourcesForModelByType(
			OvCore::Global::ServiceLocator::Get<OvCore::ResourceManagement::MaterialManager>(),
			p_modelPath,
			[](const std::string& p_assetName) { return OvRendering::Resources::Parsers::ParseEmbeddedMaterialIndex(p_assetName).has_value(); }
		);

		ReloadEmbeddedResourcesForModelByType(
			OvCore::Global::ServiceLocator::Get<OvCore::ResourceManagement::TextureManager>(),
			p_modelPath,
			[](const std::string& p_assetName) { return OvRendering::Resources::Parsers::ParseEmbeddedTextureIndex(p_assetName).has_value(); }
		);
	}
}

OvRendering::Resources::Model* OvCore::ResourceManagement::ModelManager::CreateResource(const std::filesystem::path& p_path)
{
	std::string realPath = GetRealPath(p_path).string();
	const auto metadata = GetAssetMetadata(realPath);
	auto model = OvRendering::Resources::Loaders::ModelLoader::Create(
		realPath,
		metadata.parserFlags,
		metadata.generateEmbeddedAssets
	);
	if (model)
	{
		const_cast<std::string&>(model->path) = p_path.string(); // Force the resource path to fit the given path
	}

	return model;
}

void OvCore::ResourceManagement::ModelManager::DestroyResource(OvRendering::Resources::Model* p_resource)
{
	OvRendering::Resources::Loaders::ModelLoader::Destroy(p_resource);
}

void OvCore::ResourceManagement::ModelManager::ReloadResource(OvRendering::Resources::Model* p_resource, const std::filesystem::path& p_path)
{
	std::string realPath = GetRealPath(p_path).string();
	const auto metadata = GetAssetMetadata(realPath);
	OvRendering::Resources::Loaders::ModelLoader::Reload(
		*p_resource,
		realPath,
		metadata.parserFlags,
		metadata.generateEmbeddedAssets
	);

	ReloadEmbeddedModelResources(p_path.string());
}
