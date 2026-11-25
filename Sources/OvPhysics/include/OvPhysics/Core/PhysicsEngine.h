/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include <map>
#include <optional>
#include <vector>

#include <OvPhysics/Entities/PhysicalObject.h>
#include <OvPhysics/Entities/RaycastHit.h>
#include <OvPhysics/Settings/PhysicsSettings.h>

namespace JPH
{
	class BodyID;
	class BroadPhaseLayerInterfaceTable;
	class Factory;
	class JobSystem;
	class ObjectLayerPairFilterTable;
	class ObjectVsBroadPhaseLayerFilterTable;
	class PhysicsSystem;
	class TempAllocator;
}

namespace OvPhysics::Core
{
	/**
	* Main class of OvPhysics, it handles the creation of the physical world. It must be created
	* before any PhysicalObject to ensure PhysicalObject consideration
	*/
	class PhysicsEngine
	{
	public:
		/**
		* Creates the PhysicsEngine
		* @param p_settings
		*/
		PhysicsEngine(const Settings::PhysicsSettings& p_settings);

		/**
		* Destructor
		*/
		virtual ~PhysicsEngine();

		/**
		* Simulate the physics. This method call is decomposed in 3 things:
		* - Pre-Update (Apply FTransforms to physical bodies, called every Update call)
		* - Simulation (Simulate the physics, called 60 times per seconds)
		* - Post-Update (Apply the simulation results, physical bodies transforms, to FTransforms)
		* This methods returns true if the call invoked a physics simulation
		*/
		bool Update(float p_deltaTime);

		/* Casts a ray against all Physical Object in the Scene and returns information on what was hit
		 * @param p_origin
		 * @param p_end
		 */
		std::optional<Entities::RaycastHit> Raycast(OvMaths::FVector3 p_origin, OvMaths::FVector3 p_direction, float p_distance);

		/**
		* Defines the world gravity to apply
		* @param p_gravity
		*/
		void SetGravity(const OvMaths::FVector3& p_gravity);

		/**
		* Returns the current world gravity
		*/
		OvMaths::FVector3 GetGravity() const;

	private:
		void PreUpdate();
		void PostUpdate();

		void ListenToPhysicalObjects();

		void Consider(OvPhysics::Entities::PhysicalObject& p_toConsider);
		void Unconsider(OvPhysics::Entities::PhysicalObject& p_toUnconsider);

		void ResetCollisionEvents();
		void CheckCollisionStopEvents();

		void CollisionCallback(const JPH::BodyID& p_body1, const JPH::BodyID& p_body2);

	private:
		class ContactListener;

		/* Jolt world */
		std::unique_ptr<JPH::Factory> m_factory;
		std::unique_ptr<JPH::TempAllocator> m_tempAllocator;
		std::unique_ptr<JPH::JobSystem> m_jobSystem;
		std::unique_ptr<JPH::BroadPhaseLayerInterfaceTable> m_broadPhaseLayerInterface;
		std::unique_ptr<JPH::ObjectLayerPairFilterTable> m_objectLayerPairFilter;
		std::unique_ptr<JPH::ObjectVsBroadPhaseLayerFilterTable> m_objectVsBroadPhaseLayerFilter;
		std::unique_ptr<ContactListener> m_contactListener;
		std::unique_ptr<JPH::PhysicsSystem> m_physicsSystem;
		float m_accumulatedTime = 0.0f;

		static std::map<std::pair<Entities::PhysicalObject*, Entities::PhysicalObject*>, bool> m_collisionEvents;
		std::vector<std::reference_wrapper<Entities::PhysicalObject>> m_physicalObjects;
	};
}
