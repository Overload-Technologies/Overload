/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include <map>
#include <ranges>

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
#include <OvCore/ECS/Components/CSpotLight.h>
#include <OvCore/ECS/Components/CTransform.h>
#include <OvEditor/Core/EditorActions.h>
#include <OvEditor/Panels/Inspector.h>
#include <OvUI/Plugins/DDTarget.h>
#include <OvUI/Widgets/Buttons/Button.h>
#include <OvUI/Widgets/Layout/Columns.h>
#include <OvUI/Widgets/Layout/Spacing.h>

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
		CreateComponentInfo<CAudioSource>("Audio Source"),
		CreateComponentInfo<CAudioListener>("Audio Listener"),
		CreateComponentInfo<CPostProcessStack>("Post Process Stack")
	});

	OvTools::Utils::OptRef<IComponentInfo> GetComponentInfo(size_t p_index)
	{
		if (p_index >= componentRegistry.size())
		{
			return std::nullopt;
		}

		return *componentRegistry.at(p_index);
	}

	std::map<int, std::string> GetComponentMap()
	{
		std::map<int, std::string> choices;
		for (size_t i = 0; i < componentRegistry.size(); ++i)
		{
			choices[static_cast<int>(i)] = componentRegistry.at(i)->GetName();
		}
		return choices;
	}
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

	_DrawAddComponentSection();
	_DrawAddScriptSection();
}

void OvEditor::Panels::Inspector::_PopulateActorComponents()
{
	for (auto component : m_targetActor->GetComponents() | std::views::reverse)
	{
		_DrawComponent(*component);
	}
}

void OvEditor::Panels::Inspector::_PopulateActorBehaviours()
{
	std::map<std::string, std::reference_wrapper<Behaviour>> behaviours;

	// Sorts the behaviours alphabetically
	for (auto& behaviour : m_targetActor->GetBehaviours() | std::views::values)
	{
		behaviours.emplace(behaviour.name, std::ref(behaviour));
	}

	// Iterate through the sorted behaviours
	for (auto& behaviour : behaviours | std::views::values)
	{
		_DrawBehaviour(behaviour.get());
	}
}

void OvEditor::Panels::Inspector::_DrawAddComponentSection()
{
	// Component selection
	auto& componentSelector = m_content->CreateWidget<Selection::ComboBox>(m_selectedComponent);
	componentSelector.lineBreak = false;
	componentSelector.choices = GetComponentMap();

	m_addComponentButton = m_content->CreateWidget<Buttons::Button>("Add Component", OvMaths::FVector2{ 100.f, 0 });
	m_addComponentButton->idleBackgroundColor = OvUI::Types::Color{ 0.7f, 0.5f, 0.f };
	m_addComponentButton->textColor = OvUI::Types::Color::White;
	m_addComponentButton->ClickedEvent += [this, &componentSelector] {
		if (auto compInfo = GetComponentInfo(componentSelector.currentChoice))
		{
			compInfo->AddComponent(m_targetActor.value());
		}
	};

	componentSelector.ValueChangedEvent += [this](int p_value) {
		// We keep track of the selected component so that if the inspector is refreshed,
		// the selected component will be the same as before.
		m_selectedComponent = p_value;
		_UpdateAddComponentButton();
	};

	_UpdateAddComponentButton();
}

void OvEditor::Panels::Inspector::_DrawAddScriptSection()
{
	// Script selelection
	auto& scriptSelector = m_content->CreateWidget<InputFields::InputText>(m_selectedScript);
	scriptSelector.lineBreak = false;
	auto& ddTarget = scriptSelector.AddPlugin<OvUI::Plugins::DDTarget<std::pair<std::string, Layout::Group*>>>("File");

	m_addScriptButton = m_content->CreateWidget<Buttons::Button>("Add Script", OvMaths::FVector2{ 100.f, 0 });
	m_addScriptButton->idleBackgroundColor = OvUI::Types::Color{ 0.7f, 0.5f, 0.f };
	m_addScriptButton->textColor = OvUI::Types::Color::White;
	m_addScriptButton->disabled = true;

	scriptSelector.ContentChangedEvent += [this](const std::string& p_content) {
		m_selectedScript = p_content;
		_UpdateAddScriptButton();
	};

	_UpdateAddScriptButton();

	m_addScriptButton->ClickedEvent += [this] {
		const std::string defaultScriptExtension = OVSERVICE(OvCore::Scripting::ScriptEngine).GetDefaultExtension();

		const auto realScriptPath =
			EDITOR_CONTEXT(projectScriptsPath) /
			std::format("{}{}", m_selectedScript, defaultScriptExtension);

		// Ensure that the script is a valid one
		if (std::filesystem::exists(realScriptPath))
		{
			m_targetActor->AddBehaviour(m_selectedScript);
			_UpdateAddScriptButton();
		}
	};

	ddTarget.DataReceivedEvent += [&scriptSelector](std::pair<std::string, Layout::Group*> p_data) {
		scriptSelector.content = EDITOR_EXEC(GetScriptPath(p_data.first));
		scriptSelector.ContentChangedEvent.Invoke(scriptSelector.content);
	};
}

void OvEditor::Panels::Inspector::_DrawComponent(AComponent& p_component)
{
	auto& header = m_content->CreateWidget<Layout::GroupCollapsable>(p_component.GetName());
	header.closable = !dynamic_cast<CTransform*>(&p_component);
	header.CloseEvent += [this, &header, &p_component] { 
		p_component.owner.RemoveComponent(p_component);
	};
	auto& columns = header.CreateWidget<Layout::Columns<2>>();
	columns.widths[0] = 200;
	p_component.OnInspector(columns);
}

void OvEditor::Panels::Inspector::_DrawBehaviour(Behaviour& p_behaviour)
{
	auto& header = m_content->CreateWidget<Layout::GroupCollapsable>(p_behaviour.name);
	header.closable = true;
	header.CloseEvent += [&p_behaviour] {
		p_behaviour.owner.RemoveBehaviour(p_behaviour);
	};

	auto& columns = header.CreateWidget<Layout::Columns<2>>();
	columns.widths[0] = 200;
	p_behaviour.OnInspector(columns);
}

void OvEditor::Panels::Inspector::_UpdateAddComponentButton()
{
	OVASSERT(m_addComponentButton.has_value(), "Add component button not set");

	m_addComponentButton->disabled = ![this] {
		if (auto compInfo = GetComponentInfo(m_selectedComponent))
		{
			return compInfo->IsAddable(m_targetActor.value());
		}

		return false;
	}();
}

void OvEditor::Panels::Inspector::_UpdateAddScriptButton()
{
	OVASSERT(m_addScriptButton.has_value(), "Add script button not set");

	const std::string defaultScriptExtension = OVSERVICE(OvCore::Scripting::ScriptEngine).GetDefaultExtension();

	const auto realScriptPath =
		EDITOR_CONTEXT(projectScriptsPath) /
		std::format("{}{}", m_selectedScript, defaultScriptExtension);

	const bool canAdd =
		std::filesystem::exists(realScriptPath) &&
		!m_targetActor->GetBehaviour(m_selectedScript);

	m_addScriptButton->disabled = !canAdd;
}

void OvEditor::Panels::Inspector::Refresh()
{
	if (m_targetActor)
	{
		m_content->RemoveAllWidgets();
		_Populate();
	}
}
