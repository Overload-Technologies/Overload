/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include <filesystem>
#include <format>
#include <ranges>
#include <string>

#include <OvCore/ECS/Components/CAmbientBoxLight.h>
#include <OvCore/ECS/Components/CAmbientSphereLight.h>
#include <OvCore/ECS/Components/CAudioListener.h>
#include <OvCore/ECS/Components/CAudioSource.h>
#include <OvCore/ECS/Components/CCamera.h>
#include <OvCore/ECS/Components/CDirectionalLight.h>
#include <OvCore/ECS/Components/CMaterialRenderer.h>
#include <OvCore/ECS/Components/CModelRenderer.h>
#include <OvCore/ECS/Components/CPhysicalBox.h>
#include <OvCore/ECS/Components/CPhysicalCapsule.h>
#include <OvCore/ECS/Components/CPhysicalSphere.h>
#include <OvCore/ECS/Components/CPointLight.h>
#include <OvCore/ECS/Components/CPostProcessStack.h>
#include <OvCore/ECS/Components/CReflectionProbe.h>
#include <OvCore/ECS/Components/CSkinnedMeshRenderer.h>
#include <OvCore/ECS/Components/CSpotLight.h>
#include <OvCore/ECS/Components/CTransform.h>
#include <OvCore/Helpers/GUIDrawer.h>
#include <OvCore/Helpers/GUIHelpers.h>
#include <OvEditor/Core/EditorActions.h>
#include <OvEditor/Core/EditorResources.h>
#include <OvEditor/Helpers/PickerHelpers.h>
#include <OvEditor/Panels/Inspector.h>
#include <OvTools/Utils/PathParser.h>
#include <OvUI/Widgets/Buttons/Button.h>
#include <OvUI/Widgets/Layout/Columns.h>
#include <OvUI/Widgets/Layout/Spacing.h>
#include <OvWindowing/Dialogs/MessageBox.h>

using namespace OvCore::ECS;
using namespace OvCore::ECS::Components;
using namespace OvUI::Widgets;

namespace
{
	// Base interface for component operations
	struct IComponentInfo
	{
		virtual ~IComponentInfo() = default;
		virtual std::string_view GetName() const = 0;
		virtual void AddComponent(Actor& actor) const = 0;
		virtual bool IsAddable(Actor& actor) const = 0;
	};

	// Template implementation for each component type
	template<typename TComponent>
	struct ComponentInfo : public IComponentInfo
	{
		const std::string_view name;

		ComponentInfo(std::string_view p_name) : name(p_name) {}

		std::string_view GetName() const override { return name; }

		void AddComponent(Actor& p_actor) const override
		{
			p_actor.AddComponent<TComponent>();
		}

		bool IsAddable(Actor& p_actor) const override
		{
			if constexpr (std::is_base_of_v<CPhysicalObject, TComponent>)
			{
				return !p_actor.GetComponent<CPhysicalObject>();
			}
			else
			{
				return !p_actor.GetComponent<TComponent>();
			}
		}
	};

	template<typename TComponent>
	auto CreateComponentInfo(const std::string_view p_name) -> std::unique_ptr<IComponentInfo>
	{
		return std::make_unique<ComponentInfo<TComponent>>(p_name);
	}

	const auto componentRegistry = std::to_array({
		CreateComponentInfo<CModelRenderer>("Model Renderer"),
		CreateComponentInfo<CCamera>("Camera"),
		CreateComponentInfo<CPhysicalBox>("Physical Box"),
		CreateComponentInfo<CPhysicalSphere>("Physical Sphere"),
		CreateComponentInfo<CPhysicalCapsule>("Physical Capsule"),
		CreateComponentInfo<CPointLight>("Point Light"),
		CreateComponentInfo<CDirectionalLight>("Directional Light"),
		CreateComponentInfo<CSpotLight>("Spot Light"),
		CreateComponentInfo<CAmbientBoxLight>("Ambient Box Light"),
		CreateComponentInfo<CAmbientSphereLight>("Ambient Sphere Light"),
		CreateComponentInfo<CMaterialRenderer>("Material Renderer"),
		CreateComponentInfo<CSkinnedMeshRenderer>("Skinned Mesh Renderer"),
		CreateComponentInfo<CAudioSource>("Audio Source"),
		CreateComponentInfo<CAudioListener>("Audio Listener"),
		CreateComponentInfo<CPostProcessStack>("Post Process Stack"),
		CreateComponentInfo<CReflectionProbe>("Reflection Probe"),
	});

}

OvEditor::Panels::Inspector::Inspector(
	const std::string& p_title,
	bool p_opened,
	const OvUI::Settings::PanelWindowSettings & p_windowSettings
) : PanelWindow(p_title, p_opened, p_windowSettings)
{
	m_content = &CreateWidget<Layout::Group>();

	m_destroyedListener = Actor::DestroyedEvent += [this](Actor& p_destroyed) {
		if (m_targetActor.has_value() &&  &m_targetActor.value() == &p_destroyed)
		{
			UnFocus();
		}
	};
}

