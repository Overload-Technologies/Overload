/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include <string_view>

#include <OvRendering/Data/FeatureSet.h>

namespace OvCore::ECS::Components { class CSkinnedMeshRenderer; }
namespace OvRendering::Entities { struct Drawable; }

namespace OvCore::Rendering::SkinningUtils
{
	inline constexpr std::string_view kFeatureName = "SKINNING";

	/**
	* Returns true when a skinned renderer can provide valid skinning data.
	* @param p_renderer
	*/
	bool IsSkinningActive(const OvCore::ECS::Components::CSkinnedMeshRenderer* p_renderer);

	/**
	* Builds a feature set containing the skinning feature.
	* @param p_baseFeatures
	*/
	OvRendering::Data::FeatureSet BuildFeatureSet(const OvRendering::Data::FeatureSet* p_baseFeatures = nullptr);

	/**
	* Adds the skinning descriptor to the target drawable.
	* @param p_drawable
	* @param p_renderer
	*/
	void ApplyDescriptor(
		OvRendering::Entities::Drawable& p_drawable,
		const OvCore::ECS::Components::CSkinnedMeshRenderer& p_renderer
	);

	/**
	* Applies both feature override and skinning descriptor to a drawable.
	* @param p_drawable
	* @param p_renderer
	* @param p_baseFeatures
	*/
	void ApplyToDrawable(
		OvRendering::Entities::Drawable& p_drawable,
		const OvCore::ECS::Components::CSkinnedMeshRenderer& p_renderer,
		const OvRendering::Data::FeatureSet* p_baseFeatures = nullptr
	);
}
