/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include "OvCore/ResourceManagement/MaterialManager.h"

#include <filesystem>
#include <optional>
#include <string_view>
#include <utility>

#include <OvCore/Global/ServiceLocator.h>
#include <OvCore/ResourceManagement/ModelManager.h>
#include <OvCore/ResourceManagement/ShaderManager.h>
#include <OvCore/ResourceManagement/TextureManager.h>
#include <OvRendering/Resources/Parsers/EmbeddedAssetPath.h>
#include <OvTools/Utils/PathParser.h>

namespace
{
	struct EmbeddedMaterialContext
	{
		std::string modelPath;
		const OvRendering::Resources::Model* model = nullptr;
		const OvRendering::Resources::EmbeddedMaterialData* materialData = nullptr;
	};

	std::optional<EmbeddedMaterialContext> ResolveEmbeddedMaterialContext(const std::filesystem::path& p_path)
	{
		using namespace OvRendering::Resources::Parsers;

		const auto embeddedAssetPath = ParseEmbeddedAssetPath(p_path.string());
		if (!embeddedAssetPath)
		{
			return std::nullopt;
		}

		const auto materialIndex = ParseEmbeddedMaterialIndex(embeddedAssetPath->assetName);
		if (!materialIndex)
		{
			return std::nullopt;
		}

		auto* model = OvCore::Global::ServiceLocator::Get<OvCore::ResourceManagement::ModelManager>().GetResource(embeddedAssetPath->modelPath);
		if (!model)
		{
			return std::nullopt;
		}

		const auto embeddedMaterial = model->GetEmbeddedMaterial(materialIndex.value());
		if (!embeddedMaterial)
		{
			return std::nullopt;
		}

		return EmbeddedMaterialContext{
			.modelPath = embeddedAssetPath->modelPath,
			.model = model,
			.materialData = &embeddedMaterial.value()
		};
	}

	std::optional<std::string> ResolveLinkedTextureResourcePath(const std::string& p_modelPath, const std::string& p_sourcePath)
	{
		const std::string normalizedSourcePath = OvTools::Utils::PathParser::MakeNonWindowsStyle(p_sourcePath);
		if (normalizedSourcePath.empty())
		{
			return std::nullopt;
		}

		if (normalizedSourcePath.starts_with(':'))
		{
			return OvTools::Utils::PathParser::MakeWindowsStyle(normalizedSourcePath);
		}

		auto sourcePath = std::filesystem::path{ normalizedSourcePath };
		if (sourcePath.is_absolute())
		{
			return std::nullopt;
		}

		const auto modelPath = std::filesystem::path{ OvTools::Utils::PathParser::MakeNonWindowsStyle(p_modelPath) };
		const auto linkedTexturePath = (modelPath.parent_path() / sourcePath).lexically_normal();
		return OvTools::Utils::PathParser::MakeWindowsStyle(linkedTexturePath.string());
	}

	bool BindEmbeddedTextureProperty(
		OvCore::Resources::Material& p_material,
		const EmbeddedMaterialContext& p_context,
		const std::optional<uint32_t>& p_textureIndex,
		const std::string_view p_uniformName
	)
	{
		if (!p_textureIndex.has_value())
		{
			return false;
		}

		auto& textureManager = OvCore::Global::ServiceLocator::Get<OvCore::ResourceManagement::TextureManager>();
		const auto embeddedTexture = p_context.model->GetEmbeddedTexture(p_textureIndex.value());
		if (!embeddedTexture)
		{
			return false;
		}

		const auto& textureData = embeddedTexture.value();
		using SourceType = OvRendering::Resources::EmbeddedTextureData::ESourceType;
		if (textureData.sourceType == SourceType::EXTERNAL_FILE)
		{
			if (const auto linkedTexturePath = ResolveLinkedTextureResourcePath(p_context.modelPath, textureData.sourcePath))
			{
				if (auto* linkedTexture = textureManager.GetResource(linkedTexturePath.value()))
				{
					p_material.TrySetProperty(std::string{ p_uniformName }, linkedTexture);
					return true;
				}
			}

			return false;
		}

		const std::string extension = textureData.extension.empty() ? "bin" : textureData.extension;
		const std::string texturePath = OvRendering::Resources::Parsers::MakeEmbeddedTexturePath(
			p_context.modelPath,
			p_textureIndex.value(),
			extension
		);

		if (auto* texture = textureManager.GetResource(texturePath))
		{
			p_material.TrySetProperty(std::string{ p_uniformName }, texture);
			return true;
		}

		return false;
	}

