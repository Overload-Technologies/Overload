/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include <cstdint>
#include <filesystem>
#include <functional>
#include <vector>

namespace tinyxml2
{
	class XMLDocument;
	class XMLNode;
	class XMLElement;
}

namespace OvCore::ECS
{
	class Actor;
}

namespace OvCore::SceneSystem
{
	class Scene;
}

namespace OvEditor::Utils
{
	/**
	* Structure describing a serialized actor entry inside a prefab file.
	*/
	struct PrefabActorEntry
	{
		tinyxml2::XMLElement* actorNode = nullptr;
		int64_t id = 0;
		int64_t parentID = 0;
	};

	/**
	* Class exposing utility functions to serialize and deserialize prefab hierarchies.
	*/
	class PrefabUtility
	{
	public:
		/**
		* Save the given actor hierarchy to the prefab file.
		* @param p_rootActor
		* @param p_prefabPath
		*/
		static bool SaveActorHierarchyAsPrefab(OvCore::ECS::Actor& p_rootActor, const std::filesystem::path& p_prefabPath);

		/**
		* Load and validate serialized actor entries from a prefab file.
		* @param p_prefabPath
		* @param p_doc
		* @param p_outActors
		*/
		static bool LoadPrefabActors(
			const std::filesystem::path& p_prefabPath,
			tinyxml2::XMLDocument& p_doc,
			std::vector<PrefabActorEntry>& p_outActors
		);

		/**
		* Deserialize the prefab hierarchy into actors.
		* @param p_doc
		* @param p_entries
		* @param p_rootActor
		* @param p_createActor
		* @param p_orphanParent
		*/
		static bool DeserializePrefabHierarchy(
			tinyxml2::XMLDocument& p_doc,
			const std::vector<PrefabActorEntry>& p_entries,
			OvCore::ECS::Actor& p_rootActor,
			std::function<OvCore::ECS::Actor&()> p_createActor,
			OvCore::ECS::Actor* p_orphanParent = nullptr
		);

		/**
		* Remove children, non-transform components and behaviours from an actor.
		* @param p_scene
		* @param p_actor
		*/
		static void ClearActorHierarchyContent(OvCore::SceneSystem::Scene& p_scene, OvCore::ECS::Actor& p_actor);

	private:
		static void SerializeActorHierarchy(
			OvCore::ECS::Actor& p_actor,
			tinyxml2::XMLDocument& p_doc,
			tinyxml2::XMLNode* p_actorsRoot
		);

		static std::vector<PrefabActorEntry> GatherSerializedActorEntries(tinyxml2::XMLNode* p_actorsRoot);
		static const PrefabActorEntry* FindRootActor(const std::vector<PrefabActorEntry>& p_entries);
		static void DeserializeActorPreservingIdentity(
			OvCore::ECS::Actor& p_actor,
			tinyxml2::XMLDocument& p_doc,
			tinyxml2::XMLElement* p_actorNode
		);
	};
}
