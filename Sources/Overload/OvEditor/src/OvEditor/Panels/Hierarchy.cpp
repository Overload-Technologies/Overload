/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include "OvEditor/Panels/Hierarchy.h"
#include "OvEditor/Core/EditorActions.h"

#include <OvUI/Widgets/Buttons/Button.h>
#include <OvUI/Widgets/Selection/CheckBox.h>
#include <OvUI/Widgets/Visual/Separator.h>
#include <OvUI/Plugins/DDSource.h>
#include <OvUI/Plugins/DDTarget.h>

#include <OvDebug/Logger.h>

#include <OvCore/Global/ServiceLocator.h>

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

#include "OvEditor/Utils/ActorCreationMenu.h"

class ActorContextualMenu : public OvUI::Plugins::ContextualMenu
{
public:
	ActorContextualMenu(OvCore::ECS::Actor* p_target, OvUI::Widgets::Layout::TreeNode* p_treeNode = nullptr, bool p_panelMenu = false) :
		m_target(p_target),
		m_treeNode(p_treeNode)
	{
		using namespace OvUI::Panels;
		using namespace OvUI::Widgets;
		using namespace OvUI::Widgets::Menu;
		using namespace OvCore::ECS::Components;

		if (m_target)
		{
			auto& focusButton = CreateWidget<OvUI::Widgets::Menu::MenuItem>("Focus");
			focusButton.ClickedEvent += [this]
			{
				EDITOR_EXEC(MoveToTarget(*m_target));
			};

			auto& duplicateButton = CreateWidget<OvUI::Widgets::Menu::MenuItem>("Duplicate");
			duplicateButton.ClickedEvent += [this]
			{
				EDITOR_EXEC(DelayAction(EDITOR_BIND(DuplicateActor, std::ref(*m_target), nullptr, true), 0));
			};

			auto& deleteButton = CreateWidget<OvUI::Widgets::Menu::MenuItem>("Delete");
			deleteButton.ClickedEvent += [this]
			{
				EDITOR_EXEC(DestroyActor(std::ref(*m_target)));
			};

			auto& renameMenu = CreateWidget<OvUI::Widgets::Menu::MenuList>("Rename to...");

			auto& nameEditor = renameMenu.CreateWidget<OvUI::Widgets::InputFields::InputText>("");
			nameEditor.selectAllOnClick = true;

			renameMenu.ClickedEvent += [this, &nameEditor]
			{
				nameEditor.content = m_target->GetName();
			};

			nameEditor.EnterPressedEvent += [this](std::string p_newName)
			{
				m_target->SetName(p_newName);
			};
		}

		auto& createActor = CreateWidget<OvUI::Widgets::Menu::MenuList>("Create...");

		const auto onItemClicked =
			m_treeNode ?
			std::make_optional<std::function<void()>>(
				std::bind(&OvUI::Widgets::Layout::TreeNode::Open, m_treeNode)
			) :
			std::nullopt;

		OvEditor::Utils::ActorCreationMenu::GenerateActorCreationMenu(createActor, m_target, onItemClicked);
	}

	virtual void Execute(OvUI::Plugins::EPluginExecutionContext p_context) override
	{
		if (m_widgets.size() > 0)
			OvUI::Plugins::ContextualMenu::Execute(p_context);
	}

private:
	OvCore::ECS::Actor* m_target;
	OvUI::Widgets::Layout::TreeNode* m_treeNode;
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
) : PanelWindow(p_title, p_opened, p_windowSettings)
{
	auto& searchBar = CreateWidget<OvUI::Widgets::InputFields::InputText>();
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

	RemoveAllWidgets();
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
	auto& textSelectable = CreateWidget<OvUI::Widgets::Layout::TreeNode>(p_actor.GetName(), true);
	textSelectable.leaf = true;
	textSelectable.AddPlugin<ActorContextualMenu>(&p_actor, &textSelectable);
	textSelectable.AddPlugin<OvUI::Plugins::DDSource<std::pair<OvCore::ECS::Actor*, OvUI::Widgets::Layout::TreeNode*>>>("Actor", "Attach to...", std::make_pair(&p_actor, &textSelectable));
	textSelectable.AddPlugin<OvUI::Plugins::DDTarget<std::pair<OvCore::ECS::Actor*, OvUI::Widgets::Layout::TreeNode*>>>("Actor").DataReceivedEvent += [&p_actor, &textSelectable](std::pair<OvCore::ECS::Actor*, OvUI::Widgets::Layout::TreeNode*> p_element)
	{
		if (p_actor.IsDescendantOf(p_element.first))
		{
			OVLOG_WARNING("Cannot attach \"" + p_element.first->GetName() + "\" to \"" + p_actor.GetName() + "\" because it is a descendant of the latter.");
			return;
		}

		p_element.first->SetParent(p_actor);
	};
	auto& dispatcher = textSelectable.AddPlugin<OvUI::Plugins::DataDispatcher<std::string>>();

	OvCore::ECS::Actor* targetPtr = &p_actor;
	dispatcher.RegisterGatherer([targetPtr] { return targetPtr->GetName(); });

	m_widgetActorLink[targetPtr] = &textSelectable;

	textSelectable.ClickedEvent += EDITOR_BIND(SelectActor, std::ref(p_actor));
	textSelectable.DoubleClickedEvent += EDITOR_BIND(MoveToTarget, std::ref(p_actor));
}