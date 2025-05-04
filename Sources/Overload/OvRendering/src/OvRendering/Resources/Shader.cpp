/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include <OvDebug/Assertion.h>
#include <OvRendering/Resources/Shader.h>

namespace
{
	void ValidateProgramRegistry(const OvRendering::Resources::Shader::ProgramVariants& p_programs)
	{
		OVASSERT(p_programs.size() > 0 && p_programs.contains({}),
			"Shader program registry must contain at least a default program"
		);
	}
}

size_t OvRendering::Resources::Shader::FeatureSetHash::operator()(const FeatureSet& fs) const
{
	size_t hash = 0;

	for (const auto& feature : fs)
	{
		hash ^= std::hash<std::string>{}(feature)+0x9e3779b9 + (hash << 6) + (hash >> 2);
	}

	return hash;
}

bool OvRendering::Resources::Shader::FeatureSetEqual::operator()(const FeatureSet& lhs, const FeatureSet& rhs) const
{
	return lhs == rhs;
};

OvRendering::HAL::ShaderProgram& OvRendering::Resources::Shader::GetProgram(const FeatureSet& p_featureSet)
{
	if (m_programs.contains(p_featureSet))
	{
		return *m_programs[p_featureSet];
	}
	else
	{
		OVASSERT(m_programs.contains({}), "No default program found for this shader");
		return *m_programs[{}];
	}
}

const OvRendering::Resources::Shader::FeatureSet& OvRendering::Resources::Shader::GetFeatures() const
{
	return m_features;
}

OvRendering::Resources::Shader::Shader(
	const std::string p_path,
	ProgramVariants&& p_program
) : path(p_path)
{
	SetPrograms(std::move(p_program));
}

void OvRendering::Resources::Shader::SetPrograms(ProgramVariants&& p_programs)
{
	ValidateProgramRegistry(p_programs);
	m_programs = std::move(p_programs);

	m_features.clear();

	// Find all features based on the compiled programs
	for (const auto& [key, _] : m_programs)
	{
		m_features.insert(key.begin(), key.end());
	}
}

const OvRendering::Resources::Shader::ProgramVariants& OvRendering::Resources::Shader::GetPrograms() const
{
	return m_programs;
}
