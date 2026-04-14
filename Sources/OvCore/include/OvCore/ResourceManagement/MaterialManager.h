/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include <optional>
#include <string>

#include "OvCore/Resources/Loaders/MaterialLoader.h"
#include "OvCore/ResourceManagement/AResourceManager.h"

namespace OvCore::ResourceManagement
{
	/**
	* ResourceManager of materials
	*/
	class MaterialManager : public AResourceManager<OvCore::Resources::Material>
	{
	public:
		struct StandardShaderDefinition
		{
			std::string shaderPath;
			std::string albedoUniform = "u_Albedo";
			std::string metallicUniform = "u_Metallic";
			std::string roughnessUniform = "u_Roughness";
			std::string emissiveColorUniform = "u_EmissiveColor";
			std::string emissiveIntensityUniform = "u_EmissiveIntensity";
			std::string normalMapUniform = "u_NormalMap";
			std::string heightMapUniform = "u_HeightMap";
			std::string albedoMapUniform = "u_AlbedoMap";
			std::string metallicMapUniform = "u_MetallicMap";
			std::string roughnessMapUniform = "u_RoughnessMap";
			std::string ambientOcclusionMapUniform = "u_AmbientOcclusionMap";
			std::string emissiveMapUniform = "u_EmissiveMap";
			std::string opacityMapUniform = "u_MaskMap";
			std::string normalMappingFeature = "NORMAL_MAPPING";
			std::string parallaxMappingFeature = "PARALLAX_MAPPING";
		};

		/**
		* Provide the shader definition used to create embedded materials
		* @param p_definition
		*/
		void ProvideStandardShaderDefinition(StandardShaderDefinition p_definition);

		/**
		* Returns the current shader definition used for embedded materials
		*/
		const std::optional<StandardShaderDefinition>& GetStandardShaderDefinition() const;

		/**
		* Create the resource identified by the given path
		* @param p_path
		*/
		virtual OvCore::Resources::Material* CreateResource(const std::filesystem::path & p_path) override;

		/**
		* Destroy the given resource
		* @param p_resource
		*/
		virtual void DestroyResource(OvCore::Resources::Material* p_resource) override;

		/**
		* Reload the given resource
		* @param p_resource
		* @param p_path
		*/
		virtual void ReloadResource(OvCore::Resources::Material* p_resource, const std::filesystem::path& p_path) override;

	private:
		std::optional<StandardShaderDefinition> m_standardShaderDefinition;
	};
}
