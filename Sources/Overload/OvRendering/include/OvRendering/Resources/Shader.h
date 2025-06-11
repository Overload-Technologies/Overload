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
#include <OvTools/Eventing/Event.h>

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
		* Returns supported features
		*/
		const Data::FeatureSet& GetFeatures() const;

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
			Variants&& p_variants
		);

		~Shader() = default;
		void SetVariants(Variants&& p_variants);

	public:
		const std::string path;
		OvTools::Eventing::Event<> VariantsChangedEvent;

	private:
		std::unordered_set<std::string> m_passes;
		Data::FeatureSet m_features;
		Variants m_variants;
	};
}
