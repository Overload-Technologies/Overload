/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include <string>
#include <unordered_set>

namespace OvRendering::Data
{
	using FeatureSet = std::unordered_set<std::string>;

	struct FeatureSetHash
	{
		size_t operator()(const FeatureSet& fs) const;
	};

	struct FeatureSetEqual
	{
		bool operator()(const FeatureSet& lhs, const FeatureSet& rhs) const;
	};
}

/**
* Convenience operator to add a feature to a set
* @param p_lhs
* @param p_feature
*/
OvRendering::Data::FeatureSet operator+(const OvRendering::Data::FeatureSet& p_lhs, const std::string& p_feature);

/**
* Convenience operator to remove a feature from a set
* @param p_lhs
* @param p_feature
*/
OvRendering::Data::FeatureSet operator-(const OvRendering::Data::FeatureSet& p_lhs, const std::string& p_feature);