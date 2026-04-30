/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include "OvCore/ECS/Components/CSkinnedMeshRenderer.h"
#include <OvCore/ECS/Components/CAmbientBoxLight.h>
#include <OvCore/ECS/Components/CAmbientSphereLight.h>
#include <OvCore/ECS/Components/CAudioSource.h>
#include <OvCore/ECS/Components/CAudioListener.h>
#include <OvCore/ECS/Components/CDirectionalLight.h>
#include <OvCore/ECS/Components/CCamera.h>
#include <OvCore/ECS/Components/CMaterialRenderer.h>
#include <OvCore/ECS/Components/CPhysicalBox.h>
#include <OvCore/ECS/Components/CPhysicalCapsule.h>
#include <OvCore/ECS/Components/CPhysicalSphere.h>
#include <OvCore/ECS/Components/CPointLight.h>
#include <OvCore/ECS/Components/CPostProcessStack.h>
#include <OvCore/ECS/Components/CReflectionProbe.h>
#include <OvCore/ECS/Components/CSpotLight.h>
#include <OvCore/Helpers/GUIHelpers.h>

#include <OvEditor/Core/EditorActions.h>
#include <OvEditor/Utils/ActorCreationMenu.h>

#include <algorithm>

#include <OvUI/Widgets/Menu/MenuItem.h>
#include <OvUI/Widgets/Menu/MenuList.h>

namespace
{
	OvCore::ECS::Actor* ResolveAliveParent(OvCore::ECS::Actor* p_parent)
	{
		if (!p_parent)
		{
			return nullptr;
		}

		auto* currentScene = EDITOR_CONTEXT(sceneManager).GetCurrentScene();
		if (!currentScene)
		{
			return nullptr;
		}

		const auto& actors = currentScene->GetActors();
		const bool parentIsStillInScene = std::find(actors.begin(), actors.end(), p_parent) != actors.end();
		return parentIsStillInScene ? p_parent : nullptr;
	}

	void CreateSkysphere(OvCore::ECS::Actor* p_parent)
	{
		auto& instance = EDITOR_EXEC(CreateEmptyActor(false, p_parent));

		auto& materialRenderer = instance.AddComponent<OvCore::ECS::Components::CMaterialRenderer>();
		auto& modelRenderer = instance.AddComponent<OvCore::ECS::Components::CModelRenderer>();
		modelRenderer.SetFrustumBehaviour(OvCore::ECS::Components::CModelRenderer::EFrustumBehaviour::DISABLED);

		auto skysphereMaterial = EDITOR_CONTEXT(materialManager).GetResource(":Materials\\Skysphere.ovmat");
		auto sphereModel = EDITOR_CONTEXT(modelManager).GetResource(":Models\\Sphere.fbx");

		if (skysphereMaterial)
		{
			materialRenderer.SetMaterialAtIndex(0, *skysphereMaterial);
		}

		if (sphereModel)
		{
			modelRenderer.SetModel(sphereModel);
		}

		instance.SetName("Skysphere");

		EDITOR_EXEC(SelectActor(instance));
	}

	void CreateAtmosphere(OvCore::ECS::Actor* p_parent)
	{
		auto& instance = EDITOR_EXEC(CreateEmptyActor(false, p_parent));

		auto& materialRenderer = instance.AddComponent<OvCore::ECS::Components::CMaterialRenderer>();
		auto& modelRenderer = instance.AddComponent<OvCore::ECS::Components::CModelRenderer>();
		modelRenderer.SetFrustumBehaviour(OvCore::ECS::Components::CModelRenderer::EFrustumBehaviour::DISABLED);

		auto atmosphereMaterial = EDITOR_CONTEXT(materialManager).GetResource(":Materials\\Atmosphere.ovmat");
		auto sphereModel = EDITOR_CONTEXT(modelManager).GetResource(":Models\\Sphere.fbx");

		if (atmosphereMaterial)
		{
			materialRenderer.SetMaterialAtIndex(0, *atmosphereMaterial);
		}

		if (sphereModel)
		{
			modelRenderer.SetModel(sphereModel);
		}

		instance.SetName("Atmosphere");

		EDITOR_EXEC(SelectActor(instance));
	}

	void CreateCharacter(OvCore::ECS::Actor* p_parent)
	{
		auto& instance = EDITOR_EXEC(CreateEmptyActor(false, p_parent));

		auto& materialRenderer = instance.AddComponent<OvCore::ECS::Components::CMaterialRenderer>();
		auto& modelRenderer = instance.AddComponent<OvCore::ECS::Components::CModelRenderer>();
		auto& skinnedMeshRenderer = instance.AddComponent<OvCore::ECS::Components::CSkinnedMeshRenderer>();

		auto defaultMaterial = EDITOR_CONTEXT(materialManager).GetResource(":Materials/Default.ovmat");
		auto mannequin = EDITOR_CONTEXT(modelManager).GetResource(":Models/Mannequin.fbx");

		if (mannequin)
		{
			modelRenderer.SetModel(mannequin);
			materialRenderer.FillWithEmbeddedMaterials(true, defaultMaterial);
		}

		skinnedMeshRenderer.SetAnimation("root|idle");

		instance.SetName("Character");

		EDITOR_EXEC(SelectActor(instance));
	}


