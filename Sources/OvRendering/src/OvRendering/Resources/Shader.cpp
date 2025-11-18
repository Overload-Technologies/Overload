/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include <format>
#include <ranges>

#include <OvDebug/Assertion.h>
#include <OvRendering/Resources/Shader.h>

namespace
{
	void ValidateVariants(const OvRendering::Resources::Shader::Variants& p_variants)
	{
		OVASSERT(p_variants.contains({}), "Missing default pass.");
		OVASSERT(p_variants.at({}).size() > 0 && p_variants.at({}).contains({}), "Missing default program.");
	}
}

OvRendering::HAL::ShaderProgram& OvRendering::Resources::Shader::GetVariant(std::optional<const std::string_view> p_pass, const Data::FeatureSet& p_featureSet)
{
	const std::string pass = std::string{ p_pass.value_or("") };

	if (!m_variants.contains(pass))
	{
		OVASSERT(m_variants[{}].contains({}), "No default program found for the default pass");
		return *m_variants[{}][{}];
	}

	if (!m_variants[pass].contains(p_featureSet))
	{
		OVASSERT(m_variants[pass].contains({}), std::format("No default program found for pass: {}", pass));
		return *m_variants[pass][{}];
	}

	return *m_variants[pass][p_featureSet];
}

const OvRendering::Data::FeatureSet& OvRendering::Resources::Shader::GetFeatures() const
{
	return m_features;
}

const std::unordered_set<std::string>& OvRendering::Resources::Shader::GetPasses() const
{
	return m_passes;
}

OvRendering::Resources::Shader::Shader(
	const std::string p_path,
	Variants&& p_variants
) : path(p_path)
{
	SetVariants(std::move(p_variants));
}

void OvRendering::Resources::Shader::SetVariants(Variants&& p_variants)
{
	ValidateVariants(p_variants);
	m_variants = std::move(p_variants);

	m_passes.clear();
	m_features.clear();

	// Find all passes & features based on the compiled variants
	for (const auto& [pass, featureVariants] : m_variants)
	{
		m_passes.insert(pass);

		for (const auto& featureSet : featureVariants | std::views::keys)
		{
			m_features.insert(featureSet.begin(), featureSet.end());
		}
	}
}

const OvRendering::Resources::Shader::Variants& OvRendering::Resources::Shader::GetVariants() const
{
	return m_variants;
}
