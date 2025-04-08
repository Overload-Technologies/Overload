/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include <algorithm>
#include <string>

#include <OvCore/ECS/Components/CAmbientSphereLight.h>
#include <OvCore/ECS/Components/CDirectionalLight.h>
#include <OvCore/SceneSystem/Scene.h>

#include <OvTools/Profiling/CPUProfiling.h>

OvCore::SceneSystem::Scene::Scene()
{

}

OvCore::SceneSystem::Scene::~Scene()
{
	std::for_each(m_actors.begin(), m_actors.end(), [](OvCore::ECS::Actor* element)
	{ 
		delete element;
	});

	m_actors.clear();
}

void OvCore::SceneSystem::Scene::AddDefaultCamera()
{
	auto& camera = CreateActor("Main Camera");
	camera.AddComponent<ECS::Components::CCamera>();
	camera.transform.SetLocalPosition({ 0.0f, 3.0f, 8.0f });
	camera.transform.SetLocalRotation(OvMaths::FQuaternion({ 20.0f, 180.0f, 0.0f }));
}

void OvCore::SceneSystem::Scene::AddDefaultLights()
{
	auto& directionalLight = CreateActor("Directional Light");
	directionalLight.AddComponent<ECS::Components::CDirectionalLight>().SetIntensity(0.75f);
	directionalLight.transform.SetLocalPosition({ 0.0f, 10.0f, 0.0f });
	directionalLight.transform.SetLocalRotation(OvMaths::FQuaternion({ 120.0f, -40.0f, 0.0f }));

	auto& ambientLight = CreateActor("Ambient Light");
	ambientLight.AddComponent<ECS::Components::CAmbientSphereLight>().SetRadius(10000.0f);
}

void OvCore::SceneSystem::Scene::Play()
{
	m_isPlaying = true;

	/* Wake up actors to allow them to react to OnEnable, OnDisable and OnDestroy, */
	std::for_each(m_actors.begin(), m_actors.end(), [](ECS::Actor * p_element) { p_element->SetSleeping(false); });

	std::for_each(m_actors.begin(), m_actors.end(), [](ECS::Actor * p_element) { if (p_element->IsActive()) p_element->OnAwake(); });
	std::for_each(m_actors.begin(), m_actors.end(), [](ECS::Actor * p_element) { if (p_element->IsActive()) p_element->OnEnable(); });
	std::for_each(m_actors.begin(), m_actors.end(), [](ECS::Actor * p_element) { if (p_element->IsActive()) p_element->OnStart(); });
}

bool OvCore::SceneSystem::Scene::IsPlaying() const
{
	return m_isPlaying;
}

void OvCore::SceneSystem::Scene::Update(float p_deltaTime)
{
	CPUZone;
	auto actors = m_actors;
	std::for_each(actors.begin(), actors.end(), std::bind(std::mem_fn(&ECS::Actor::OnUpdate), std::placeholders::_1, p_deltaTime));
}

void OvCore::SceneSystem::Scene::FixedUpdate(float p_deltaTime)
{
	CPUZone;
	auto actors = m_actors;
	std::for_each(actors.begin(), actors.end(), std::bind(std::mem_fn(&ECS::Actor::OnFixedUpdate), std::placeholders::_1, p_deltaTime));
}

void OvCore::SceneSystem::Scene::LateUpdate(float p_deltaTime)
{
	CPUZone;
	auto actors = m_actors;
	std::for_each(actors.begin(), actors.end(), std::bind(std::mem_fn(&ECS::Actor::OnLateUpdate), std::placeholders::_1, p_deltaTime));
}

OvCore::ECS::Actor& OvCore::SceneSystem::Scene::CreateActor()
{
	return CreateActor("New Actor");
}

OvCore::ECS::Actor& OvCore::SceneSystem::Scene::CreateActor(const std::string& p_name, const std::string& p_tag)
{
	m_actors.push_back(new OvCore::ECS::Actor(m_availableID++, p_name, p_tag, m_isPlaying));
	ECS::Actor& instance = *m_actors.back();
	instance.ComponentAddedEvent	+= std::bind(&Scene::OnComponentAdded, this, std::placeholders::_1);
	instance.ComponentRemovedEvent	+= std::bind(&Scene::OnComponentRemoved, this, std::placeholders::_1);
	if (m_isPlaying)
	{
		instance.SetSleeping(false);
		if (instance.IsActive())
		{
			instance.OnAwake();
			instance.OnEnable();
			instance.OnStart();
		}
	}
	return instance;
}

bool OvCore::SceneSystem::Scene::DestroyActor(ECS::Actor& p_target)
{
	auto found = std::find_if(m_actors.begin(), m_actors.end(), [&p_target](OvCore::ECS::Actor* element)
	{
		return element == &p_target;
	});

	if (found != m_actors.end())
	{
		delete *found;
		m_actors.erase(found);
		return true;
	}
	else
	{
		return false;
	}
}

void OvCore::SceneSystem::Scene::CollectGarbages()
{
	m_actors.erase(std::remove_if(m_actors.begin(), m_actors.end(), [this](ECS::Actor* element)
	{ 
		bool isGarbage = !element->IsAlive();
		if (isGarbage)
		{
			delete element;
		}
		return isGarbage;
	}), m_actors.end());
}

OvCore::ECS::Actor* OvCore::SceneSystem::Scene::FindActorByName(const std::string& p_name) const
{
	auto result = std::find_if(m_actors.begin(), m_actors.end(), [p_name](OvCore::ECS::Actor* element)
	{ 
		return element->GetName() == p_name;
	});

	if (result != m_actors.end())
		return *result;
	else
		return nullptr;
}

