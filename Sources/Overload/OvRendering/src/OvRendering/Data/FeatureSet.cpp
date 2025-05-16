/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include <OvRendering/Data/FeatureSet.h>

namespace OvRendering::Data
{
	size_t FeatureSetHash::operator()(const FeatureSet& fs) const
	{
		size_t hash = 0;

		for (const auto& feature : fs)
		{
			hash ^= std::hash<std::string>{}(feature);
		}

		return hash;
	}

	bool FeatureSetEqual::operator()(const FeatureSet& lhs, const FeatureSet& rhs) const
	{
		return lhs == rhs;
	};
}

OvRendering::Data::FeatureSet operator+(const OvRendering::Data::FeatureSet& p_lhs, const std::string& p_feature)
{
	OvRendering::Data::FeatureSet result = p_lhs;
	result.insert(p_feature);
	return result;
}

OvRendering::Data::FeatureSet operator-(const OvRendering::Data::FeatureSet& p_lhs, const std::string& p_feature)
{
	OvRendering::Data::FeatureSet result = p_lhs;
	result.erase(p_feature);
	return result;
}
