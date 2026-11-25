/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include <algorithm>
#include <cstdint>
#include <mutex>
#include <utility>

#include <Jolt/Jolt.h>

#include <Jolt/Core/Factory.h>
#include <Jolt/Core/JobSystemThreadPool.h>
#include <Jolt/Core/TempAllocator.h>
#include <Jolt/Physics/Body/Body.h>
#include <Jolt/Physics/Body/BodyInterface.h>
#include <Jolt/Physics/Collision/BroadPhase/BroadPhaseLayerInterfaceTable.h>
#include <Jolt/Physics/Collision/BroadPhase/ObjectVsBroadPhaseLayerFilterTable.h>
#include <Jolt/Physics/Collision/CastResult.h>
#include <Jolt/Physics/Collision/CollisionCollectorImpl.h>
#include <Jolt/Physics/Collision/ContactListener.h>
#include <Jolt/Physics/Collision/NarrowPhaseQuery.h>
#include <Jolt/Physics/Collision/ObjectLayerPairFilterTable.h>
#include <Jolt/Physics/Collision/RayCast.h>
#include <Jolt/Physics/PhysicsSettings.h>
#include <Jolt/Physics/PhysicsSystem.h>
#include <Jolt/RegisterTypes.h>

#include <OvDebug/Logger.h>

#include <OvPhysics/Core/PhysicsEngine.h>
#include <OvPhysics/Entities/PhysicalObject.h>
#include <OvPhysics/Tools/Conversion.h>

using namespace OvPhysics::Tools;
using namespace OvPhysics::Entities;

std::map<std::pair<PhysicalObject*, PhysicalObject*>, bool> OvPhysics::Core::PhysicsEngine::m_collisionEvents;

namespace
{
	constexpr float kFixedTimeStep = 1.0f / 60.0f;
	constexpr int kMaxSubSteps = 10;
	constexpr uint32_t kMaxBodies = 65536;
	constexpr uint32_t kBodyMutexCount = 0; // 0 lets Jolt pick a default value
	constexpr uint32_t kMaxBodyPairs = 65536;
	constexpr uint32_t kMaxContactConstraints = 20480;
	constexpr uint32_t kObjectLayerCount = 1;
	constexpr uint32_t kBroadPhaseLayerCount = 1;
	constexpr size_t kTempAllocatorSize = 10 * 1024 * 1024;

	using PhysicalObjectEvent = OvTools::Eventing::Event<PhysicalObject&> PhysicalObject::*;

	/**
	* Invokes the trigger event of the target if it is a trigger, or its collision event if none of the objects is a trigger
	*/
	void InvokeContactEvent(PhysicalObject& p_target, PhysicalObject& p_other, PhysicalObjectEvent p_triggerEvent, PhysicalObjectEvent p_collisionEvent)
	{
		if (p_target.IsTrigger())
		{
			(p_target.*p_triggerEvent).Invoke(p_other);
		}
		else if (!p_other.IsTrigger())
		{
			(p_target.*p_collisionEvent).Invoke(p_other);
		}
	}
}

/**
* Collects the pairs of bodies in contact during the simulation (called from the simulation threads)
*/
class OvPhysics::Core::PhysicsEngine::ContactListener final : public JPH::ContactListener
{
public:
	void OnContactAdded(const JPH::Body& p_body1, const JPH::Body& p_body2, const JPH::ContactManifold&, JPH::ContactSettings&) override
	{
		AddContact(p_body1, p_body2);
	}

	void OnContactPersisted(const JPH::Body& p_body1, const JPH::Body& p_body2, const JPH::ContactManifold&, JPH::ContactSettings&) override
	{
		AddContact(p_body1, p_body2);
	}

	/**
	* Returns the collected pairs of bodies in contact and clears them
	*/
	std::vector<std::pair<JPH::BodyID, JPH::BodyID>> ConsumeContacts()
	{
		std::scoped_lock lock(m_mutex);
		return std::exchange(m_contacts, {});
	}

private:
	void AddContact(const JPH::Body& p_body1, const JPH::Body& p_body2)
	{
		std::scoped_lock lock(m_mutex);
		m_contacts.emplace_back(p_body1.GetID(), p_body2.GetID());
	}

private:
	std::mutex m_mutex;
	std::vector<std::pair<JPH::BodyID, JPH::BodyID>> m_contacts;
};

