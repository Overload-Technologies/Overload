/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include <unordered_map>

#include <tinyxml2.h>

#include <OvCore/ECS/Actor.h>
#include <OvCore/SceneSystem/Scene.h>
#include <OvDebug/Logger.h>
#include <OvEditor/Utils/PrefabUtility.h>

void OvEditor::Utils::PrefabUtility::SerializeActorHierarchy(
	OvCore::ECS::Actor& p_actor,
	tinyxml2::XMLDocument& p_doc,
	tinyxml2::XMLNode* p_actorsRoot
)
{
	p_actor.OnSerialize(p_doc, p_actorsRoot);

	for (auto* child : p_actor.GetChildren())
	{
		SerializeActorHierarchy(*child, p_doc, p_actorsRoot);
	}
}

std::vector<OvEditor::Utils::PrefabActorEntry> OvEditor::Utils::PrefabUtility::GatherSerializedActorEntries(tinyxml2::XMLNode* p_actorsRoot)
{
	std::vector<PrefabActorEntry> entries;

	if (!p_actorsRoot)
	{
		return entries;
	}

	for (auto* actorNode = p_actorsRoot->FirstChildElement("actor"); actorNode; actorNode = actorNode->NextSiblingElement("actor"))
	{
		PrefabActorEntry entry{};
		entry.actorNode = actorNode;

		if (auto* idNode = actorNode->FirstChildElement("id"))
		{
			idNode->QueryInt64Text(&entry.id);
		}

		if (auto* parentNode = actorNode->FirstChildElement("parent"))
		{
			parentNode->QueryInt64Text(&entry.parentID);
		}

		entries.push_back(entry);
	}

	return entries;
}

bool OvEditor::Utils::PrefabUtility::SaveActorHierarchyAsPrefab(OvCore::ECS::Actor& p_rootActor, const std::filesystem::path& p_prefabPath)
{
	std::error_code errorCode;
	if (const auto parentPath = p_prefabPath.parent_path(); !parentPath.empty())
	{
		std::filesystem::create_directories(parentPath, errorCode);
		if (errorCode)
		{
			OVLOG_ERROR("Failed to create prefab directory: " + parentPath.string());
			return false;
		}
	}

	tinyxml2::XMLDocument doc;
	auto* rootNode = doc.NewElement("root");
	doc.InsertFirstChild(rootNode);

	auto* prefabNode = doc.NewElement("prefab");
	rootNode->InsertEndChild(prefabNode);

	auto* actorsNode = doc.NewElement("actors");
	prefabNode->InsertEndChild(actorsNode);

	SerializeActorHierarchy(p_rootActor, doc, actorsNode);

	if (auto* serializedRootActor = actorsNode->FirstChildElement("actor"))
	{
		if (auto* parentNode = serializedRootActor->FirstChildElement("parent"))
		{
			parentNode->SetText(static_cast<int64_t>(0));
		}
	}

	return doc.SaveFile(p_prefabPath.string().c_str()) == tinyxml2::XMLError::XML_SUCCESS;
}

bool OvEditor::Utils::PrefabUtility::LoadPrefabActors(
	const std::filesystem::path& p_prefabPath,
	tinyxml2::XMLDocument& p_doc,
	std::vector<PrefabActorEntry>& p_outActors
)
{
	if (p_doc.LoadFile(p_prefabPath.string().c_str()) != tinyxml2::XMLError::XML_SUCCESS)
	{
		return false;
	}

	auto* rootNode = p_doc.FirstChildElement("root");
	if (!rootNode)
	{
		return false;
	}

	auto* prefabNode = rootNode->FirstChildElement("prefab");
	if (!prefabNode)
	{
		return false;
	}

	auto* actorsNode = prefabNode->FirstChildElement("actors");
	if (!actorsNode)
	{
		return false;
	}

	p_outActors = GatherSerializedActorEntries(actorsNode);
	return FindRootActor(p_outActors) != nullptr;
}

const OvEditor::Utils::PrefabActorEntry* OvEditor::Utils::PrefabUtility::FindRootActor(const std::vector<PrefabActorEntry>& p_entries)
{
	for (const auto& entry : p_entries)
	{
		if (entry.parentID == 0 && entry.actorNode)
		{
			return &entry;
		}
	}

	return nullptr;
}

void OvEditor::Utils::PrefabUtility::DeserializeActorPreservingIdentity(
	OvCore::ECS::Actor& p_actor,
	tinyxml2::XMLDocument& p_doc,
	tinyxml2::XMLElement* p_actorNode
)
{
	const int64_t actorID = p_actor.GetID();
	const uint64_t actorGUID = p_actor.GetGUID();

	p_actor.OnDeserialize(p_doc, p_actorNode);
	p_actor.SetID(actorID);
	p_actor.SetGUID(actorGUID);
}

bool OvEditor::Utils::PrefabUtility::DeserializePrefabHierarchy(
	tinyxml2::XMLDocument& p_doc,
	const std::vector<PrefabActorEntry>& p_entries,
	OvCore::ECS::Actor& p_rootActor,
	std::function<OvCore::ECS::Actor&()> p_createActor,
	OvCore::ECS::Actor* p_orphanParent
)
{
	const PrefabActorEntry* rootEntry = FindRootActor(p_entries);
	if (!rootEntry || !rootEntry->actorNode)
	{
		return false;
	}

	DeserializeActorPreservingIdentity(p_rootActor, p_doc, rootEntry->actorNode);

	std::unordered_map<int64_t, OvCore::ECS::Actor*> actorBySerializedID;
	actorBySerializedID[rootEntry->id] = &p_rootActor;

	for (const auto& entry : p_entries)
	{
		if (entry.actorNode == rootEntry->actorNode || !entry.actorNode)
		{
			continue;
		}

		auto& newActor = p_createActor();
		DeserializeActorPreservingIdentity(newActor, p_doc, entry.actorNode);
		actorBySerializedID[entry.id] = &newActor;
	}

	for (const auto& entry : p_entries)
	{
		if (entry.actorNode == rootEntry->actorNode)
		{
			continue;
		}

		const auto actorIt = actorBySerializedID.find(entry.id);
		if (actorIt == actorBySerializedID.end() || !actorIt->second)
		{
			continue;
		}

		auto* actor = actorIt->second;
		const auto parentActor = actorBySerializedID.find(entry.parentID);

		if (parentActor != actorBySerializedID.end() && parentActor->second)
		{
			actor->SetParent(*parentActor->second);
		}
		else if (p_orphanParent)
		{
			actor->SetParent(*p_orphanParent);
		}
		else
		{
			actor->DetachFromParent();
		}
	}

	return true;
}

void OvEditor::Utils::PrefabUtility::ClearActorHierarchyContent(OvCore::SceneSystem::Scene& p_scene, OvCore::ECS::Actor& p_actor)
{
	const std::vector<OvCore::ECS::Actor*> children = p_actor.GetChildren();
	for (auto* child : children)
	{
		p_scene.DestroyActor(*child);
	}

	std::vector<OvCore::ECS::Components::AComponent*> componentsToRemove;
	for (const auto& component : p_actor.GetComponents())
	{
		if (component.get() != &p_actor.transform)
		{
			componentsToRemove.push_back(component.get());
		}
	}

	for (auto* component : componentsToRemove)
	{
		p_actor.RemoveComponent(*component);
	}

	const std::vector<std::string> behaviourNames = p_actor.GetBehavioursOrder();
	for (const auto& behaviourName : behaviourNames)
	{
		p_actor.RemoveBehaviour(behaviourName);
	}
}
