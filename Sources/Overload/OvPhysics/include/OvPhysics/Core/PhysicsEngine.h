/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include <bullet/btBulletCollisionCommon.h>
#include <bullet/btBulletDynamicsCommon.h>

#include <map>
#include <optional>
#include <vector>

#include <OvPhysics/Entities/PhysicalObject.h>
#include <OvPhysics/Entities/RaycastHit.h>
#include <OvPhysics/Settings/PhysicsSettings.h>

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
		* Simulate the physics. This method call is decomposed in 3 things:
		* - Pre-Update (Apply FTransforms to btTransforms, called every Update call)
		* - Simulation (Simulate the physics, called 60 times per seconds)
		* - Post-Update (Apply the simulation results, btTransforms, to FTransforms)
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

		void Consider(btRigidBody& p_toConsider);
		void Unconsider(btRigidBody& p_toUnconsider);

		void ResetCollisionEvents();
		void CheckCollisionStopEvents();

		static bool CollisionCallback(btManifoldPoint& cp, const btCollisionObjectWrapper* obj1, int id1, int index1, const btCollisionObjectWrapper* obj2, int id2, int index2);
		void SetCollisionCallback();

	private:
		/* Bullet world */
		std::unique_ptr<btDynamicsWorld> m_world;
		std::unique_ptr<btDispatcher> m_dispatcher;
		std::unique_ptr<btCollisionConfiguration> m_collisionConfig;
		std::unique_ptr<btBroadphaseInterface> m_broadphase;
		std::unique_ptr<btConstraintSolver> m_solver;

		static std::map<std::pair<Entities::PhysicalObject*, Entities::PhysicalObject*>, bool> m_collisionEvents;
		std::vector<std::reference_wrapper<Entities::PhysicalObject>> m_physicalObjects;
	};
}
