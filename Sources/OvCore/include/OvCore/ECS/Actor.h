/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include <unordered_map>
#include <memory>

#include <OvTools/Eventing/Event.h>

#include "OvCore/ECS/Components/AComponent.h"
#include "OvCore/ECS/Components/CTransform.h"
#include "OvCore/ECS/Components/Behaviour.h"
#include "OvCore/API/ISerializable.h"

namespace OvCore::ECS
{
	/**
	* The Actor is the main class of the ECS, it corresponds to the entity and is
	* composed of componenents and behaviours (scripts)
	*/
	class Actor : public API::ISerializable
	{
	public:
		/**
		* Constructor of the actor. It will automatically add a transform component
		* @param p_actorID
		* @param p_name
		* @param p_tag
		* @param p_playing
		*/
		Actor(int64_t p_actorID, const std::string& p_name, const std::string& p_tag, bool& p_playing);

		/**
		* Destructor of the actor instance. Force invoke ComponentRemovedEvent and BehaviourRemovedEvent
		* for every components and behaviours
		*/
		virtual ~Actor() override;

		/**
		* Return the current name of the actor
		*/
		const std::string& GetName() const;

		/**
		* Return the current tag of the actor
		*/
		const std::string& GetTag() const;

		/**
		* Defines a new name for the actor
		* @param p_name
		*/
		void SetName(const std::string& p_name);

		/**
		* Defines a new tag for the actor
		* @param p_tag
		*/
		void SetTag(const std::string& p_tag);

		/**
		* Enable or disable the actor
		* @param p_active
		*/
		void SetActive(bool p_active);

		/**
		* Returns true if the actor is active, ignoring his parent (if any) active state
		*/
		bool IsSelfActive() const;

		/**
		* Returns true if the actor is and his recursive parents (if any) are active
		*/
		bool IsActive() const;

		/**
		* Defines a new ID for the actor
		* @param p_id
		*/
		void SetID(int64_t p_id);

		/**
		* Returns the ID of the actor
		*/
		int64_t GetID() const;

		/**
		* Set an actor as the parent of this actor
		* @param p_parent
		*/
		void SetParent(Actor& p_parent);

		/**
		* Detach from the parent
		*/
		void DetachFromParent();

		/**
		* Returns true if this actor transform is descendant of the actor
		* @param p_actor
		*/
		bool IsDescendantOf(const Actor* p_actor) const;

		/**
		* Returns true if the actor has a parent
		*/
		bool HasParent() const;

		/**
		* Returns the parents of this actor (Or nullptr if no parent)
		*/
		Actor* GetParent() const;

		/**
		* Returns the ID of the parent of this actor
		*/
		int64_t GetParentID() const;

		/**
		* Returns the children of this actor
		*/
		std::vector<Actor*>& GetChildren();

		/**
		* Mark the Actor as "Destroyed". A "Destroyed" actor will be removed from the scene by the scene itself
		*/
		void MarkAsDestroy();

		/**
		* Returns true if the actor is not marked as destroyed
		*/
		bool IsAlive() const;

		/**
		* Defines if the actor is sleeping or not.
		* A sleeping actor will not trigger methods suchs as OnEnable, OnDisable and OnDestroyed
		* @param p_sleeping
		*/
		void SetSleeping(bool p_sleeping);

		/**
		* Called when the scene start or when the actor gets enabled for the first time during play mode
		* This method will always be called in an ordered triple:
		* - OnAwake()
		* - OnEnable()
		* - OnStart()
		*/
		void OnAwake();

		/**
		* Called when the scene start or when the actor gets enabled for the first time during play mode
		* This method will always be called in an ordered triple:
		* - OnAwake()
		* - OnEnable()
		* - OnStart()
		*/
		void OnStart();

		/**
		* Called when the actor gets enabled (SetActive set to true) or at scene start if the actor is hierarchically active.
		* This method can be called in an ordered triple at scene start:
		* - OnAwake()
		* - OnEnable()
		* - OnStart()
		* Or can be called solo if the actor hierarchical active state changed to true and the actor already gets awaked
		* Conditions:
		* - Play mode only
		*/
		void OnEnable();

		/**
		* Called when the actor hierarchical active state changed to false or gets destroyed while being hierarchically active
		* Conditions:
		* - Play mode only
		*/
		void OnDisable();

		/**
		* Called when the actor gets destroyed if it has been awaked
		* Conditions:
		* - Play mode only
		*/
		void OnDestroy();

		/**
		* Called every frame
		* @param p_deltaTime
		*/
		void OnUpdate(float p_deltaTime);

		/**
		* Called every physics frame
		* @param p_deltaTime
		*/
		void OnFixedUpdate(float p_deltaTime);

		/**
		* Called every frame after OnUpdate
		* @param p_deltaTime
		*/
		void OnLateUpdate(float p_deltaTime);

