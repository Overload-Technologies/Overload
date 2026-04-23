/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include <cstdint>
#include <unordered_map>
#include <vector>

#include <tinyxml2.h>

#include <OvCore/ECS/Actor.h>

#include <OvEditor/Utils/PrefabOperations.h>

namespace
{
	void SerializeActorHierarchy(
		OvCore::ECS::Actor& p_actor,
		tinyxml2::XMLDocument& p_doc,
		tinyxml2::XMLNode& p_actorsRoot)
	{
		p_actor.OnSerialize(p_doc, &p_actorsRoot);

		for (auto* child : p_actor.GetChildren())
		{
			SerializeActorHierarchy(*child, p_doc, p_actorsRoot);
		}
	}
}

bool OvEditor::Utils::PrefabOperations::SaveToFile(OvCore::ECS::Actor& p_rootActor, const std::filesystem::path& p_outputPath)
{
	tinyxml2::XMLDocument doc;

	auto* rootNode = doc.NewElement("root");
	doc.InsertFirstChild(rootNode);

	auto* prefabNode = doc.NewElement("prefab");
	rootNode->InsertEndChild(prefabNode);

	auto* actorsNode = doc.NewElement("actors");
	prefabNode->InsertEndChild(actorsNode);

	SerializeActorHierarchy(p_rootActor, doc, *actorsNode);

	return doc.SaveFile(p_outputPath.string().c_str()) == tinyxml2::XML_SUCCESS;
}

OvCore::ECS::Actor* OvEditor::Utils::PrefabOperations::InstantiateFromFile(
	const std::filesystem::path& p_prefabPath,
	const std::function<OvCore::ECS::Actor&(void)>& p_createActor)
{
	if (!p_createActor)
	{
		return nullptr;
	}

	tinyxml2::XMLDocument doc;
	doc.LoadFile(p_prefabPath.string().c_str());

	if (doc.Error())
	{
		return nullptr;
	}

	auto* rootNode = doc.FirstChildElement("root");
	auto* prefabNode = rootNode ? rootNode->FirstChildElement("prefab") : nullptr;
	auto* actorsNode = prefabNode ? prefabNode->FirstChildElement("actors") : nullptr;

	if (!actorsNode)
	{
		return nullptr;
	}

	struct PendingAttachment
	{
		OvCore::ECS::Actor* actor = nullptr;
		int64_t sourceParentID = 0;
	};

	std::vector<PendingAttachment> pendingAttachments;
	std::unordered_map<int64_t, OvCore::ECS::Actor*> sourceToInstance;

	for (auto* currentActor = actorsNode->FirstChildElement("actor");
		currentActor;
		currentActor = currentActor->NextSiblingElement("actor"))
	{
		auto& newActor = p_createActor();
		const int64_t generatedID = newActor.GetID();
		const uint64_t generatedGUID = newActor.GetGUID();

		newActor.OnDeserialize(doc, currentActor);

		pendingAttachments.push_back({
			&newActor,
			newActor.GetParentID()
		});

		sourceToInstance[newActor.GetID()] = &newActor;

		newActor.SetID(generatedID);
		newActor.SetGUID(generatedGUID);
	}

	OvCore::ECS::Actor* instantiatedRoot = nullptr;

	for (auto& pending : pendingAttachments)
	{
		if (auto found = sourceToInstance.find(pending.sourceParentID); found != sourceToInstance.end())
		{
			pending.actor->SetParent(*found->second);
		}
		else if (!instantiatedRoot)
		{
			instantiatedRoot = pending.actor;
		}
	}

	return instantiatedRoot;
}
