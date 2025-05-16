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
		using ProgramVariants = std::unordered_map<
			Data::FeatureSet,
			std::unique_ptr<HAL::ShaderProgram>,
			Data::FeatureSetHash,
			Data::FeatureSetEqual
		>;

		/**
		* Returns the associated shader program for a given feature set
		* @param p_featureSet (optional) The feature set to use. If not provided, the default program will be used.
		*/
		HAL::ShaderProgram& GetProgram(const Data::FeatureSet& p_featureSet = {});

		/**
		* Returns supported features
		*/
		const Data::FeatureSet& GetFeatures() const;

		/**
		* Return all programs
		*/
		const ProgramVariants& GetPrograms() const;

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
		Data::FeatureSet m_features;
		ProgramVariants m_programs;
	};
}