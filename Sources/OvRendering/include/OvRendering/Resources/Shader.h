/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include <string>
#include <memory>
#include <unordered_set>
#include <unordered_map>

#include <OvRendering/Data/FeatureSet.h>
#include <OvRendering/HAL/ShaderProgram.h>

namespace OvRendering::Resources
{
	namespace Loaders { class ShaderLoader; }

	/**
	* Represents a shader resource, which wraps a shader program and adds a path to it.
	* Can be seen as a "Shader Asset".
	*/
	class Shader
	{
		friend class Loaders::ShaderLoader;

	public:
		// Shader programs for each feature combination
		using FeatureVariants = std::unordered_map<
			Data::FeatureSet,
			std::unique_ptr<HAL::ShaderProgram>,
			Data::FeatureSetHash,
			Data::FeatureSetEqual
		>;

		// Shader programs for each pass for each feature combination
		using Variants = std::unordered_map<
			std::string,
			FeatureVariants
		>;

		/**
		* Returns the associated shader program for a given feature set
		* @param p_pass (optional) The pass to use. If not provided, the default pass will be selected.
		* @param p_featureSet (optional) The feature set to use. If not provided, the default program will be used.
		*/
		HAL::ShaderProgram& GetVariant(
			std::optional<const std::string_view> p_pass = std::nullopt,
			const Data::FeatureSet& p_featureSet = {}
		);

		/**
		* Returns user-configurable features (declared with #feature).
		* Engine-controlled features (declared with #engine_feature) are excluded.
		*/
		const Data::FeatureSet& GetFeatures() const;

		/**
		* Returns engine-controlled features (declared with #engine_feature).
		*/
		const Data::FeatureSet& GetEngineFeatures() const;

		/**
		* Returns supported passes
		*/
		const std::unordered_set<std::string>& GetPasses() const;

		/**
		* Return all programs
		*/
		const Variants& GetVariants() const;

	private:
		Shader(
			const std::string p_path,
			Variants&& p_variants,
			Data::FeatureSet p_engineFeatures = {}
		);

		~Shader() = default;
		void SetVariants(Variants&& p_variants, Data::FeatureSet p_engineFeatures = {});

	public:
		const std::string path;

	private:
		std::unordered_set<std::string> m_passes;
		Data::FeatureSet m_features;
		Data::FeatureSet m_engineFeatures;
		Variants m_variants;
	};
}