OvEditor::Panels::Inspector::~Inspector()
{
	Actor::DestroyedEvent -= m_destroyedListener;
	UnFocus();
}

void OvEditor::Panels::Inspector::FocusActor(Actor& p_target)
{
	if (m_targetActor)
	{
		UnFocus();
	}

	m_targetActor = p_target;

	m_componentAddedListener = m_targetActor->ComponentAddedEvent += [this] (auto&) { EDITOR_EXEC(DelayAction([this] { Refresh(); })); };
	m_behaviourAddedListener = m_targetActor->BehaviourAddedEvent += [this](auto&) { EDITOR_EXEC(DelayAction([this] { Refresh(); })); };
	m_componentRemovedListener = m_targetActor->ComponentRemovedEvent += [this](auto&) { EDITOR_EXEC(DelayAction([this] { Refresh(); })); };
	m_behaviourRemovedListener = m_targetActor->BehaviourRemovedEvent += [this](auto&) { EDITOR_EXEC(DelayAction([this] { Refresh(); })); };

	_Populate();

	EDITOR_EVENT(ActorSelectedEvent).Invoke(m_targetActor.value());
}

void OvEditor::Panels::Inspector::UnFocus()
{
	if (!m_targetActor)
	{
		return;
	}

	m_targetActor->ComponentAddedEvent -= m_componentAddedListener;
	m_targetActor->ComponentRemovedEvent -= m_componentRemovedListener;
	m_targetActor->BehaviourAddedEvent -= m_behaviourAddedListener;
	m_targetActor->BehaviourRemovedEvent -= m_behaviourRemovedListener;

	m_content->RemoveAllWidgets();

	EDITOR_EVENT(ActorUnselectedEvent).Invoke(m_targetActor.value());

	m_targetActor.reset();
}

OvTools::Utils::OptRef<Actor> OvEditor::Panels::Inspector::GetTargetActor() const
{
	return m_targetActor;
}

void OvEditor::Panels::Inspector::_Populate()
{
	OVASSERT(m_targetActor.has_value(), "Cannot populate inspector without a target actor");
	_PopulateActorInfo();
	m_content->CreateWidget<Visual::Separator>();
	_PopulateActorComponents();
	_PopulateActorBehaviours();
	m_content->CreateWidget<Visual::Separator>();
	_DrawAddSection();
}

void OvEditor::Panels::Inspector::_PopulateActorInfo()
{
	auto& headerColumns = m_content->CreateWidget<Layout::Columns<2>>();

	OvCore::Helpers::GUIDrawer::DrawString(
		headerColumns,
		"Name",
		[this] { return m_targetActor->GetName(); },
		[this](const std::string& p_newName) { m_targetActor->SetName(p_newName); }
	);

	OvCore::Helpers::GUIDrawer::DrawString(
		headerColumns,
		"Tag",
		[this] { return m_targetActor->GetTag(); },
		[this](const std::string& p_newName) { m_targetActor->SetTag(p_newName); }
	);

	OvCore::Helpers::GUIDrawer::DrawBoolean(
		headerColumns,
		"Active",
		[this]{ return m_targetActor->IsSelfActive(); },
		[this](bool p_active) { m_targetActor->SetActive(p_active); }
	);

	OvCore::Helpers::GUIDrawer::DrawReadOnlyString(
		headerColumns,
		"GUID",
		[this] { return std::format("{:016X}", m_targetActor->GetGUID()); }
	);

	_DrawAddSection();
}

void OvEditor::Panels::Inspector::_PopulateActorComponents()
{
	auto& components = m_targetActor->GetComponents();
	const int total = static_cast<int>(components.size());
	for (int i = 0; i < total; ++i)
	{
		_DrawComponent(*components[i], i, total);
	}
}

void OvEditor::Panels::Inspector::_PopulateActorBehaviours()
{
	auto& order = m_targetActor->GetBehavioursOrder();
	auto& behaviours = m_targetActor->GetBehaviours();
	const int total = static_cast<int>(order.size());
	for (int i = 0; i < total; ++i)
	{
		auto it = behaviours.find(order[i]);
		if (it != behaviours.end())
			_DrawBehaviour(it->second, i, total);
	}
}

