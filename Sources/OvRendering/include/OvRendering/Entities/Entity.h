/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include <memory>

#include <OvMaths/FTransform.h>
#include <OvTools/Utils/ReferenceOrValue.h>

namespace OvRendering::Entities
{
	/**
	* Represents an entity with a transformation in space
	*/
	struct Entity
	{
		OvTools::Utils::ReferenceOrValue<OvMaths::FTransform> transform;
	};
}
