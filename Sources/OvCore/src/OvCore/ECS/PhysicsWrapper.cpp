/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include "OvCore/ECS/PhysicsWrapper.h"

#include "OvCore/Global/ServiceLocator.h"

#include <OvPhysics/Core/PhysicsEngine.h>

std::optional<OvCore::ECS::PhysicsWrapper::RaycastHit> OvCore::ECS::PhysicsWrapper::Raycast(OvMaths::FVector3 p_origin, OvMaths::FVector3 p_direction, float p_distance)
{
	if (auto result = OVSERVICE(OvPhysics::Core::PhysicsEngine).Raycast(p_origin, p_direction, p_distance))
	{
		RaycastHit finalResult;

		finalResult.FirstResultObject = std::addressof(result.value().FirstResultObject->GetUserData<std::reference_wrapper<Components::CPhysicalObject>>().get());
		for (auto object : result.value().ResultObjects)
			finalResult.ResultObjects.push_back(std::addressof(object->GetUserData<std::reference_wrapper<Components::CPhysicalObject>>().get()));

		return finalResult;
	}
	else
		return {};
}

std::optional<uint32_t> OvCore::ECS::PhysicsWrapper::GetLayerIndex(const std::string& p_name)
{
	return OVSERVICE(OvPhysics::Core::PhysicsEngine).GetCollisionLayers().FindLayer(p_name);
}

const std::string& OvCore::ECS::PhysicsWrapper::GetLayerName(uint32_t p_layer)
{
	return OVSERVICE(OvPhysics::Core::PhysicsEngine).GetCollisionLayers().GetLayerName(p_layer);
}

void OvCore::ECS::PhysicsWrapper::SetLayerCollision(uint32_t p_first, uint32_t p_second, bool p_collide)
{
	OVSERVICE(OvPhysics::Core::PhysicsEngine).SetLayerCollision(p_first, p_second, p_collide);
}

bool OvCore::ECS::PhysicsWrapper::GetLayerCollision(uint32_t p_first, uint32_t p_second)
{
	return OVSERVICE(OvPhysics::Core::PhysicsEngine).GetCollisionLayers().GetLayerCollision(p_first, p_second);
}
