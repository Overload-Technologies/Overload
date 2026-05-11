/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include "OvEditor/Panels/Hierarchy.h"
#include "OvEditor/Core/EditorActions.h"

#include <algorithm>
#include <cstdint>

#include <OvUI/Styling/Style.h>
#include <OvUI/Widgets/Buttons/Button.h>
#include <OvUI/Widgets/Selection/CheckBox.h>
#include <OvUI/Widgets/Visual/Separator.h>
#include <OvUI/Plugins/DDSource.h>
#include <OvUI/Plugins/DDTarget.h>

#include <OvDebug/Logger.h>

#include <OvCore/Global/ServiceLocator.h>
#include <OvCore/Helpers/GUIHelpers.h>
#include <OvCore/Helpers/GUIDrawer.h>

#include <OvCore/ECS/Components/CCamera.h>
#include <OvCore/ECS/Components/CPointLight.h>
#include <OvCore/ECS/Components/CDirectionalLight.h>
#include <OvCore/ECS/Components/CSpotLight.h>
#include <OvCore/ECS/Components/CAmbientBoxLight.h>
#include <OvCore/ECS/Components/CAmbientSphereLight.h>
#include <OvCore/ECS/Components/CPhysicalBox.h>
#include <OvCore/ECS/Components/CPhysicalSphere.h>
#include <OvCore/ECS/Components/CPhysicalCapsule.h>
#include <OvCore/ECS/Components/CAudioSource.h>
#include <OvCore/ECS/Components/CAudioListener.h>

#include <OvUI/Plugins/ContextualMenu.h>
#include <OvWindowing/Dialogs/MessageBox.h>
#include <OvWindowing/Dialogs/SaveFileDialog.h>

#include <OvTools/Utils/PathParser.h>

#include "OvEditor/Core/EditorResources.h"
#include "OvEditor/Utils/ActorCreationMenu.h"

namespace
{
	const OvCore::ECS::Actor* GetPrefabInstanceRoot(const OvCore::ECS::Actor& p_actor)
	{
		const OvCore::ECS::Actor* current = &p_actor;

		while (current)
		{
			if (current->HasPrefabSource())
			{
				return current;
			}

			current = current->GetParent();
		}

		return nullptr;
	}

	bool IsPartOfPrefabInstance(const OvCore::ECS::Actor& p_actor)
	{
		return GetPrefabInstanceRoot(p_actor) != nullptr;
	}
}