	template<class T>
	std::function<void()> ActorWithComponentCreationHandler(OvCore::ECS::Actor* p_parent, std::optional<std::function<void()>> p_onItemClicked)
	{
		return [p_parent, p_onItemClicked]()
		{
			EDITOR_EXEC(CreateMonoComponentActor<T>(true, ResolveAliveParent(p_parent)));

			if (p_onItemClicked.has_value())
			{
				p_onItemClicked.value()();
			}
		};
	}

	std::function<void()> ActorWithModelComponentCreationHandler(OvCore::ECS::Actor* p_parent, const std::string& p_modelName, std::optional<std::function<void()>> p_onItemClicked)
	{
		return [p_parent, p_modelName, p_onItemClicked]()
		{
			EDITOR_EXEC(CreateActorWithModel(":Models\\" + p_modelName + ".fbx", true, ResolveAliveParent(p_parent), p_modelName));

			if (p_onItemClicked.has_value())
			{
				p_onItemClicked.value()();
			}
		};
	}

	std::function<void()> CreateSkysphereHandler(OvCore::ECS::Actor* p_parent, std::optional<std::function<void()>> p_onItemClicked)
	{
		return [p_parent, p_onItemClicked]()
		{
			CreateSkysphere(ResolveAliveParent(p_parent));

			if (p_onItemClicked.has_value())
			{
				p_onItemClicked.value()();
			}
		};
	}

	std::function<void()> CreateAtmosphereHandler(OvCore::ECS::Actor* p_parent, std::optional<std::function<void()>> p_onItemClicked)
	{
		return [p_parent, p_onItemClicked]()
		{
			CreateAtmosphere(ResolveAliveParent(p_parent));

			if (p_onItemClicked.has_value())
			{
				p_onItemClicked.value()();
			}
		};
	}

	std::function<void()> CreateCharacterHandler(OvCore::ECS::Actor* p_parent, std::optional<std::function<void()>> p_onItemClicked)
	{
		return [p_parent, p_onItemClicked]()
		{
			CreateCharacter(ResolveAliveParent(p_parent));

			if (p_onItemClicked.has_value())
			{
				p_onItemClicked.value()();
			}
		};
	}

	std::function<void()> CreateFromPrefabHandler(OvCore::ECS::Actor* p_parent, std::optional<std::function<void()>> p_onItemClicked)
	{
		return [p_parent, p_onItemClicked]()
		{
			OvCore::Helpers::GUIHelpers::OpenAssetPicker(
				OvTools::Utils::PathParser::EFileType::PREFAB,
				[p_parent, p_onItemClicked](std::string p_prefabPath)
				{
					if (p_prefabPath.empty())
					{
						return;
					}

					if (auto* actor = EDITOR_EXEC(InstantiatePrefab(p_prefabPath)); actor)
					{
						if (auto* parent = ResolveAliveParent(p_parent); parent)
						{
							actor->SetParent(*parent);
						}

						EDITOR_EXEC(SelectActor(*actor));

						if (p_onItemClicked.has_value())
						{
							p_onItemClicked.value()();
						}
					}
				},
				true,
				false
			);
		};
	}
}

