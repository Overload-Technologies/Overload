/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include <OvMaths/FVector3.h>

namespace OvPhysics::Settings
{
	/**
	* Specifies the settings of a physical body
	*/
	struct BodySettings
	{
		OvMaths::FVector3 linearVelocity = OvMaths::FVector3::Zero;
		OvMaths::FVector3 angularVelocity = OvMaths::FVector3::Zero;
		OvMaths::FVector3 linearFactor = OvMaths::FVector3::One;
		OvMaths::FVector3 angularFactor = OvMaths::FVector3::One;
		float restitution = 0.f;
		float friction = 0.5f;
		bool isTrigger = false;
		bool isKinematic = false;
	};
}