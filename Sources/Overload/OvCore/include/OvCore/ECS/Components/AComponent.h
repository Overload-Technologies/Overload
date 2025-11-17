/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include "OvCore/API/IInspectorItem.h"

namespace OvCore::ECS { class Actor; }

namespace OvCore::ECS::Components
{
	class CPhysicalObject;

	/**
	* AComponent is the base class for any component.
	* A component is a set of data and behaviours (Entity-Component without systems) that is interpreted by the engine (Or the user)
	*/
	class AComponent : public API::IInspectorItem
	{
	public:
		/**
		* Constructor of a AComponent (Must be called by derived classes)
		* @param p_owner
		*/
		AComponent(ECS::Actor& p_owner);

		/**
		* Destroying a AComponent will call dervied classes destructors
		*/
		virtual ~AComponent();

		/**
		* Called when the scene start right before OnStart
		* It allows you to apply prioritized game logic on scene start
		*/
		virtual void OnAwake() {}

		/**
		* Called when the scene start right after OnAwake
		* It allows you to apply prioritized game logic on scene start
		*/
		virtual void OnStart() {}

		/**
		* Called when the components gets enabled (owner SetActive set to true) and after OnAwake() on scene starts
		*/
		virtual void OnEnable() {}

		/**
		* Called when the component gets disabled (owner SetActive set to false) and before OnDestroy() when the component get destroyed
		*/
		virtual void OnDisable() {}

		/**
		* Called when the components gets destroyed
		*/
		virtual void OnDestroy() {}

		/**
		* Called every frame
		* @param p_deltaTime
		*/
		virtual void OnUpdate(float p_deltaTime) {}

		/**
		* Called every physics frame
		* @param p_deltaTime
		*/
		virtual void OnFixedUpdate(float p_deltaTime) {}

		/**
		* Called every frame after OnUpdate
		* @param p_deltaTime
		*/
		virtual void OnLateUpdate(float p_deltaTime) {}

		/**
		* Called when the owner of this component enter in collision with another physical object
		* @param p_otherObject
		*/
		virtual void OnCollisionEnter(Components::CPhysicalObject& p_otherObject) {}

		/**
		* Called when the owner of this component is in collision with another physical object
		* @param p_otherObject
		*/
		virtual void OnCollisionStay(Components::CPhysicalObject& p_otherObject) {}

		/**
		* Called when the owner of this component exit from collision with another physical object
		* @param p_otherObject
		*/
		virtual void OnCollisionExit(Components::CPhysicalObject& p_otherObject) {}

		/**
		* Called when the owner of this component enter in trigger with another physical object
		* @param p_otherObject
		*/
		virtual void OnTriggerEnter(Components::CPhysicalObject& p_otherObject) {}

		/**
		* Called when the owner of this component is in trigger with another physical object
		* @param p_otherObject
		*/
		virtual void OnTriggerStay(Components::CPhysicalObject& p_otherObject) {}

		/**
		* Called when the owner of this component exit from trigger with another physical object
		* @param p_otherObject
		*/
		virtual void OnTriggerExit(Components::CPhysicalObject& p_otherObject) {}

		/**
		* Returns the name of the component
		*/
		virtual std::string GetName() = 0;

	public:
		ECS::Actor& owner;
	};

	template<typename T>
	struct ComponentTraits
	{
		static constexpr std::string_view Name = "Unknown Component Type";
	};
}