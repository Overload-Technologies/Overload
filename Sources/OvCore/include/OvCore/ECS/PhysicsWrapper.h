/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include <optional>
#include <vector>

#include <OvPhysics/Entities/RaycastHit.h>

#include <OvMaths/FVector3.h>

#include "OvCore/ECS/Components/CPhysicalObject.h"

namespace OvCore::ECS
{
	/**
	* Simple class that contains wrappers for OvPhysics in an ECS style
	*/
	class PhysicsWrapper
	{
	public:
		/**
		* Simple data structure that wraps the physics RaycastHit with physics components
		*/
		struct RaycastHit
		{
			Components::CPhysicalObject* FirstResultObject = nullptr;
			std::vector<Components::CPhysicalObject*> ResultObjects;
		};

		/* Casts a ray against all Physical Object in the Scene and returns information on what was hit
		 * @param p_origin
		 * @param p_end
		 */
		static std::optional<RaycastHit> Raycast(OvMaths::FVector3 p_origin, OvMaths::FVector3 p_direction, float p_distance);
	};
}