void OvEditor::Utils::ActorCreationMenu::GenerateActorCreationMenu(OvUI::Widgets::Menu::MenuList& p_menuList, OvCore::ECS::Actor* p_parent, std::optional<std::function<void()>> p_onItemClicked)
{
	using namespace OvUI::Widgets::Menu;
	using namespace OvCore::ECS::Components;

	p_menuList.CreateWidget<MenuItem>("Create Empty").ClickedEvent += [p_parent, p_onItemClicked]()
	{
		EDITOR_EXEC(CreateEmptyActor(true, ResolveAliveParent(p_parent), ""));

		if (p_onItemClicked.has_value())
		{
			p_onItemClicked.value()();
		}
	};
	p_menuList.CreateWidget<MenuItem>("From prefab...").ClickedEvent += CreateFromPrefabHandler(p_parent, p_onItemClicked);

	auto& primitives = p_menuList.CreateWidget<MenuList>("Primitives");
	auto& physicals = p_menuList.CreateWidget<MenuList>("Physicals");
	auto& lights = p_menuList.CreateWidget<MenuList>("Lights");
	auto& audio = p_menuList.CreateWidget<MenuList>("Audio");
	auto& others = p_menuList.CreateWidget<MenuList>("Others");

	primitives.CreateWidget<MenuItem>("Cube").ClickedEvent += ActorWithModelComponentCreationHandler(p_parent, "Cube", p_onItemClicked);
	primitives.CreateWidget<MenuItem>("Sphere").ClickedEvent += ActorWithModelComponentCreationHandler(p_parent, "Sphere", p_onItemClicked);
	primitives.CreateWidget<MenuItem>("Cone").ClickedEvent += ActorWithModelComponentCreationHandler(p_parent, "Cone", p_onItemClicked);
	primitives.CreateWidget<MenuItem>("Cylinder").ClickedEvent += ActorWithModelComponentCreationHandler(p_parent, "Cylinder", p_onItemClicked);
	primitives.CreateWidget<MenuItem>("Plane").ClickedEvent += ActorWithModelComponentCreationHandler(p_parent, "Plane", p_onItemClicked);
	primitives.CreateWidget<MenuItem>("Gear").ClickedEvent += ActorWithModelComponentCreationHandler(p_parent, "Gear", p_onItemClicked);
	primitives.CreateWidget<MenuItem>("Helix").ClickedEvent += ActorWithModelComponentCreationHandler(p_parent, "Helix", p_onItemClicked);
	primitives.CreateWidget<MenuItem>("Pipe").ClickedEvent += ActorWithModelComponentCreationHandler(p_parent, "Pipe", p_onItemClicked);
	primitives.CreateWidget<MenuItem>("Pyramid").ClickedEvent += ActorWithModelComponentCreationHandler(p_parent, "Pyramid", p_onItemClicked);
	primitives.CreateWidget<MenuItem>("Torus").ClickedEvent += ActorWithModelComponentCreationHandler(p_parent, "Torus", p_onItemClicked);
	physicals.CreateWidget<MenuItem>("Physical Box").ClickedEvent += ActorWithComponentCreationHandler<CPhysicalBox>(p_parent, p_onItemClicked);
	physicals.CreateWidget<MenuItem>("Physical Sphere").ClickedEvent += ActorWithComponentCreationHandler<CPhysicalSphere>(p_parent, p_onItemClicked);
	physicals.CreateWidget<MenuItem>("Physical Capsule").ClickedEvent += ActorWithComponentCreationHandler<CPhysicalCapsule>(p_parent, p_onItemClicked);
	lights.CreateWidget<MenuItem>("Point").ClickedEvent += ActorWithComponentCreationHandler<CPointLight>(p_parent, p_onItemClicked);
	lights.CreateWidget<MenuItem>("Directional").ClickedEvent += ActorWithComponentCreationHandler<CDirectionalLight>(p_parent, p_onItemClicked);
	lights.CreateWidget<MenuItem>("Spot").ClickedEvent += ActorWithComponentCreationHandler<CSpotLight>(p_parent, p_onItemClicked);
	lights.CreateWidget<MenuItem>("Ambient Box").ClickedEvent += ActorWithComponentCreationHandler<CAmbientBoxLight>(p_parent, p_onItemClicked);
	lights.CreateWidget<MenuItem>("Ambient Sphere").ClickedEvent += ActorWithComponentCreationHandler<CAmbientSphereLight>(p_parent, p_onItemClicked);
	audio.CreateWidget<MenuItem>("Audio Source").ClickedEvent += ActorWithComponentCreationHandler<CAudioSource>(p_parent, p_onItemClicked);
	audio.CreateWidget<MenuItem>("Audio Listener").ClickedEvent += ActorWithComponentCreationHandler<CAudioListener>(p_parent, p_onItemClicked);
	others.CreateWidget<MenuItem>("Camera").ClickedEvent += ActorWithComponentCreationHandler<CCamera>(p_parent, p_onItemClicked);
	others.CreateWidget<MenuItem>("Post Process Stack").ClickedEvent += ActorWithComponentCreationHandler<CPostProcessStack>(p_parent, p_onItemClicked);
	others.CreateWidget<MenuItem>("Reflection Probe").ClickedEvent += ActorWithComponentCreationHandler<CReflectionProbe>(p_parent, p_onItemClicked);
	others.CreateWidget<MenuItem>("Skysphere").ClickedEvent += CreateSkysphereHandler(p_parent, p_onItemClicked);
	others.CreateWidget<MenuItem>("Atmosphere").ClickedEvent += CreateAtmosphereHandler(p_parent, p_onItemClicked);
	others.CreateWidget<MenuItem>("Character").ClickedEvent += CreateCharacterHandler(p_parent, p_onItemClicked);
}
