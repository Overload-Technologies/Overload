/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include <OvMaths/FVector3.h>

#include <OvPhysics/Settings/CollisionLayers.h>

namespace OvPhysics::Settings
{
	/**
	* Data structure to give to the PhysicsEngine constructor to setup its settings
	*/
	struct PhysicsSettings
	{
		OvMaths::FVector3 gravity = { 0.0f, -9.81f, 0.f };
		CollisionLayers collisionLayers;
	};
}