		/**
		* Called when the actor enter in collision with another physical object
		* @param p_otherObject
		*/
		void OnCollisionEnter(Components::CPhysicalObject& p_otherObject);

		/**
		* Called when the actor is in collision with another physical object
		* @param p_otherObject
		*/
		void OnCollisionStay(Components::CPhysicalObject& p_otherObject);

		/**
		* Called when the actor exit from collision with another physical object
		* @param p_otherObject
		*/
		void OnCollisionExit(Components::CPhysicalObject& p_otherObject);

		/**
		* Called when the actor enter in trigger with another physical object
		* @param p_otherObject
		*/
		void OnTriggerEnter(Components::CPhysicalObject& p_otherObject);

		/**
		* Called when the actor is in trigger with another physical object
		* @param p_otherObject
		*/
		void OnTriggerStay(Components::CPhysicalObject& p_otherObject);

		/**
		* Called when the actor exit from trigger with another physical object
		* @param p_otherObject
		*/
		void OnTriggerExit(Components::CPhysicalObject& p_otherObject);

		/**
		* Add a component to the actor (Or return the component if already existing)
		* @param p_args (Parameter pack forwared to the component constructor)
		*/
		template<typename T, typename ... Args>
		T& AddComponent(Args&&... p_args);

		/**
		* Remove the given component
		*/
		template<typename T>
		bool RemoveComponent();

		/**
		* Remove the component by refering to the given instance
		* @param p_component
		*/
		bool RemoveComponent(OvCore::ECS::Components::AComponent& p_component);

		/**
		* Try to get the given component (Returns nullptr on failure)
		*/
		template<typename T>
		T* GetComponent() const;

		/**
		* Returns a reference to the vector of components
		*/
		std::vector<std::shared_ptr<Components::AComponent>>& GetComponents();

		/**
		* Add a behaviour to the actor
		* @param p_name
		*/
		Components::Behaviour& AddBehaviour(const std::string& p_name);

		/**
		* Remove a behaviour by refering to the given instance
		* @param p_behaviour
		*/
		bool RemoveBehaviour(Components::Behaviour& p_behaviour);

		/**
		* Remove a behaviour by refering to his name
		* @param p_name
		*/
		bool RemoveBehaviour(const std::string& p_name);

		/**
		* Try to get the given behaviour (Returns nullptr on failure)
		* @param p_name
		*/
		Components::Behaviour* GetBehaviour(const std::string& p_name);

		/**
		* Returns a reference to the vector of behaviours
		*/
		std::unordered_map<std::string, Components::Behaviour>& GetBehaviours();

		/**
		* Serialize all the components
		*/
		virtual void OnSerialize(tinyxml2::XMLDocument& p_doc, tinyxml2::XMLNode* p_actorsRoot) override;

		/**
		* Deserialize all the components
		*/
		virtual void OnDeserialize(tinyxml2::XMLDocument& p_doc, tinyxml2::XMLNode* p_actorsRoot) override;

	private:
		/**
		 * @brief Deleted copy constructor
		 * @param p_actor
		 */
		Actor(const Actor& p_actor) = delete;

		void RecursiveActiveUpdate();
		void RecursiveWasActiveUpdate();

	public:
		/* Some events that are triggered when an action occur on the actor instance */
		OvTools::Eventing::Event<Components::AComponent&>	ComponentAddedEvent;
		OvTools::Eventing::Event<Components::AComponent&>	ComponentRemovedEvent;
		OvTools::Eventing::Event<Components::Behaviour&>	BehaviourAddedEvent;
		OvTools::Eventing::Event<Components::Behaviour&>	BehaviourRemovedEvent;

		/* Some events that are triggered when an action occur on any actor */
		static OvTools::Eventing::Event<Actor&>				DestroyedEvent;
		static OvTools::Eventing::Event<Actor&>				CreatedEvent;
		static OvTools::Eventing::Event<Actor&, Actor&>		AttachEvent;
		static OvTools::Eventing::Event<Actor&>				DettachEvent;

	private:
		/* Settings */
		std::string		m_name;
		std::string		m_tag;
		bool			m_active = true;
		bool&			m_playing;

		/* Internal settings */
		int64_t	m_actorID;
		bool	m_destroyed = false;
		bool	m_sleeping = true;
		bool	m_awaked = false;
		bool	m_started = false;
		bool	m_wasActive = false;

		/* Parenting system stuff */
		int64_t					m_parentID = 0;
		Actor*					m_parent = nullptr;
		std::vector<Actor*>		m_children;

		/* Actors components */
		std::vector<std::shared_ptr<Components::AComponent>> m_components;
		std::unordered_map<std::string, Components::Behaviour> m_behaviours;

	public:
		Components::CTransform& transform;
	};
}

#include "OvCore/ECS/Actor.inl"