	bool ConfigureEmbeddedMaterial(
		OvCore::Resources::Material& p_material,
		const EmbeddedMaterialContext& p_context,
		const OvCore::ResourceManagement::MaterialManager::StandardShaderDefinition& p_shaderDefinition
	)
	{
		auto* shader = OvCore::Global::ServiceLocator::Get<OvCore::ResourceManagement::ShaderManager>()[p_shaderDefinition.shaderPath];
		if (!shader)
		{
			return false;
		}

		const auto& materialData = *p_context.materialData;

		p_material.SetShader(shader);
		p_material.SetFeatures(OvRendering::Data::FeatureSet{});

		p_material.TrySetProperty(p_shaderDefinition.albedoUniform, materialData.albedo);
		p_material.TrySetProperty(p_shaderDefinition.metallicUniform, materialData.metallic);
		p_material.TrySetProperty(p_shaderDefinition.roughnessUniform, materialData.roughness);
		p_material.TrySetProperty(p_shaderDefinition.emissiveColorUniform, materialData.emissiveColor);
		p_material.TrySetProperty(p_shaderDefinition.emissiveIntensityUniform, materialData.emissiveIntensity);

		const bool normalTextureBound = BindEmbeddedTextureProperty(p_material, p_context, materialData.normalTexture, p_shaderDefinition.normalMapUniform);
		const bool hasDistinctHeightTexture =
			materialData.heightTexture.has_value() &&
			(
				!materialData.normalTexture.has_value() ||
				materialData.heightTexture.value() != materialData.normalTexture.value()
			);
		const bool heightTextureBound = hasDistinctHeightTexture &&
			BindEmbeddedTextureProperty(p_material, p_context, materialData.heightTexture, p_shaderDefinition.heightMapUniform);

		BindEmbeddedTextureProperty(p_material, p_context, materialData.albedoTexture, p_shaderDefinition.albedoMapUniform);
		BindEmbeddedTextureProperty(p_material, p_context, materialData.metallicTexture, p_shaderDefinition.metallicMapUniform);
		BindEmbeddedTextureProperty(p_material, p_context, materialData.roughnessTexture, p_shaderDefinition.roughnessMapUniform);
		BindEmbeddedTextureProperty(p_material, p_context, materialData.ambientOcclusionTexture, p_shaderDefinition.ambientOcclusionMapUniform);
		BindEmbeddedTextureProperty(p_material, p_context, materialData.emissiveTexture, p_shaderDefinition.emissiveMapUniform);
		BindEmbeddedTextureProperty(p_material, p_context, materialData.opacityTexture, p_shaderDefinition.opacityMapUniform);

		if (normalTextureBound)
		{
			p_material.AddFeature(p_shaderDefinition.normalMappingFeature);
		}

		if (heightTextureBound)
		{
			p_material.AddFeature(p_shaderDefinition.parallaxMappingFeature);
		}

		return true;
	}

	bool ResetMissingEmbeddedMaterial(
		OvCore::Resources::Material& p_material,
		const OvCore::ResourceManagement::MaterialManager::StandardShaderDefinition& p_shaderDefinition
	)
	{
		auto* shader = OvCore::Global::ServiceLocator::Get<OvCore::ResourceManagement::ShaderManager>()[p_shaderDefinition.shaderPath];
		if (!shader)
		{
			return false;
		}

		p_material.SetShader(shader);
		p_material.SetFeatures(OvRendering::Data::FeatureSet{});
		return true;
	}
}

void OvCore::ResourceManagement::MaterialManager::ProvideStandardShaderDefinition(StandardShaderDefinition p_definition)
{
	m_standardShaderDefinition = std::move(p_definition);
}

const std::optional<OvCore::ResourceManagement::MaterialManager::StandardShaderDefinition>& OvCore::ResourceManagement::MaterialManager::GetStandardShaderDefinition() const
{
	return m_standardShaderDefinition;
}

OvCore::Resources::Material * OvCore::ResourceManagement::MaterialManager::CreateResource(const std::filesystem::path & p_path)
{
	if (const auto embeddedMaterialContext = ResolveEmbeddedMaterialContext(p_path))
	{
		const auto& shaderDefinition = GetStandardShaderDefinition();
		if (!shaderDefinition.has_value())
		{
			return nullptr;
		}

		auto* material = new OvCore::Resources::Material{};
		if (ConfigureEmbeddedMaterial(*material, embeddedMaterialContext.value(), shaderDefinition.value()))
		{
			const_cast<std::string&>(material->path) = p_path.string(); // Force the resource path to fit the given path
			return material;
		}

		delete material;
		return nullptr;
	}

	std::string realPath = GetRealPath(p_path).string();

	Resources::Material* material = OvCore::Resources::Loaders::MaterialLoader::Create(realPath);
	if (material)
	{
		const_cast<std::string&>(material->path) = p_path.string(); // Force the resource path to fit the given path
	}

	return material;
}

void OvCore::ResourceManagement::MaterialManager::DestroyResource(OvCore::Resources::Material * p_resource)
{
	OvCore::Resources::Loaders::MaterialLoader::Destroy(p_resource);
}

void OvCore::ResourceManagement::MaterialManager::ReloadResource(OvCore::Resources::Material* p_resource, const std::filesystem::path& p_path)
{
	const auto embeddedAssetPath = OvRendering::Resources::Parsers::ParseEmbeddedAssetPath(p_path.string());
	const bool isEmbeddedMaterialPath = embeddedAssetPath &&
		OvRendering::Resources::Parsers::ParseEmbeddedMaterialIndex(embeddedAssetPath->assetName).has_value();

	if (const auto embeddedMaterialContext = ResolveEmbeddedMaterialContext(p_path))
	{
		if (const auto& shaderDefinition = GetStandardShaderDefinition(); shaderDefinition.has_value())
		{
			if (ConfigureEmbeddedMaterial(*p_resource, embeddedMaterialContext.value(), shaderDefinition.value()))
			{
				return;
			}
		}

		p_resource->SetShader(nullptr);
		p_resource->SetFeatures(OvRendering::Data::FeatureSet{});
		return;
	}

	if (isEmbeddedMaterialPath)
	{
		if (const auto& shaderDefinition = GetStandardShaderDefinition(); shaderDefinition.has_value())
		{
			ResetMissingEmbeddedMaterial(*p_resource, shaderDefinition.value());
		}
		else
		{
			p_resource->SetShader(nullptr);
			p_resource->SetFeatures(OvRendering::Data::FeatureSet{});
		}

		return;
	}

	std::string realPath = GetRealPath(p_path).string();
	OvCore::Resources::Loaders::MaterialLoader::Reload(*p_resource, realPath);
}
