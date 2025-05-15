/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include <tinyxml2.h>
#include <OvWindowing/Dialogs/MessageBox.h>

#include "OvCore/SceneSystem/SceneManager.h"
#include "OvCore/ECS/Components/CDirectionalLight.h"
#include "OvCore/ECS/Components/CAmbientSphereLight.h"
#include "OvCore/ECS/Components/CCamera.h"

OvCore::SceneSystem::SceneManager::SceneManager(const std::string& p_sceneRootFolder) : m_sceneRootFolder(p_sceneRootFolder)
{
	LoadEmptyScene();
}

OvCore::SceneSystem::SceneManager::~SceneManager()
{
	UnloadCurrentScene();
}

void OvCore::SceneSystem::SceneManager::Update()
{
	if (m_delayedLoadCall)
	{
		m_delayedLoadCall();
		m_delayedLoadCall = 0;
	}
}

void OvCore::SceneSystem::SceneManager::LoadAndPlayDelayed(const std::string& p_path, bool p_absolute)
{
	m_delayedLoadCall = [this, p_path, p_absolute]
	{
		std::string previousSourcePath = GetCurrentSceneSourcePath();
		LoadScene(p_path, p_absolute);
		StoreCurrentSceneSourcePath(previousSourcePath);
		GetCurrentScene()->Play();
	};
}

void OvCore::SceneSystem::SceneManager::LoadEmptyScene()
{
	UnloadCurrentScene();
	m_currentScene.reset(new Scene());
	SceneLoadEvent.Invoke();
}

void OvCore::SceneSystem::SceneManager::LoadDefaultScene()
{
	UnloadCurrentScene();
	m_currentScene.reset(new Scene());
	m_currentScene->AddDefaultCamera();
	m_currentScene->AddDefaultLights();
	m_currentScene->AddDefaultAtmosphere();
	m_currentScene->AddDefaultPostProcessStack();
	SceneLoadEvent.Invoke();
}

bool OvCore::SceneSystem::SceneManager::LoadScene(const std::string& p_path, bool p_absolute)
{
	std::string completePath = (p_absolute ? "" : m_sceneRootFolder) + p_path;

	tinyxml2::XMLDocument doc;
	doc.LoadFile(completePath.c_str());

	if (LoadSceneFromMemory(doc))
	{
		StoreCurrentSceneSourcePath(completePath);
		return true;
	}

	return false;
}

bool OvCore::SceneSystem::SceneManager::LoadSceneFromMemory(tinyxml2::XMLDocument& p_doc)
{
	if (!p_doc.Error())
	{
		tinyxml2::XMLNode* root = p_doc.FirstChild();
		if (root)
		{
			tinyxml2::XMLNode* sceneNode = root->FirstChildElement("scene");
			if (sceneNode)
			{
				LoadEmptyScene();
				m_currentScene->OnDeserialize(p_doc, sceneNode);
				return true;
			}
		}
	}

	OvWindowing::Dialogs::MessageBox message("Scene loading failed", "The scene you are trying to load was not found or corrupted", OvWindowing::Dialogs::MessageBox::EMessageType::ERROR, OvWindowing::Dialogs::MessageBox::EButtonLayout::OK, true);
	return false;
}

void OvCore::SceneSystem::SceneManager::UnloadCurrentScene()
{
	if (m_currentScene)
	{
		m_currentScene.reset();
		SceneUnloadEvent.Invoke();
	}

	ForgetCurrentSceneSourcePath();
}

bool OvCore::SceneSystem::SceneManager::HasCurrentScene() const
{
	return m_currentScene != nullptr;
}

OvCore::SceneSystem::Scene* OvCore::SceneSystem::SceneManager::GetCurrentScene() const
{
	return m_currentScene.get();
}

std::string OvCore::SceneSystem::SceneManager::GetCurrentSceneSourcePath() const
{
	return m_currentSceneSourcePath;
}

bool OvCore::SceneSystem::SceneManager::IsCurrentSceneLoadedFromDisk() const
{
	return m_currentSceneLoadedFromPath;
}

void OvCore::SceneSystem::SceneManager::StoreCurrentSceneSourcePath(const std::string& p_path)
{
	m_currentSceneSourcePath = p_path;
	m_currentSceneLoadedFromPath = true;
	CurrentSceneSourcePathChangedEvent.Invoke(m_currentSceneSourcePath);
}

void OvCore::SceneSystem::SceneManager::ForgetCurrentSceneSourcePath()
{
	m_currentSceneSourcePath = "";
	m_currentSceneLoadedFromPath = false;
	CurrentSceneSourcePathChangedEvent.Invoke(m_currentSceneSourcePath);
}