class ActorContextualMenu : public OvUI::Plugins::ContextualMenu
{
public:
	ActorContextualMenu(OvCore::ECS::Actor* p_target, OvUI::Widgets::Layout::TreeNode* p_treeNode = nullptr, bool p_panelMenu = false) :
		m_targetActor(p_target),
		m_treeNode(p_treeNode)
	{
		using namespace OvUI::Panels;
		using namespace OvUI::Widgets;
		using namespace OvUI::Widgets::Menu;
		using namespace OvCore::ECS::Components;

		if (m_targetActor)
		{
			auto& focusButton = CreateWidget<OvUI::Widgets::Menu::MenuItem>("Focus");
			focusButton.ClickedEvent += [this]
			{
				if (auto* target = GetTargetActor())
				{
					EDITOR_EXEC(MoveToTarget(*target));
				}
			};

			auto& copyButton = CreateWidget<OvUI::Widgets::Menu::MenuItem>("Copy");
			copyButton.ClickedEvent += [this]
			{
				if (auto* target = GetTargetActor())
				{
					EDITOR_EXEC(CopyActor(*target));
				}
			};

			auto& duplicateButton = CreateWidget<OvUI::Widgets::Menu::MenuItem>("Duplicate");
			duplicateButton.ClickedEvent += [this]
			{
				if (auto* target = GetTargetActor())
				{
					EDITOR_EXEC(DelayAction(EDITOR_BIND(DuplicateActor, std::ref(*target), nullptr, true, true), 0));
				}
			};

			auto& pasteButton = CreateWidget<OvUI::Widgets::Menu::MenuItem>("Paste");
			pasteButton.ClickedEvent += [this]
			{
				if (auto* target = GetTargetActor())
				{
					EDITOR_EXEC(DelayAction(EDITOR_BIND(PasteActor, target), 0));
				}
			};
			
			auto& saveAsPrefabButton = CreateWidget<OvUI::Widgets::Menu::MenuItem>("Save as Prefab...");
			saveAsPrefabButton.ClickedEvent += [this]
			{
				auto* target = GetTargetActor();
				if (!target)
				{
					return;
				}

				OvWindowing::Dialogs::SaveFileDialog dialog("Save Prefab");
				dialog.SetInitialDirectory(EDITOR_CONTEXT(projectAssetsPath).string());
				dialog.SetInitialFilename(target->GetName());
				dialog.DefineExtension("Overload Prefab", ".ovprefab");
				dialog.Show();

				if (!dialog.HasSucceeded())
				{
					return;
				}

				if (dialog.IsFileExisting())
				{
					OvWindowing::Dialogs::MessageBox message(
						"File already exists!",
						"The file \"" + dialog.GetSelectedFileName() + "\" already exists.\n\nOverwriting this file will replace the previous prefab content.\n\nAre you ok with that?",
						OvWindowing::Dialogs::MessageBox::EMessageType::WARNING,
						OvWindowing::Dialogs::MessageBox::EButtonLayout::YES_NO,
						true
					);

					if (message.GetUserAction() != OvWindowing::Dialogs::MessageBox::EUserAction::YES)
					{
						return;
					}
				}

				EDITOR_EXEC(SaveActorAsPrefab(*target, dialog.GetSelectedFilePath()));
			};

			auto& applyToPrefabButton = CreateWidget<OvUI::Widgets::Menu::MenuItem>("Apply to Prefab");
			m_applyToPrefabButton = &applyToPrefabButton;
			applyToPrefabButton.ClickedEvent += [this]
			{
				if (auto* target = GetTargetActor())
				{
					EDITOR_EXEC(ApplyActorToPrefab(*target));
				}
			};

			auto& revertToPrefabButton = CreateWidget<OvUI::Widgets::Menu::MenuItem>("Revert to Prefab");
			m_revertToPrefabButton = &revertToPrefabButton;
			revertToPrefabButton.ClickedEvent += [this]
			{
				if (auto* target = GetTargetActor())
				{
					EDITOR_EXEC(RevertActorToPrefab(*target));
				}
			};

			auto& deleteButton = CreateWidget<OvUI::Widgets::Menu::MenuItem>("Delete");
			deleteButton.ClickedEvent += [this]
			{
				if (auto* target = GetTargetActor())
				{
					EDITOR_EXEC(DestroyActor(std::ref(*target)));
				}
			};

			auto& renameMenu = CreateWidget<OvUI::Widgets::Menu::MenuList>("Rename to...");

			auto& nameEditor = renameMenu.CreateWidget<OvUI::Widgets::InputFields::InputText>("");
			nameEditor.selectAllOnClick = true;

			renameMenu.ClickedEvent += [this, &nameEditor]
			{
				if (auto* target = GetTargetActor())
				{
					nameEditor.content = target->GetName();
				}
			};

			nameEditor.EnterPressedEvent += [this](std::string p_newName)
			{
				if (auto* target = GetTargetActor())
				{
					target->SetName(p_newName);
				}
			};
		}
		else
		{
			auto& pasteButton = CreateWidget<OvUI::Widgets::Menu::MenuItem>("Paste");
			pasteButton.ClickedEvent += [this]
			{
				EDITOR_EXEC(DelayAction(EDITOR_BIND(PasteActor, nullptr), 0));
			};
		}

		auto& createActor = CreateWidget<OvUI::Widgets::Menu::MenuList>("Create...");

		const auto onItemClicked =
			m_treeNode ?
			std::make_optional<std::function<void()>>(
				std::bind(&OvUI::Widgets::Layout::TreeNode::Open, m_treeNode)
			) :
			std::nullopt;

		OvEditor::Utils::ActorCreationMenu::GenerateActorCreationMenu(
			createActor,
			m_targetActor,
			onItemClicked
		);
	}

