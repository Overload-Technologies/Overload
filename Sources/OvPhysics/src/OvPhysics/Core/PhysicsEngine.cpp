/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include <algorithm>
#include <cstdint>

#include <bullet/btBulletCollisionCommon.h>
#include <bullet/btBulletDynamicsCommon.h>

#include <OvDebug/Logger.h>

#include <OvPhysics/Core/PhysicsEngine.h>
#include <OvPhysics/Entities/PhysicalObject.h>
#include <OvPhysics/Tools/Conversion.h>

using namespace OvPhysics::Tools;
using namespace OvPhysics::Entities;

std::map<std::pair<PhysicalObject*, PhysicalObject*>, bool> OvPhysics::Core::PhysicsEngine::m_collisionEvents;
bool OvPhysics::Core::PhysicsEngine::s_dispatching = false;
std::vector<std::shared_ptr<void>> OvPhysics::Core::PhysicsEngine::s_graveyard;

OvPhysics::Core::PhysicsEngine::PhysicsEngine(const Settings::PhysicsSettings & p_settings)
{
	m_collisionConfig = std::make_unique<btDefaultCollisionConfiguration>();
	m_dispatcher = std::make_unique<btCollisionDispatcher>(m_collisionConfig.get());
	m_broadphase = std::make_unique<btDbvtBroadphase>();
	m_solver = std::make_unique<btSequentialImpulseConstraintSolver>();
	m_world = std::make_unique<btDiscreteDynamicsWorld>(m_dispatcher.get(), m_broadphase.get(), m_solver.get(), m_collisionConfig.get());

	m_world->setGravity(Conversion::ToBtVector3(p_settings.gravity));

	ListenToPhysicalObjects();
	SetCollisionCallback();
}

OvPhysics::Core::PhysicsEngine::~PhysicsEngine()
{
    //reelease physical object(s) whose destruction was deferred during the last update, while the engine and the physics event(s) are still alive.
    FlushGraveyard();

}

void OvPhysics::Core::PhysicsEngine::PreUpdate()
{
	std::for_each(m_physicalObjects.begin(), m_physicalObjects.end(), std::mem_fn(&PhysicalObject::UpdateBtTransform));

	ResetCollisionEvents();
}

void OvPhysics::Core::PhysicsEngine::PostUpdate()
{
	std::for_each(m_physicalObjects.begin(), m_physicalObjects.end(), std::mem_fn(&PhysicalObject::UpdateFTransform));

	CheckCollisionStopEvents();
}

bool OvPhysics::Core::PhysicsEngine::Update(float p_deltaTime)
{
	FlushGraveyard(); //release objects removed during the previous update...

	bool simulated = false;
	{
		struct DispatchScope
		{
			DispatchScope()  { s_dispatching = true; }
			~DispatchScope() { s_dispatching = false; }
		} scope;

		PreUpdate();

		if (m_world->stepSimulation(p_deltaTime, 10))
		{
			PostUpdate();
			simulated = true;
		}
	}

	return simulated; // no flush here
}

bool OvPhysics::Core::PhysicsEngine::IsDispatching()
{
	return s_dispatching;
}

void OvPhysics::Core::PhysicsEngine::DeferDestruction(std::shared_ptr<void> p_object)
{
	s_graveyard.push_back(std::move(p_object));
}

void OvPhysics::Core::PhysicsEngine::FlushGraveyard()
{
	auto dead = std::move(s_graveyard);
	s_graveyard.clear();
	dead.clear();
}

std::optional<RaycastHit> OvPhysics::Core::PhysicsEngine::Raycast(OvMaths::FVector3 p_origin, OvMaths::FVector3 p_direction, float p_distance)
{
	if (p_direction == OvMaths::FVector3::Zero)
		return {};

	btVector3 origin = Tools::Conversion::ToBtVector3(p_origin);
	btVector3 target = Tools::Conversion::ToBtVector3(p_origin + p_direction * p_distance);

	RaycastHit resultHit;

	// Try to get First Hit
	btCollisionWorld::ClosestRayResultCallback ClosestRayCallback(origin, target);
	m_world->rayTest(origin, target, ClosestRayCallback);

	if (ClosestRayCallback.hasHit())
	{
		// Get First Hit
		resultHit.FirstResultObject = reinterpret_cast<OvPhysics::Entities::PhysicalObject*>(ClosestRayCallback.m_collisionObject->getUserPointer());

		// Try to get all Hit
		btCollisionWorld::AllHitsRayResultCallback rayCallback(origin, target);
		m_world->rayTest(origin, target, rayCallback);

		// Get all Hit
		for (int i = 0; i < rayCallback.m_collisionObjects.size(); i++)
			resultHit.ResultObjects.push_back(reinterpret_cast<OvPhysics::Entities::PhysicalObject*>(rayCallback.m_collisionObjects[i]->getUserPointer()));

		return resultHit;
	}
	else
		return {};
}

void OvPhysics::Core::PhysicsEngine::SetGravity(const OvMaths::FVector3 & p_gravity)
{
	m_world->setGravity(Conversion::ToBtVector3(p_gravity));
}

OvMaths::FVector3 OvPhysics::Core::PhysicsEngine::GetGravity() const
{
	return Conversion::ToOvVector3(m_world->getGravity());
}

