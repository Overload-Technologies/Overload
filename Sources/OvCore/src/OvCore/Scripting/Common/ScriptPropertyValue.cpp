/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/



#include "OvCore/Scripting/Common/ScriptPropertyValue.h"

#include <format>
#include <stdexcept>

#include "OvCore/ECS/Actor.h"
#include "OvCore/Global/ServiceLocator.h"
#include "OvCore/SceneSystem/SceneManager.h"

namespace OvCore::Scripting
{
	bool ActorRef::IsAlive() const
	{
		if (guid == 0)
			return false;

		auto* scene = OVSERVICE(OvCore::SceneSystem::SceneManager).GetCurrentScene();
		if (!scene)
			return false;

		auto* actor = scene->FindActorByGUID(guid);
		return actor && actor->IsAlive();
	}

	OvCore::ECS::Actor& ActorRef::Resolve() const
	{
    if (guid == 0)
        throw std::runtime_error("attempt to use a null actor reference");

    auto* scene = OVSERVICE(OvCore::SceneSystem::SceneManager).GetCurrentScene();
    if (!scene)
        throw std::runtime_error("ActorRef: no active scene");

    auto* actor = scene->FindActorByGUID(guid);
    if (!actor || !actor->IsAlive())
        throw std::runtime_error(std::format(
            "attempt to use a destroyed actor (GUID: {:016X})", guid));

    return *actor;
	}
}
