/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once


#include "OvCore/ECS/Actor.h"
#include "OvCore/API/ISerializable.h"

#include "OvCore/ECS/Components/CModelRenderer.h"
#include "OvCore/ECS/Components/CCamera.h"
#include "OvCore/ECS/Components/CLight.h"
#include "OvCore/ECS/Components/CPostProcessStack.h"

namespace OvCore::SceneSystem
{
	/**
	* The scene is a set of actors
	*/
	class Scene : public API::ISerializable
	{
	public:
		/**
		* Contains a set of vectors of components that are sorted. It allows fast
		* manipulation of components without parsing the whole scene
		*/
		struct FastAccessComponents
		{
			std::vector<ECS::Components::CModelRenderer*>	modelRenderers;
			std::vector<ECS::Components::CCamera*>			cameras;
			std::vector<ECS::Components::CLight*>			lights;
			std::vector<ECS::Components::CPostProcessStack*> postProcessStacks;
		};

		/**
		* Constructor of the scene
		*/
		Scene();

		/**
		* Handle the memory de-allocation of every actors
		*/
		~Scene();

		/**
		* Add a default camera to the scene
		*/
		void AddDefaultCamera();

		/**
		* Add default lights to the scene
		*/
		void AddDefaultLights();

		/**
		* Add default post-process stack to the scene
		*/
		void AddDefaultPostProcessStack();

		/**
		* Add default skysphere to the scene
		*/
		void AddDefaultSkysphere();

		/**
		* Play the scene
		*/
		void Play();

		/**
		* Returns true if the scene is playing
		*/
		bool IsPlaying() const;

		/**
		* Update every actors
		* @param p_deltaTime
		*/
		void Update(float p_deltaTime);

		/**
		* Update every actors 60 frames per seconds
		* @param p_deltaTime
		*/
		void FixedUpdate(float p_deltaTime);

		/**
		* Update every actors lately
		* @param p_deltaTime
		*/
		void LateUpdate(float p_deltaTime);

		/**
		* Create an actor with a default name and return a reference to it.
		*/
		ECS::Actor& CreateActor();

		/**
		* Create an actor with the given name and return a reference to it.
		* @param p_name
		* @param p_tag
		*/
		ECS::Actor& CreateActor(const std::string& p_name, const std::string& p_tag = "");

		/**
		* Destroy and actor and return true on success
		* @param p_target (The actor to remove from the scene)
		*/
		bool DestroyActor(ECS::Actor& p_target);

		/**
		* Collect garbages by removing Destroyed-marked actors
		*/
		void CollectGarbages();

		/**
		* Return the first actor identified by the given name, or nullptr on fail
		* @param p_name
		*/
		ECS::Actor* FindActorByName(const std::string& p_name) const;

		/**
		* Return the first actor identified by the given tag, or nullptr on fail
		* @param p_tag
		*/
		ECS::Actor* FindActorByTag(const std::string& p_tag) const;

		/**
		* Return the actor identified by the given ID (Returns nullptr on fail)
		* @param p_id
		*/
		ECS::Actor* FindActorByID(int64_t p_id) const;

		/**
		* Return every actors identified by the given name
		* @param p_name
		*/
		std::vector<std::reference_wrapper<ECS::Actor>> FindActorsByName(const std::string& p_name) const;

		/**
		* Return every actors identified by the given tag
		* @param p_tag
		*/
		std::vector<std::reference_wrapper<ECS::Actor>> FindActorsByTag(const std::string& p_tag) const;

		/**
		* Parse the scene to find the main camera
		*/
		OvCore::ECS::Components::CCamera* FindMainCamera() const;

		/**
		* Callback method called everytime a component is added on an actor of the scene
		* @param p_component
		*/
		void OnComponentAdded(ECS::Components::AComponent& p_compononent);

		/**
		* Callback method called everytime a component is removed on an actor of the scene
		* @param p_component
		*/
		void OnComponentRemoved(ECS::Components::AComponent& p_compononent);

		/**
		* Return a reference on the actor map
		*/
		std::vector<OvCore::ECS::Actor*>& GetActors();

		/**
		* Return the fast access components data structure
		*/
		const FastAccessComponents& GetFastAccessComponents() const;

		/**
		* Serialize the scene
		* @param p_doc
		* @param p_root
		*/
		virtual void OnSerialize(tinyxml2::XMLDocument& p_doc, tinyxml2::XMLNode* p_root) override;

		/**
		* Deserialize the scene
		* @param p_doc
		* @param p_root
		*/
		virtual void OnDeserialize(tinyxml2::XMLDocument& p_doc, tinyxml2::XMLNode* p_root) override;

	private:
		int64_t m_availableID = 1;
		bool m_isPlaying = false;
		std::vector<ECS::Actor*> m_actors;

		FastAccessComponents m_fastAccessComponents;
	};
}