	virtual void Execute(OvUI::Plugins::EPluginExecutionContext p_context) override
	{
		if (m_applyToPrefabButton || m_revertToPrefabButton)
		{
			auto* target = GetTargetActor();
			const bool canEditPrefab = target && GetPrefabInstanceRoot(*target) != nullptr;

			if (m_applyToPrefabButton)
			{
				m_applyToPrefabButton->enabled = canEditPrefab;
			}

			if (m_revertToPrefabButton)
			{
				m_revertToPrefabButton->enabled = canEditPrefab;
			}
		}

		if (m_widgets.size() > 0)
			OvUI::Plugins::ContextualMenu::Execute(p_context);
	}

private:
	OvCore::ECS::Actor* GetTargetActor() const
	{
		if (!m_targetActor)
		{
			return nullptr;
		}

		auto* currentScene = EDITOR_CONTEXT(sceneManager).GetCurrentScene();
		if (!currentScene)
		{
			return nullptr;
		}

		const auto& actors = currentScene->GetActors();
		const bool actorIsStillInScene =
			std::find(actors.begin(), actors.end(), m_targetActor) != actors.end();

		return actorIsStillInScene ? m_targetActor : nullptr;
	}

	OvCore::ECS::Actor* m_targetActor = nullptr;
	OvUI::Widgets::Layout::TreeNode* m_treeNode;
	OvUI::Widgets::Menu::MenuItem* m_applyToPrefabButton = nullptr;
	OvUI::Widgets::Menu::MenuItem* m_revertToPrefabButton = nullptr;
};

void ExpandTreeNode(OvUI::Widgets::Layout::TreeNode& p_toExpand)
{
	p_toExpand.Open();

	if (p_toExpand.HasParent())
	{
		if (auto parent = dynamic_cast<OvUI::Widgets::Layout::TreeNode*>(p_toExpand.GetParent()); parent)
		{
			ExpandTreeNode(*parent);
		}
	}
}

std::vector<OvUI::Widgets::Layout::TreeNode*> nodesToCollapse;
std::vector<OvUI::Widgets::Layout::TreeNode*> founds;

void ExpandTreeNodeAndEnable(OvUI::Widgets::Layout::TreeNode& p_toExpand)
{
	if (!p_toExpand.IsOpened())
	{
		p_toExpand.Open();
		nodesToCollapse.push_back(&p_toExpand);
	}

	p_toExpand.enabled = true;

	if (p_toExpand.HasParent())
	{
		if (auto parent = dynamic_cast<OvUI::Widgets::Layout::TreeNode*>(p_toExpand.GetParent()); parent)
		{
			ExpandTreeNodeAndEnable(*parent);
		}
	}
}

