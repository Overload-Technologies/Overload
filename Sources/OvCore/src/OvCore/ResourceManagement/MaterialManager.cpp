/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include "OvCore/ResourceManagement/MaterialManager.h"

#include <optional>
#include <string_view>

#include <OvCore/Global/ServiceLocator.h>
#include <OvCore/ResourceManagement/ModelManager.h>
#include <OvCore/ResourceManagement/ShaderManager.h>
#include <OvCore/ResourceManagement/TextureManager.h>
#include <OvRendering/Resources/Parsers/EmbeddedAssetPath.h>

namespace
{
	constexpr std::string_view kStandardShaderPath = ":Shaders\\Standard.ovfx";

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

		const auto* embeddedMaterial = model->GetEmbeddedMaterial(materialIndex.value());
		if (!embeddedMaterial)
		{
			return std::nullopt;
		}

		return EmbeddedMaterialContext{
			.modelPath = embeddedAssetPath->modelPath,
			.model = model,
			.materialData = embeddedMaterial
		};
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

		const auto* embeddedTexture = p_context.model->GetEmbeddedTexture(p_textureIndex.value());
		if (!embeddedTexture)
		{
			return false;
		}

		const std::string extension = embeddedTexture->extension.empty() ? "bin" : embeddedTexture->extension;
		const std::string texturePath = OvRendering::Resources::Parsers::MakeEmbeddedTexturePath(
			p_context.modelPath,
			p_textureIndex.value(),
			extension
		);

		if (auto* texture = OvCore::Global::ServiceLocator::Get<OvCore::ResourceManagement::TextureManager>().GetResource(texturePath))
		{
			p_material.TrySetProperty(std::string{ p_uniformName }, texture);
			return true;
		}

		return false;
	}

	bool ConfigureEmbeddedMaterial(
		OvCore::Resources::Material& p_material,
		const EmbeddedMaterialContext& p_context
	)
	{
		auto* shader = OvCore::Global::ServiceLocator::Get<OvCore::ResourceManagement::ShaderManager>()[std::string{ kStandardShaderPath }];
		if (!shader)
		{
			return false;
		}

		const auto& materialData = *p_context.materialData;

		p_material.SetShader(shader);
		p_material.SetFeatures(OvRendering::Data::FeatureSet{});

		p_material.TrySetProperty("u_Albedo", materialData.albedo);
		p_material.TrySetProperty("u_Metallic", materialData.metallic);
		p_material.TrySetProperty("u_Roughness", materialData.roughness);
		p_material.TrySetProperty("u_EmissiveColor", materialData.emissiveColor);
		p_material.TrySetProperty("u_EmissiveIntensity", materialData.emissiveIntensity);

		const bool normalTextureBound = BindEmbeddedTextureProperty(p_material, p_context, materialData.normalTexture, "u_NormalMap");
		const bool heightTextureBound = BindEmbeddedTextureProperty(p_material, p_context, materialData.heightTexture, "u_HeightMap");

		BindEmbeddedTextureProperty(p_material, p_context, materialData.albedoTexture, "u_AlbedoMap");
		BindEmbeddedTextureProperty(p_material, p_context, materialData.metallicTexture, "u_MetallicMap");
		BindEmbeddedTextureProperty(p_material, p_context, materialData.roughnessTexture, "u_RoughnessMap");
		BindEmbeddedTextureProperty(p_material, p_context, materialData.ambientOcclusionTexture, "u_AmbientOcclusionMap");
		BindEmbeddedTextureProperty(p_material, p_context, materialData.emissiveTexture, "u_EmissiveMap");
		BindEmbeddedTextureProperty(p_material, p_context, materialData.opacityTexture, "u_MaskMap");

		if (materialData.normalMapping || normalTextureBound)
		{
			p_material.AddFeature("NORMAL_MAPPING");
		}

		if (materialData.parallaxMapping || heightTextureBound)
		{
			p_material.AddFeature("PARALLAX_MAPPING");
		}

		return true;
	}
}

OvCore::Resources::Material * OvCore::ResourceManagement::MaterialManager::CreateResource(const std::filesystem::path & p_path)
{
	if (const auto embeddedMaterialContext = ResolveEmbeddedMaterialContext(p_path))
	{
		auto* material = new OvCore::Resources::Material{};
		if (ConfigureEmbeddedMaterial(*material, embeddedMaterialContext.value()))
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
	if (const auto embeddedMaterialContext = ResolveEmbeddedMaterialContext(p_path))
	{
		ConfigureEmbeddedMaterial(*p_resource, embeddedMaterialContext.value());
		return;
	}

	std::string realPath = GetRealPath(p_path).string();
	OvCore::Resources::Loaders::MaterialLoader::Reload(*p_resource, realPath);
}
