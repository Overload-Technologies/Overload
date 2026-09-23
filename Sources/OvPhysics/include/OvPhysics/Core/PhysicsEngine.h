/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include <cstdint>
#include <map>
#include <optional>
#include <vector>

#include <OvPhysics/Entities/PhysicalObject.h>
#include <OvPhysics/Entities/RaycastHit.h>
#include <OvPhysics/Settings/CollisionLayers.h>
#include <OvPhysics/Settings/PhysicsSettings.h>

class btDynamicsWorld;
class btDispatcher;
class btCollisionConfiguration;
class btBroadphaseInterface;
class btConstraintSolver;
class btRigidBody;
class btManifoldPoint;
struct btCollisionObjectWrapper;
struct btOverlapFilterCallback;

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

		/**
		* Returns the collision layers used to filter collisions
		*/
		const Settings::CollisionLayers& GetCollisionLayers() const;

		/**
		* Defines the collision layers used to filter collisions. Physical objects keep the layer
		* index they are on, layer slots being stable
		* @param p_collisionLayers
		*/
		void SetCollisionLayers(const Settings::CollisionLayers& p_collisionLayers);

		/**
		* Defines if the two given collision layers should collide together
		* @param p_first
		* @param p_second
		* @param p_collide
		*/
		void SetLayerCollision(uint32_t p_first, uint32_t p_second, bool p_collide);

	private:
		void MarkCollisionFiltersDirty();

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
		std::unique_ptr<btOverlapFilterCallback> m_collisionFilter; // Outlives the world, which holds it
		std::unique_ptr<btDynamicsWorld> m_world;
		std::unique_ptr<btDispatcher> m_dispatcher;
		std::unique_ptr<btCollisionConfiguration> m_collisionConfig;
		std::unique_ptr<btBroadphaseInterface> m_broadphase;
		std::unique_ptr<btConstraintSolver> m_solver;

		static std::map<std::pair<Entities::PhysicalObject*, Entities::PhysicalObject*>, bool> m_collisionEvents;
		std::vector<std::reference_wrapper<Entities::PhysicalObject>> m_physicalObjects;
		Settings::CollisionLayers m_collisionLayers;
	};
}