OvEditor::Panels::Hierarchy::Hierarchy
(
	const std::string & p_title,
	bool p_opened,
	const OvUI::Settings::PanelWindowSettings& p_windowSettings
) :
	PanelWindow(p_title, p_opened, p_windowSettings),
	m_actions(CreateWidget<OvUI::Widgets::Layout::Group>()),
	m_actors(CreateWidget<OvUI::Widgets::Layout::Group>())
{
	const uint32_t searchIconID = []{
		if (auto* tex = EDITOR_CONTEXT(editorResources)->GetTexture("Search"))
			return tex->GetTexture().GetID();
		return 0u;
	}();

	auto& searchBar = OvCore::Helpers::GUIDrawer::DrawSearchBar(m_actions, searchIconID);
	searchBar.ContentChangedEvent += [this](const std::string& p_content)
	{
		founds.clear();
		auto content = p_content;
		std::transform(content.begin(), content.end(), content.begin(), ::tolower);

		for (auto& [actor, item] : m_widgetActorLink)
		{
			if (!p_content.empty())
			{
				auto itemName = item->name;
				std::transform(itemName.begin(), itemName.end(), itemName.begin(), ::tolower);

				if (itemName.find(content) != std::string::npos)
				{
					founds.push_back(item);
				}

				item->enabled = false;
			}
			else
			{
				item->enabled = true;
			}
		}

		for (auto node : founds)
		{
			node->enabled = true;

			if (node->HasParent())
			{
				if (auto parent = dynamic_cast<OvUI::Widgets::Layout::TreeNode*>(node->GetParent()); parent)
				{
					ExpandTreeNodeAndEnable(*parent);
				}
			}
		}

		if (p_content.empty())
		{
			for (auto node : nodesToCollapse)
			{
				node->Close();
			}

			nodesToCollapse.clear();
		}
	};

	auto& windowDDTarget = AddPlugin<OvUI::Plugins::DDTarget<std::pair<OvCore::ECS::Actor*, OvUI::Widgets::Layout::TreeNode*>>>("Actor");
	windowDDTarget.showYellowRect = false;
	windowDDTarget.DataReceivedEvent += [this](std::pair<OvCore::ECS::Actor*, OvUI::Widgets::Layout::TreeNode*> p_element)
	{
		if (p_element.second->HasParent())
		{
			p_element.second->GetParent()->UnconsiderWidget(*p_element.second);
		}

		ConsiderWidget(*p_element.second);

		p_element.first->DetachFromParent();
	};

	AddPlugin<ActorContextualMenu>(nullptr, nullptr);

	// TODO: This code is unsafe, if the hierarchy gets deleted before the last actor gets deleted, this might crash
	EDITOR_EVENT(ActorUnselectedEvent) += std::bind(&Hierarchy::UnselectActorsWidgets, this);
	EDITOR_CONTEXT(sceneManager).SceneUnloadEvent += std::bind(&Hierarchy::Clear, this);
	OvCore::ECS::Actor::CreatedEvent += std::bind(&Hierarchy::AddActorByInstance, this, std::placeholders::_1);
	OvCore::ECS::Actor::DestroyedEvent += std::bind(&Hierarchy::DeleteActorByInstance, this, std::placeholders::_1);
	EDITOR_EVENT(ActorSelectedEvent) += std::bind(&Hierarchy::SelectActorByInstance, this, std::placeholders::_1);
	OvCore::ECS::Actor::AttachEvent += std::bind(&Hierarchy::AttachActorToParent, this, std::placeholders::_1);
	OvCore::ECS::Actor::DettachEvent += std::bind(&Hierarchy::DetachFromParent, this, std::placeholders::_1);
}

void OvEditor::Panels::Hierarchy::Clear()
{
	EDITOR_EXEC(UnselectActor());

	m_actors.RemoveAllWidgets();
	m_widgetActorLink.clear();
}

void OvEditor::Panels::Hierarchy::UnselectActorsWidgets()
{
	for (auto& widget : m_widgetActorLink)
		widget.second->selected = false;
}

void OvEditor::Panels::Hierarchy::SelectActorByInstance(OvCore::ECS::Actor& p_actor)
{
	if (auto result = m_widgetActorLink.find(&p_actor); result != m_widgetActorLink.end())
		if (result->second)
			SelectActorByWidget(*result->second);
}

void OvEditor::Panels::Hierarchy::SelectActorByWidget(OvUI::Widgets::Layout::TreeNode & p_widget)
{
	UnselectActorsWidgets();

	p_widget.selected = true;

	if (p_widget.HasParent())
	{
		if (auto parent = dynamic_cast<OvUI::Widgets::Layout::TreeNode*>(p_widget.GetParent()); parent)
		{
			ExpandTreeNode(*parent);
		}
	}
}

void OvEditor::Panels::Hierarchy::AttachActorToParent(OvCore::ECS::Actor & p_actor)
{
	auto actorWidget = m_widgetActorLink.find(&p_actor);

	if (actorWidget != m_widgetActorLink.end())
	{
		auto widget = actorWidget->second;

		if (widget->HasParent())
		{
			widget->GetParent()->UnconsiderWidget(*widget);
		}

		if (p_actor.HasParent())
		{
			if (auto parentWidget = m_widgetActorLink.find(p_actor.GetParent()); parentWidget != m_widgetActorLink.end())
			{
				parentWidget->second->leaf = false;
				parentWidget->second->ConsiderWidget(*widget);
			}
		}
	}
}

