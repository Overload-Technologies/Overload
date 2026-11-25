/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include <cstdint>
#include <optional>
#include <string>
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

		/**
		* Returns the index of the collision layer having the given name, or nothing if no layer matches
		* @param p_name
		*/
		static std::optional<uint32_t> GetLayerIndex(const std::string& p_name);

		/**
		* Returns the name of the given collision layer
		* @param p_layer
		*/
		static const std::string& GetLayerName(uint32_t p_layer);

		/**
		* Defines if the two given collision layers should collide together
		* @param p_first
		* @param p_second
		* @param p_collide
		*/
		static void SetLayerCollision(uint32_t p_first, uint32_t p_second, bool p_collide);

		/**
		* Returns true if the two given collision layers collide together
		* @param p_first
		* @param p_second
		*/
		static bool GetLayerCollision(uint32_t p_first, uint32_t p_second);
	};
}