void OvEditor::Panels::Inspector::_DrawAddSection()
{
	auto& addButton = m_content->CreateWidget<Buttons::Button>("Add Component...", OvMaths::FVector2{ -1.f, 0 });
	addButton.ClickedEvent += [this] {
		if (!m_targetActor.has_value())
			return;

		const uint32_t componentIconID = EDITOR_CONTEXT(editorResources)->GetTexture("Component")->GetTexture().GetID();

		OvCore::Helpers::GUIHelpers::PickerItemList items;

		for (const auto& info : componentRegistry)
		{
			if (!info->IsAddable(m_targetActor.value()))
				continue;

			const std::string name = std::string(info->GetName());
			items.Add({
				name,
				name,
				name,
				componentIconID,
				[this, name, &info = *info] {
					if (!m_targetActor.has_value())
						return;

					if (!info.IsAddable(m_targetActor.value()))
					{
						OvWindowing::Dialogs::MessageBox(
							"Component already attached",
							"The component \"" + name + "\" is already attached to this actor.",
							OvWindowing::Dialogs::MessageBox::EMessageType::ERROR,
							OvWindowing::Dialogs::MessageBox::EButtonLayout::OK
						);
						return;
					}

					info.AddComponent(m_targetActor.value());
				}
			});
		}

		OvEditor::Helpers::PickerHelpers::AddFileItems(
			items,
			OvTools::Utils::PathParser::EFileType::SCRIPT,
			[this](const std::string& p_resourcePath) {
				if (!m_targetActor.has_value())
					return;

				const std::string scriptPath = EDITOR_EXEC(GetScriptPath(p_resourcePath));
				const std::string displayName = OvTools::Utils::PathParser::GetElementName(scriptPath);

				if (m_targetActor->GetBehaviour(scriptPath))
				{
					OvWindowing::Dialogs::MessageBox(
						"Script already attached",
						"The script \"" + displayName + "\" is already attached to this actor.",
						OvWindowing::Dialogs::MessageBox::EMessageType::ERROR,
						OvWindowing::Dialogs::MessageBox::EButtonLayout::OK
					);
					return;
				}

				m_targetActor->AddBehaviour(scriptPath);
			},
			true, false
		);

		OvCore::Helpers::GUIHelpers::OpenPicker(std::move(items), "Add Component");
	};
}

void OvEditor::Panels::Inspector::_DrawComponent(AComponent& p_component, int p_index, int p_total)
{
	auto& header = m_content->CreateWidget<Layout::GroupCollapsable>(p_component.GetName());
	const bool isTransform = dynamic_cast<CTransform*>(&p_component) != nullptr;
	header.closable = !isTransform;
	header.CloseEvent += [this, &header, &p_component] { 
		p_component.owner.RemoveComponent(p_component);
	};

	if (!isTransform)
	{
		header.reorderable = true;
		header.canMoveUp = (p_index > 1);
		header.canMoveDown = (p_index < p_total - 1);

		auto move = [this, &p_component](bool up) {
			auto& comps = p_component.owner.GetComponents();
			auto it = std::find_if(comps.begin(), comps.end(), [&](const auto& c) { return c.get() == &p_component; });
			if (up) {
				if (it != comps.begin() && !dynamic_cast<CTransform*>(std::prev(it)->get()))
					std::iter_swap(it, std::prev(it));
			} else {
				if (auto next = std::next(it); next != comps.end())
					std::iter_swap(it, next);
			}
			EDITOR_EXEC(DelayAction([this] { Refresh(); }));
		};
		header.MoveUpEvent += [move] { move(true); };
		header.MoveDownEvent += [move] { move(false); };
	}

	auto& columns = header.CreateWidget<Layout::Columns<2>>();
	columns.SetID("comp_" + p_component.GetName());
	columns.widths[0] = 200;
	p_component.OnInspector(columns);
}

void OvEditor::Panels::Inspector::_DrawBehaviour(Behaviour& p_behaviour, int p_index, int p_total)
{
	auto& header = m_content->CreateWidget<Layout::GroupCollapsable>(std::filesystem::path(p_behaviour.name).replace_extension().string());
	header.closable = true;
	header.CloseEvent += [&p_behaviour] {
		p_behaviour.owner.RemoveBehaviour(p_behaviour);
	};

	header.reorderable = true;
	header.canMoveUp = (p_index > 0);
	header.canMoveDown = (p_index < p_total - 1);

	auto move = [this, &p_behaviour](bool up) {
		auto& order = p_behaviour.owner.GetBehavioursOrder();
		auto it = std::find(order.begin(), order.end(), p_behaviour.name);
		if (up) {
			if (it != order.begin())
				std::iter_swap(it, std::prev(it));
		} else {
			if (auto next = std::next(it); next != order.end())
				std::iter_swap(it, next);
		}
		EDITOR_EXEC(DelayAction([this] { Refresh(); }));
	};
	header.MoveUpEvent += [move] { move(true); };
	header.MoveDownEvent += [move] { move(false); };

	auto& columns = header.CreateWidget<Layout::Columns<2>>();
	columns.SetID("bhv_" + p_behaviour.name);
	columns.widths[0] = 200;
	p_behaviour.OnInspector(columns);
}

void OvEditor::Panels::Inspector::Refresh()
{
	if (m_targetActor)
	{
		m_content->RemoveAllWidgets();
		_Populate();
	}
}