void OvEditor::Panels::Hierarchy::DetachFromParent(OvCore::ECS::Actor & p_actor)
{
	if (auto actorWidget = m_widgetActorLink.find(&p_actor); actorWidget != m_widgetActorLink.end())
	{
		if (p_actor.HasParent() && p_actor.GetParent()->GetChildren().size() == 1)
		{
			if (auto parentWidget = m_widgetActorLink.find(p_actor.GetParent()); parentWidget != m_widgetActorLink.end())
			{
				parentWidget->second->leaf = true;
			}
		}

		auto widget = actorWidget->second;

		if (widget->HasParent())
		{
			widget->GetParent()->UnconsiderWidget(*widget);
		}

		ConsiderWidget(*widget);
	}
}

void OvEditor::Panels::Hierarchy::DeleteActorByInstance(OvCore::ECS::Actor& p_actor)
{
	if (auto result = m_widgetActorLink.find(&p_actor); result != m_widgetActorLink.end())
	{
		if (result->second)
		{
			result->second->Destroy();
		}

		if (p_actor.HasParent() && p_actor.GetParent()->GetChildren().size() == 1)
		{
			if (auto parentWidget = m_widgetActorLink.find(p_actor.GetParent()); parentWidget != m_widgetActorLink.end())
			{
				parentWidget->second->leaf = true;
			}
		}

		m_widgetActorLink.erase(result);
	}
}

void OvEditor::Panels::Hierarchy::AddActorByInstance(OvCore::ECS::Actor & p_actor)
{
	auto& textSelectable = m_actors.CreateWidget<OvUI::Widgets::Layout::TreeNode>(p_actor.GetName(), true);
	textSelectable.leaf = true;

	const uint32_t actorIconID = OvCore::Helpers::GUIHelpers::GetActorIconID();
	const uint32_t prefabIconID = OvCore::Helpers::GUIHelpers::GetIconForFileType(OvTools::Utils::PathParser::EFileType::PREFAB);

	textSelectable.iconTextureID = IsPartOfPrefabInstance(p_actor) && prefabIconID != 0 ? prefabIconID : actorIconID;

	textSelectable.AddPlugin<ActorContextualMenu>(&p_actor, &textSelectable);
	textSelectable.AddPlugin<OvUI::Plugins::DDSource<std::pair<OvCore::ECS::Actor*, OvUI::Widgets::Layout::TreeNode*>>>("Actor", "Attach to...", std::make_pair(&p_actor, &textSelectable));
	textSelectable.AddPlugin<OvUI::Plugins::DDTarget<std::pair<OvCore::ECS::Actor*, OvUI::Widgets::Layout::TreeNode*>>>("Actor").DataReceivedEvent += [&p_actor, &textSelectable](std::pair<OvCore::ECS::Actor*, OvUI::Widgets::Layout::TreeNode*> p_element)
	{
		if (&p_actor == p_element.first)
		{
			return;
		}

		if (p_actor.IsDescendantOf(p_element.first))
		{
			OVLOG_WARNING("Cannot attach \"" + p_element.first->GetName() + "\" to \"" + p_actor.GetName() + "\" because it is a descendant of the latter.");
			return;
		}

		p_element.first->SetParent(p_actor);
	};
	auto& dispatcher = textSelectable.AddPlugin<OvUI::Plugins::DataDispatcher<std::string>>();

	OvCore::ECS::Actor* targetPtr = &p_actor;
	dispatcher.RegisterGatherer([targetPtr, &textSelectable, actorIconID, prefabIconID]
	{
		const bool isActive = targetPtr->IsActive();
		textSelectable.labelColor = isActive
			? OVUI_STYLE(Text)
			: OVUI_STYLE(TextDisabled);

		textSelectable.iconTextureID = IsPartOfPrefabInstance(*targetPtr) && prefabIconID != 0 ? prefabIconID : actorIconID;

		return targetPtr->GetName();
	});

	m_widgetActorLink[targetPtr] = &textSelectable;

	textSelectable.ClickedEvent += EDITOR_BIND(SelectActor, std::ref(p_actor));
	textSelectable.DoubleClickedEvent += EDITOR_BIND(MoveToTarget, std::ref(p_actor));
}
