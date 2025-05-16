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
	/**
	* Drawable entity
	*/
	struct Drawable : public Data::Describable
	{
		OvTools::Utils::OptRef<OvRendering::Resources::IMesh> mesh;
		OvTools::Utils::OptRef<OvRendering::Data::Material> material;
		Data::StateMask stateMask;
		Settings::EPrimitiveMode primitiveMode = OvRendering::Settings::EPrimitiveMode::TRIANGLES;
		std::optional<Data::FeatureSet> featureSetOverride = std::nullopt;
	};
}