OvPhysics::Core::PhysicsEngine::PhysicsEngine(const Settings::PhysicsSettings & p_settings)
{
	JPH::RegisterDefaultAllocator();

	m_factory = std::make_unique<JPH::Factory>();
	JPH::Factory::sInstance = m_factory.get();
	JPH::RegisterTypes();

	m_tempAllocator = std::make_unique<JPH::TempAllocatorImpl>(kTempAllocatorSize);
	m_jobSystem = std::make_unique<JPH::JobSystemThreadPool>(JPH::cMaxPhysicsJobs, JPH::cMaxPhysicsBarriers);

	m_broadPhaseLayerInterface = std::make_unique<JPH::BroadPhaseLayerInterfaceTable>(kObjectLayerCount, kBroadPhaseLayerCount);
	m_broadPhaseLayerInterface->MapObjectToBroadPhaseLayer(PhysicalObject::kObjectLayer, JPH::BroadPhaseLayer(0));

	m_objectLayerPairFilter = std::make_unique<JPH::ObjectLayerPairFilterTable>(kObjectLayerCount);
	m_objectLayerPairFilter->EnableCollision(PhysicalObject::kObjectLayer, PhysicalObject::kObjectLayer);

	m_objectVsBroadPhaseLayerFilter = std::make_unique<JPH::ObjectVsBroadPhaseLayerFilterTable>(*m_broadPhaseLayerInterface, kBroadPhaseLayerCount, *m_objectLayerPairFilter, kObjectLayerCount);

	m_contactListener = std::make_unique<ContactListener>();

	m_physicsSystem = std::make_unique<JPH::PhysicsSystem>();
	m_physicsSystem->Init(kMaxBodies, kBodyMutexCount, kMaxBodyPairs, kMaxContactConstraints, *m_broadPhaseLayerInterface, *m_objectVsBroadPhaseLayerFilter, *m_objectLayerPairFilter);
	m_physicsSystem->SetContactListener(m_contactListener.get());
	m_physicsSystem->SetGravity(Conversion::ToJoltVector3(p_settings.gravity));

	// Friction and restitution of two bodies in contact are combined by multiplication
	m_physicsSystem->SetCombineFriction([](const JPH::Body& p_body1, const JPH::SubShapeID&, const JPH::Body& p_body2, const JPH::SubShapeID&)
	{
		return p_body1.GetFriction() * p_body2.GetFriction();
	});

	m_physicsSystem->SetCombineRestitution([](const JPH::Body& p_body1, const JPH::SubShapeID&, const JPH::Body& p_body2, const JPH::SubShapeID&)
	{
		return p_body1.GetRestitution() * p_body2.GetRestitution();
	});

	ListenToPhysicalObjects();
}

OvPhysics::Core::PhysicsEngine::~PhysicsEngine()
{
	m_physicsSystem.reset();

	JPH::UnregisterTypes();
	JPH::Factory::sInstance = nullptr;
}

void OvPhysics::Core::PhysicsEngine::PreUpdate()
{
	std::for_each(m_physicalObjects.begin(), m_physicalObjects.end(), std::mem_fn(&PhysicalObject::UpdateBodyTransform));

	ResetCollisionEvents();
}

void OvPhysics::Core::PhysicsEngine::PostUpdate()
{
	for (const auto& [body1, body2] : m_contactListener->ConsumeContacts())
	{
		CollisionCallback(body1, body2);
	}

	std::for_each(m_physicalObjects.begin(), m_physicalObjects.end(), std::mem_fn(&PhysicalObject::UpdateFTransform));

	CheckCollisionStopEvents();
}

bool OvPhysics::Core::PhysicsEngine::Update(float p_deltaTime)
{
	PreUpdate();

	m_accumulatedTime += p_deltaTime;

	if (m_accumulatedTime < kFixedTimeStep)
	{
		return false;
	}

	// Excess steps are dropped to avoid a spiral of death when the simulation can't keep up
	const int stepCount = static_cast<int>(m_accumulatedTime / kFixedTimeStep);
	const int simulatedStepCount = std::min(stepCount, kMaxSubSteps);
	m_accumulatedTime -= stepCount * kFixedTimeStep;

	m_physicsSystem->Update(simulatedStepCount * kFixedTimeStep, simulatedStepCount, m_tempAllocator.get(), m_jobSystem.get());

	PostUpdate();
	return true;
}

std::optional<RaycastHit> OvPhysics::Core::PhysicsEngine::Raycast(OvMaths::FVector3 p_origin, OvMaths::FVector3 p_direction, float p_distance)
{
	if (p_direction == OvMaths::FVector3::Zero)
		return {};

	const JPH::RRayCast ray(Tools::Conversion::ToJoltVector3(p_origin), Tools::Conversion::ToJoltVector3(p_direction * p_distance));
	const JPH::NarrowPhaseQuery& narrowPhaseQuery = m_physicsSystem->GetNarrowPhaseQuery();
	const JPH::BodyInterface& bodyInterface = m_physicsSystem->GetBodyInterface();

	RaycastHit resultHit;

	// Try to get First Hit
	JPH::RayCastResult closestHit;

	if (narrowPhaseQuery.CastRay(ray, closestHit))
	{
		// Get First Hit
		resultHit.FirstResultObject = reinterpret_cast<OvPhysics::Entities::PhysicalObject*>(bodyInterface.GetUserData(closestHit.mBodyID));

		// Try to get all Hit
		JPH::AllHitCollisionCollector<JPH::CastRayCollector> allHitsCollector;
		narrowPhaseQuery.CastRay(ray, JPH::RayCastSettings(), allHitsCollector);

		// Get all Hit
		for (const JPH::RayCastResult& hit : allHitsCollector.mHits)
			resultHit.ResultObjects.push_back(reinterpret_cast<OvPhysics::Entities::PhysicalObject*>(bodyInterface.GetUserData(hit.mBodyID)));

		return resultHit;
	}
	else
		return {};
}

