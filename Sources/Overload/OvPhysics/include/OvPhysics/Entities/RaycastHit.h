/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include <vector>

#include <OvPhysics/Entities/PhysicalObject.h>

namespace OvPhysics::Entities
{
	/**
	* Data structure that holds raycast hit information
	*/
	struct RaycastHit
	{
		Entities::PhysicalObject* FirstResultObject = nullptr;
		std::vector<Entities::PhysicalObject*> ResultObjects;
	};
}