/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include <OvCore/ECS/Components/CPhysicalObject.h>
#include <OvTools/Utils/OptRef.h>
#include <OvCore/Scripting/ScriptEngine.h>

namespace OvCore::ECS { class Actor; }

namespace OvCore::ECS::Components
{
	/**
	* A Behaviour is a script that can be attached to an actor
	*/
	class Behaviour : public AComponent
	{
	public:
		/**
		* Constructor of a Behaviour
		* @param p_owner
		* @param p_scriptPath
		*/
		Behaviour(ECS::Actor& p_owner, const std::string& p_scriptPath);

		/**
		* Destructor
		*/
		~Behaviour();

		/**
		* Returns the name of the component
		*/
		virtual std::string GetName() override;

		/**
		* Returns the script name associated with this behaviour
		*/
		std::string GetScriptName() const;

		/**
		* Returns the path of the script associated with this behaviour
		*/
		std::string GetScriptPath() const;

		/**
		* Sets the script associated with this behaviour
		* @param p_script
		*/
		void SetScript(std::unique_ptr<Scripting::Script>&& p_script);

		/**
		* Returns the script context of this behaviour
		*/
		OvTools::Utils::OptRef<Scripting::Script> GetScript();

		/**
		* Removes the script associated with this behaviour
		*/
		void RemoveScript();

		/**
		* Called when the scene start right before OnStart
		* It allows you to apply prioritized game logic on scene start
		*/
		virtual void OnAwake() override;

		/**
		* Called when the scene start right after OnAwake
		* It allows you to apply prioritized game logic on scene start
		*/
		virtual void OnStart() override;

		/**
		* Called when the behaviour gets enabled (owner SetActive set to true)
		*/
		virtual void OnEnable() override;

		/**
		* Called when the behaviour gets disabled (owner SetActive set to false)
		*/
		virtual void OnDisable() override;

		/**
		* Called when the behaviour gets destroyed
		*/
		virtual void OnDestroy() override;

		/**
		* Called every frame
		* @param p_deltaTime
		*/
		virtual void OnUpdate(float p_deltaTime) override;

		/**
		* Called every physics frame
		* @param p_deltaTime
		*/
		virtual void OnFixedUpdate(float p_deltaTime) override;

		/**
		* Called every frame after OnUpdate
		* @param p_deltaTime
		*/
		virtual void OnLateUpdate(float p_deltaTime) override;

		/**
		* Called when the owner of this component enter in collision with another physical object
		* @param p_otherObject
		*/
		virtual void OnCollisionEnter(Components::CPhysicalObject& p_otherObject) override;

		/**
		* Called when the owner of this component is in collision with another physical object
		* @param p_otherObject
		*/
		virtual void OnCollisionStay(Components::CPhysicalObject& p_otherObject) override;

		/**
		* Called when the owner of this component exit from collision with another physical object
		* @param p_otherObject
		*/
		virtual void OnCollisionExit(Components::CPhysicalObject& p_otherObject) override;

		/**
		* Called when the owner of this component enter in trigger with another physical object
		* @param p_otherObject
		*/
		virtual void OnTriggerEnter(Components::CPhysicalObject& p_otherObject) override;

		/**
		* Called when the owner of this component is in trigger with another physical object
		* @param p_otherObject
		*/
		virtual void OnTriggerStay(Components::CPhysicalObject& p_otherObject) override;

		/**
		* Called when the owner of this component exit from trigger with another physical object
		* @param p_otherObject
		*/
		virtual void OnTriggerExit(Components::CPhysicalObject& p_otherObject) override;

		/**
		* Serialize the behaviour
		* @param p_doc
		* @param p_node
		*/
		virtual void OnSerialize(tinyxml2::XMLDocument & p_doc, tinyxml2::XMLNode * p_node) override;

		/**
		* Deserialize the behaviour
		* @param p_doc
		* @param p_node
		*/
		virtual void OnDeserialize(tinyxml2::XMLDocument & p_doc, tinyxml2::XMLNode * p_node) override;

		/**
		* Defines how the behaviour should be drawn in the inspector
		* @param p_root
		*/
		virtual void OnInspector(OvUI::Internal::WidgetContainer & p_root) override;

	private:
		const std::string m_scriptName;
		const std::string m_scriptPath;
		std::unique_ptr<Scripting::Script> m_script;
	};
}