OvCore::ECS::Actor* OvCore::SceneSystem::Scene::FindActorByTag(const std::string & p_tag) const
{
	auto result = std::find_if(m_actors.begin(), m_actors.end(), [p_tag](OvCore::ECS::Actor* element)
	{
		return element->GetTag() == p_tag;
	});

	if (result != m_actors.end())
		return *result;
	else
		return nullptr;
}

OvCore::ECS::Actor* OvCore::SceneSystem::Scene::FindActorByID(int64_t p_id) const
{
	auto result = std::find_if(m_actors.begin(), m_actors.end(), [p_id](OvCore::ECS::Actor* element)
	{
		return element->GetID() == p_id;
	});

	if (result != m_actors.end())
		return *result;
	else
		return nullptr;
}

std::vector<std::reference_wrapper<OvCore::ECS::Actor>> OvCore::SceneSystem::Scene::FindActorsByName(const std::string & p_name) const
{
	std::vector<std::reference_wrapper<OvCore::ECS::Actor>> actors;

	for (auto actor : m_actors)
	{
		if (actor->GetName() == p_name)
			actors.push_back(std::ref(*actor));
	}

	return actors;
}

std::vector<std::reference_wrapper<OvCore::ECS::Actor>> OvCore::SceneSystem::Scene::FindActorsByTag(const std::string & p_tag) const
{
	std::vector<std::reference_wrapper<OvCore::ECS::Actor>> actors;

	for (auto actor : m_actors)
	{
		if (actor->GetTag() == p_tag)
			actors.push_back(std::ref(*actor));
	}

	return actors;
}

OvCore::ECS::Components::CCamera* OvCore::SceneSystem::Scene::FindMainCamera() const
{
	for (OvCore::ECS::Components::CCamera* camera : m_fastAccessComponents.cameras)
	{
		if (camera->owner.IsActive())
		{
			return camera;
		}
	}

	return nullptr;
}

void OvCore::SceneSystem::Scene::OnComponentAdded(ECS::Components::AComponent& p_compononent)
{
	if (auto result = dynamic_cast<ECS::Components::CModelRenderer*>(&p_compononent))
		m_fastAccessComponents.modelRenderers.push_back(result);

	if (auto result = dynamic_cast<ECS::Components::CCamera*>(&p_compononent))
		m_fastAccessComponents.cameras.push_back(result);

	if (auto result = dynamic_cast<ECS::Components::CLight*>(&p_compononent))
		m_fastAccessComponents.lights.push_back(result);

	if (auto result = dynamic_cast<ECS::Components::CPostProcessStack*>(&p_compononent))
		m_fastAccessComponents.postProcessStacks.push_back(result);
}

void OvCore::SceneSystem::Scene::OnComponentRemoved(ECS::Components::AComponent& p_compononent)
{
	if (auto result = dynamic_cast<ECS::Components::CModelRenderer*>(&p_compononent))
		m_fastAccessComponents.modelRenderers.erase(std::remove(m_fastAccessComponents.modelRenderers.begin(), m_fastAccessComponents.modelRenderers.end(), result), m_fastAccessComponents.modelRenderers.end());

	if (auto result = dynamic_cast<ECS::Components::CCamera*>(&p_compononent))
		m_fastAccessComponents.cameras.erase(std::remove(m_fastAccessComponents.cameras.begin(), m_fastAccessComponents.cameras.end(), result), m_fastAccessComponents.cameras.end());

	if (auto result = dynamic_cast<ECS::Components::CLight*>(&p_compononent))
		m_fastAccessComponents.lights.erase(std::remove(m_fastAccessComponents.lights.begin(), m_fastAccessComponents.lights.end(), result), m_fastAccessComponents.lights.end());

	if (auto result = dynamic_cast<ECS::Components::CPostProcessStack*>(&p_compononent))
		m_fastAccessComponents.postProcessStacks.erase(std::remove(m_fastAccessComponents.postProcessStacks.begin(), m_fastAccessComponents.postProcessStacks.end(), result), m_fastAccessComponents.postProcessStacks.end());
}

std::vector<OvCore::ECS::Actor*>& OvCore::SceneSystem::Scene::GetActors()
{
	return m_actors;
}

const OvCore::SceneSystem::Scene::FastAccessComponents& OvCore::SceneSystem::Scene::GetFastAccessComponents() const
{
	return m_fastAccessComponents;
}

void OvCore::SceneSystem::Scene::OnSerialize(tinyxml2::XMLDocument & p_doc, tinyxml2::XMLNode * p_root)
{
	tinyxml2::XMLNode* sceneNode = p_doc.NewElement("scene");
	p_root->InsertEndChild(sceneNode);

	tinyxml2::XMLNode* actorsNode = p_doc.NewElement("actors");
	sceneNode->InsertEndChild(actorsNode);

	for (auto& actor : m_actors)
	{
		actor->OnSerialize(p_doc, actorsNode);
	}
}

void OvCore::SceneSystem::Scene::OnDeserialize(tinyxml2::XMLDocument & p_doc, tinyxml2::XMLNode * p_root)
{
	tinyxml2::XMLNode* actorsRoot = p_root->FirstChildElement("actors");

	if (actorsRoot)
	{
		tinyxml2::XMLElement* currentActor = actorsRoot->FirstChildElement("actor");

		int64_t maxID = 1;

		while (currentActor)
		{
			auto& actor = CreateActor();
			actor.OnDeserialize(p_doc, currentActor);
			maxID = std::max(actor.GetID() + 1, maxID);
			currentActor = currentActor->NextSiblingElement("actor");
		}

		m_availableID = maxID;

		/* We recreate the hierarchy of the scene by attaching children to their parents */
		for (auto actor : m_actors)
		{
			if (actor->GetParentID() > 0)
			{
				if (auto found = FindActorByID(actor->GetParentID()); found)
					actor->SetParent(*found);
			}
		}
	}
}