void OvPhysics::Core::PhysicsEngine::ListenToPhysicalObjects()
{
	PhysicalObject::CreatedEvent += std::bind(static_cast<void(PhysicsEngine::*)(PhysicalObject&)>(&PhysicsEngine::Consider), this, std::placeholders::_1);
	PhysicalObject::DestroyedEvent += std::bind(static_cast<void(PhysicsEngine::*)(PhysicalObject&)>(&PhysicsEngine::Unconsider), this, std::placeholders::_1);

	PhysicalObject::ConsiderEvent += std::bind(static_cast<void(PhysicsEngine::*)(btRigidBody&)>(&PhysicsEngine::Consider), this, std::placeholders::_1);
	PhysicalObject::UnconsiderEvent += std::bind(static_cast<void(PhysicsEngine::*)(btRigidBody&)>(&PhysicsEngine::Unconsider), this, std::placeholders::_1);
}

void OvPhysics::Core::PhysicsEngine::Consider(PhysicalObject& p_toConsider)
{
	m_physicalObjects.push_back(std::ref(p_toConsider));
}

void OvPhysics::Core::PhysicsEngine::Unconsider(PhysicalObject& p_toUnconsider)
{
	{
		auto found = std::find_if(m_physicalObjects.begin(), m_physicalObjects.end(), [&p_toUnconsider](std::reference_wrapper<PhysicalObject> element)
		{
			return std::addressof(p_toUnconsider) == std::addressof(element.get());
		});

		if (found != m_physicalObjects.end())
			m_physicalObjects.erase(found);
	}

	{
		decltype(m_collisionEvents)::iterator iter = m_collisionEvents.begin();
		decltype(m_collisionEvents)::iterator endIter = m_collisionEvents.end();

		for (; iter != endIter; )
		{
			if (iter->first.first == std::addressof(p_toUnconsider) || iter->first.second == std::addressof(p_toUnconsider))
			{
				m_collisionEvents.erase(iter++);
			}
			else {
				++iter;
			}
		}
	}
}

void OvPhysics::Core::PhysicsEngine::Consider(btRigidBody& p_toConsider)
{
	m_world->addRigidBody(&p_toConsider);
}

void OvPhysics::Core::PhysicsEngine::Unconsider(btRigidBody& p_toUnconsider)
{
	m_world->removeRigidBody(&p_toUnconsider);
}

void OvPhysics::Core::PhysicsEngine::ResetCollisionEvents()
{
	for (auto& element : m_collisionEvents)
		element.second = false;
}

void OvPhysics::Core::PhysicsEngine::CheckCollisionStopEvents()
{
	for (auto it = m_collisionEvents.begin(); it != m_collisionEvents.end();)
	{
		auto objects = it->first;
		if (!it->second)
		{
			if (!objects.first->IsTrigger() && !objects.second->IsTrigger())
			{
				objects.first->CollisionStopEvent.Invoke(*objects.second);
				objects.second->CollisionStopEvent.Invoke(*objects.first);
			}
			else
			{
				if (objects.first->IsTrigger())
					objects.first->TriggerStopEvent.Invoke(*objects.second);
				else
					objects.second->TriggerStopEvent.Invoke(*objects.first);
			}

			it = m_collisionEvents.erase(it);
		}
		else
			++it;
	}
}

bool OvPhysics::Core::PhysicsEngine::CollisionCallback(btManifoldPoint& cp, const btCollisionObjectWrapper* obj1, int id1, int index1, const btCollisionObjectWrapper* obj2, int id2, int index2)
{
	auto object1 = reinterpret_cast<PhysicalObject*>(obj1->getCollisionObject()->getUserPointer());
	auto object2 = reinterpret_cast<PhysicalObject*>(obj2->getCollisionObject()->getUserPointer());

	if (object1 && object2)
	{
		/* If the objects are not all trigger, enter */
		if (!object1->IsTrigger() || !object2->IsTrigger())
		{
			if (m_collisionEvents.find({ object1 , object2 }) == m_collisionEvents.end())
			{
				/* If object is trigger, invoke Trigger event,
				 * else : is the other object trigger ? yes -> do nothing, no -> invoke Collision event
				 */

				 // Object 1 (Start event)
				if (object1->IsTrigger())
					object1->TriggerStartEvent.Invoke(*object2);
				else
				{
					if (!object2->IsTrigger())
						object1->CollisionStartEvent.Invoke(*object2);
				}
				// Object 2 (Start event)
				if (object2->IsTrigger())
					object2->TriggerStartEvent.Invoke(*object1);
				else
				{
					if (!object1->IsTrigger())
						object2->CollisionStartEvent.Invoke(*object1);
				}

				// Object 1 (Stay event)
				if (object1->IsTrigger())
					object1->TriggerStayEvent.Invoke(*object2);
				else
				{
					if (!object2->IsTrigger())
						object1->CollisionStayEvent.Invoke(*object2);
				}
				// Object 2 (Stay event)
				if (object2->IsTrigger())
					object2->TriggerStayEvent.Invoke(*object1);
				else
				{
					if (!object1->IsTrigger())
						object2->CollisionStayEvent.Invoke(*object1);
				}

				m_collisionEvents[{ object1, object2 }] = true;
			}
			else
			{
				if (!m_collisionEvents[{ object1, object2 }])
				{
					// Object 1 (Stay event)
					if (object1->IsTrigger())
						object1->TriggerStayEvent.Invoke(*object2);
					else
					{
						if (!object2->IsTrigger())
							object1->CollisionStayEvent.Invoke(*object2);
					}
					// Object 2 (Stay event)
					if (object2->IsTrigger())
						object2->TriggerStayEvent.Invoke(*object1);
					else
					{
						if (!object1->IsTrigger())
							object2->CollisionStayEvent.Invoke(*object1);
					}

					m_collisionEvents[{ object1, object2 }] = true;
				}
			}
		}
	}

	return false;
}

void OvPhysics::Core::PhysicsEngine::SetCollisionCallback()
{
	gContactAddedCallback = &PhysicsEngine::CollisionCallback;
}