void OvPhysics::Core::PhysicsEngine::SetGravity(const OvMaths::FVector3 & p_gravity)
{
	m_physicsSystem->SetGravity(Conversion::ToJoltVector3(p_gravity));
}

OvMaths::FVector3 OvPhysics::Core::PhysicsEngine::GetGravity() const
{
	return Conversion::ToOvVector3(m_physicsSystem->GetGravity());
}

void OvPhysics::Core::PhysicsEngine::ListenToPhysicalObjects()
{
	PhysicalObject::CreatedEvent += std::bind(static_cast<void(PhysicsEngine::*)(PhysicalObject&)>(&PhysicsEngine::Consider), this, std::placeholders::_1);
	PhysicalObject::DestroyedEvent += std::bind(static_cast<void(PhysicsEngine::*)(PhysicalObject&)>(&PhysicsEngine::Unconsider), this, std::placeholders::_1);
}

void OvPhysics::Core::PhysicsEngine::Consider(PhysicalObject& p_toConsider)
{
	p_toConsider.m_bodyInterface = &m_physicsSystem->GetBodyInterface();
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

void OvPhysics::Core::PhysicsEngine::ResetCollisionEvents()
{
	for (auto& element : m_collisionEvents)
		element.second = false;
}

void OvPhysics::Core::PhysicsEngine::CheckCollisionStopEvents()
{
	std::vector<std::pair<PhysicalObject*, PhysicalObject*>> stoppedPairs;

	for (const auto& [pair, touched] : m_collisionEvents)
	{
		if (!touched)
		{
			stoppedPairs.push_back(pair);
		}
	}

	// A pair is removed from the collision events when one of its objects is destroyed, possibly by a previous event
	for (const auto& pair : stoppedPairs)
	{
		const auto [object1, object2] = pair;

		if (m_collisionEvents.contains(pair))
		{
			InvokeContactEvent(*object1, *object2, &PhysicalObject::TriggerStopEvent, &PhysicalObject::CollisionStopEvent);
		}

		if (m_collisionEvents.contains(pair))
		{
			InvokeContactEvent(*object2, *object1, &PhysicalObject::TriggerStopEvent, &PhysicalObject::CollisionStopEvent);
		}

		m_collisionEvents.erase(pair);
	}
}

void OvPhysics::Core::PhysicsEngine::CollisionCallback(const JPH::BodyID& p_body1, const JPH::BodyID& p_body2)
{
	// A body destroyed by a previous collision event has no user data anymore
	const JPH::BodyInterface& bodyInterface = m_physicsSystem->GetBodyInterface();
	auto object1 = reinterpret_cast<PhysicalObject*>(bodyInterface.GetUserData(p_body1));
	auto object2 = reinterpret_cast<PhysicalObject*>(bodyInterface.GetUserData(p_body2));

	/* Contacts between two triggers don't generate any event */
	if (!object1 || !object2 || (object1->IsTrigger() && object2->IsTrigger()))
	{
		return;
	}

	const std::pair<PhysicalObject*, PhysicalObject*> pair{ object1, object2 };
	const auto found = m_collisionEvents.find(pair);
	const bool isNewPair = found == m_collisionEvents.end();

	/* The events of this pair have already been invoked during this update */
	if (!isNewPair && found->second)
	{
		return;
	}

	/* The pair is flagged before invoking its events, so that it gets removed if an event destroys one of its objects */
	m_collisionEvents[pair] = true;

	const auto invokeIfAlive = [&pair](PhysicalObject& p_target, PhysicalObject& p_other, PhysicalObjectEvent p_triggerEvent, PhysicalObjectEvent p_collisionEvent)
	{
		if (m_collisionEvents.contains(pair))
		{
			InvokeContactEvent(p_target, p_other, p_triggerEvent, p_collisionEvent);
		}
	};

	if (isNewPair)
	{
		invokeIfAlive(*object1, *object2, &PhysicalObject::TriggerStartEvent, &PhysicalObject::CollisionStartEvent);
		invokeIfAlive(*object2, *object1, &PhysicalObject::TriggerStartEvent, &PhysicalObject::CollisionStartEvent);
	}

	invokeIfAlive(*object1, *object2, &PhysicalObject::TriggerStayEvent, &PhysicalObject::CollisionStayEvent);
	invokeIfAlive(*object2, *object1, &PhysicalObject::TriggerStayEvent, &PhysicalObject::CollisionStayEvent);
}
