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

namespace
{
	/**
	* Filters broadphase pairs with the collision layers, carried by the proxy groups and masks
	*/
	struct CollisionFilterCallback : btOverlapFilterCallback
	{
		bool needBroadphaseCollision(btBroadphaseProxy* p_first, btBroadphaseProxy* p_second) const override
		{
			if ((p_first->m_collisionFilterGroup & p_second->m_collisionFilterMask) == 0 ||
				(p_second->m_collisionFilterGroup & p_first->m_collisionFilterMask) == 0)
			{
				return false;
			}

			/* Assigning the groups ourselves loses the static/static exclusion that the dynamics
			   world applies when it assigns them, so it is restored here */
			const auto first = static_cast<const btCollisionObject*>(p_first->m_clientObject);
			const auto second = static_cast<const btCollisionObject*>(p_second->m_clientObject);

			return !first->isStaticOrKinematicObject() || !second->isStaticOrKinematicObject();
		}
	};

	/**
	* Ray callback ignoring the collision layers, rays not being bound to one
	*/
	template <typename T>
	struct LayerAgnosticRayCallback : T
	{
		using T::T;

		bool needsCollision(btBroadphaseProxy*) const override
		{
			return true;
		}
	};
}

std::map<std::pair<PhysicalObject*, PhysicalObject*>, bool> OvPhysics::Core::PhysicsEngine::m_collisionEvents;

OvPhysics::Core::PhysicsEngine::PhysicsEngine(const Settings::PhysicsSettings & p_settings)
{
	m_collisionConfig = std::make_unique<btDefaultCollisionConfiguration>();
	m_dispatcher = std::make_unique<btCollisionDispatcher>(m_collisionConfig.get());
	m_broadphase = std::make_unique<btDbvtBroadphase>();
	m_solver = std::make_unique<btSequentialImpulseConstraintSolver>();
	m_world = std::make_unique<btDiscreteDynamicsWorld>(m_dispatcher.get(), m_broadphase.get(), m_solver.get(), m_collisionConfig.get());

	m_world->setGravity(Conversion::ToBtVector3(p_settings.gravity));

	m_collisionFilter = std::make_unique<CollisionFilterCallback>();
	m_world->getPairCache()->setOverlapFilterCallback(m_collisionFilter.get());

	m_collisionLayers = p_settings.collisionLayers;

	ListenToPhysicalObjects();
	SetCollisionCallback();
}

OvPhysics::Core::PhysicsEngine::~PhysicsEngine()
{

}

void OvPhysics::Core::PhysicsEngine::PreUpdate()
{
	std::for_each(m_physicalObjects.begin(), m_physicalObjects.end(), std::mem_fn(&PhysicalObject::FlushCollisionFilter));
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
	PreUpdate();

	if (m_world->stepSimulation(p_deltaTime, 10))
	{
		PostUpdate();
		return true;
	}

	return false;
}

std::optional<RaycastHit> OvPhysics::Core::PhysicsEngine::Raycast(OvMaths::FVector3 p_origin, OvMaths::FVector3 p_direction, float p_distance)
{
	if (p_direction == OvMaths::FVector3::Zero)
		return {};

	btVector3 origin = Tools::Conversion::ToBtVector3(p_origin);
	btVector3 target = Tools::Conversion::ToBtVector3(p_origin + p_direction * p_distance);

	RaycastHit resultHit;

	// Try to get First Hit
	LayerAgnosticRayCallback<btCollisionWorld::ClosestRayResultCallback> ClosestRayCallback(origin, target);
	m_world->rayTest(origin, target, ClosestRayCallback);

	if (ClosestRayCallback.hasHit())
	{
		// Get First Hit
		resultHit.FirstResultObject = reinterpret_cast<OvPhysics::Entities::PhysicalObject*>(ClosestRayCallback.m_collisionObject->getUserPointer());

		// Try to get all Hit
		LayerAgnosticRayCallback<btCollisionWorld::AllHitsRayResultCallback> rayCallback(origin, target);
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

const OvPhysics::Settings::CollisionLayers& OvPhysics::Core::PhysicsEngine::GetCollisionLayers() const
{
	return m_collisionLayers;
}

void OvPhysics::Core::PhysicsEngine::SetCollisionLayers(const Settings::CollisionLayers& p_collisionLayers)
{
	m_collisionLayers = p_collisionLayers;

	MarkCollisionFiltersDirty();
}

void OvPhysics::Core::PhysicsEngine::SetLayerCollision(uint32_t p_first, uint32_t p_second, bool p_collide)
{
	m_collisionLayers.SetLayerCollision(p_first, p_second, p_collide);

	/* Only the masks of the two given layers changed, so the other objects keep their filter */
	for (auto& physicalObject : m_physicalObjects)
	{
		const uint32_t layer = physicalObject.get().GetLayer();

		if (layer == p_first || layer == p_second)
		{
			physicalObject.get().MarkCollisionFilterDirty();
		}
	}
}

void OvPhysics::Core::PhysicsEngine::MarkCollisionFiltersDirty()
{
	std::for_each(m_physicalObjects.begin(), m_physicalObjects.end(), std::mem_fn(&PhysicalObject::MarkCollisionFilterDirty));
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
	const auto physicalObject = reinterpret_cast<PhysicalObject*>(p_toConsider.getUserPointer());
	const uint32_t objectLayer = physicalObject ? physicalObject->GetLayer() : Settings::CollisionLayers::kDefaultLayer;
	const uint32_t layer = objectLayer < Settings::CollisionLayers::kMaxLayerCount ? objectLayer : Settings::CollisionLayers::kDefaultLayer;

	m_world->addRigidBody(
		&p_toConsider,
		static_cast<int>(1u << layer),
		static_cast<int>(m_collisionLayers.GetLayerMask(layer))
	);
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
