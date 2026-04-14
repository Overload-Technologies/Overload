/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include <cstdint>
#include <string>

#include <OvCore/ECS/Components/CSkinnedMeshRenderer.h>
#include <OvCore/Rendering/SkinningDrawableDescriptor.h>
#include <OvCore/Rendering/SkinningUtils.h>
#include <OvRendering/Entities/Drawable.h>

bool OvCore::Rendering::SkinningUtils::IsSkinningActive(const OvCore::ECS::Components::CSkinnedMeshRenderer* p_renderer)
{
	return p_renderer && p_renderer->HasSkinningData();
}

OvRendering::Data::FeatureSet OvCore::Rendering::SkinningUtils::BuildFeatureSet(const OvRendering::Data::FeatureSet* p_baseFeatures)
{
	OvRendering::Data::FeatureSet features = p_baseFeatures ? *p_baseFeatures : OvRendering::Data::FeatureSet{};
	features.insert(std::string{ kFeatureName });
	return features;
}

void OvCore::Rendering::SkinningUtils::ApplyDescriptor(
	OvRendering::Entities::Drawable& p_drawable,
	const OvCore::ECS::Components::CSkinnedMeshRenderer& p_renderer
)
{
	const auto& boneMatrices = p_renderer.GetBoneMatricesTransposed();

	p_drawable.SetDescriptor<OvCore::Rendering::SkinningDrawableDescriptor>({
		.matrices = boneMatrices.data(),
		.count = static_cast<uint32_t>(boneMatrices.size()),
		.poseVersion = p_renderer.GetPoseVersion(),
		.boundsScale = p_renderer.GetMeshBoundsScale()
	});
}

void OvCore::Rendering::SkinningUtils::ApplyToDrawable(
	OvRendering::Entities::Drawable& p_drawable,
	const OvCore::ECS::Components::CSkinnedMeshRenderer& p_renderer,
	const OvRendering::Data::FeatureSet* p_baseFeatures
)
{
	p_drawable.featureSetOverride = BuildFeatureSet(p_baseFeatures);
	ApplyDescriptor(p_drawable, p_renderer);
}
