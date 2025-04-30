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
		using FeatureSet = std::unordered_set<std::string>;

		struct FeatureSetHash
		{
			size_t operator()(const FeatureSet& fs) const;
		};

		struct FeatureSetEqual
		{
			bool operator()(const FeatureSet& lhs, const FeatureSet& rhs) const;
		};

		using ProgramVariants = std::unordered_map<
			FeatureSet,
			std::unique_ptr<HAL::ShaderProgram>,
			FeatureSetHash,
			FeatureSetEqual
		>;

		/**
		* Returns the associated shader program for a given feature set
		* @param p_featureSet (optional) The feature set to use. If not provided, the default program will be used.
		*/
		HAL::ShaderProgram& GetProgram(const FeatureSet& p_featureSet = {});

		/**
		* Returns supported features
		*/
		const FeatureSet& GetFeatures() const;

	private:
		Shader(
			const std::string p_path,
			ProgramVariants&& p_programs
		);

		~Shader() = default;
		void SetPrograms(ProgramVariants&& p_programs);

	public:
		const std::string path;

	private:
		FeatureSet m_features;
		ProgramVariants m_programs;
	};
}