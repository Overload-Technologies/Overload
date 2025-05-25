/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include <OvRendering/Data/Describable.h>
#include <OvRendering/Data/Material.h>
#include <OvRendering/Resources/IMesh.h>
#include <OvTools/Utils/OptRef.h>

namespace OvRendering::Entities
{
	enum class EDrawableType
	{
		UNDEFINED,
		OPAQUE,
		TRANSPARENT,
		UI
	};

	enum class ECullingPolicy
	{
		NEVER,
		ALWAYS
	};

	/**
	* Drawable entity
	*/
	struct Drawable : public Data::Describable
	{
		OvTools::Utils::OptRef<OvRendering::Resources::IMesh> mesh;
		OvTools::Utils::OptRef<OvRendering::Data::Material> material;
		OvMaths::FTransform transform = OvMaths::FVector3::Zero;
		ECullingPolicy cullingPolicy = ECullingPolicy::ALWAYS;
		EDrawableType type = EDrawableType::UNDEFINED;
		Data::StateMask stateMask;
		Settings::EPrimitiveMode primitiveMode = OvRendering::Settings::EPrimitiveMode::TRIANGLES;
		std::optional<std::string> pass = std::nullopt;
		std::optional<Data::FeatureSet> featureSetOverride = std::nullopt;
	};
}