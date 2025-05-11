/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include <map>

#include "OvEditor/Panels/Inspector.h"

#include <OvUI/Widgets/Texts/Text.h>
#include <OvUI/Widgets/Buttons/Button.h>
#include <OvUI/Widgets/Drags/DragMultipleFloats.h>
#include <OvUI/Widgets/Drags/DragFloat.h>
#include <OvUI/Widgets/Layout/Spacing.h>
#include <OvUI/Widgets/Layout/Columns.h>
#include <OvUI/Widgets/Visual/Image.h>
#include <OvUI/Widgets/InputFields/InputFloat.h>
#include <OvUI/Widgets/Selection/ColorEdit.h>
#include <OvUI/Plugins/DDTarget.h>

#include <OvCore/Global/ServiceLocator.h>
#include <OvCore/ResourceManagement/ModelManager.h>
#include <OvCore/ResourceManagement/TextureManager.h>
#include <OvCore/ResourceManagement/ShaderManager.h>

#include <OvCore/ECS/Components/CTransform.h>
#include <OvCore/ECS/Components/CModelRenderer.h>
#include <OvCore/ECS/Components/CCamera.h>
#include <OvCore/ECS/Components/CPhysicalBox.h>
#include <OvCore/ECS/Components/CPhysicalSphere.h>
#include <OvCore/ECS/Components/CPhysicalCapsule.h>
#include <OvCore/ECS/Components/CPointLight.h>
#include <OvCore/ECS/Components/CDirectionalLight.h>
#include <OvCore/ECS/Components/CSpotLight.h>
#include <OvCore/ECS/Components/CAmbientBoxLight.h>
#include <OvCore/ECS/Components/CMaterialRenderer.h>
#include <OvCore/ECS/Components/CAudioSource.h>
#include <OvCore/ECS/Components/CAudioListener.h>
#include "OvCore/ECS/Components/CAmbientSphereLight.h"
#include "OvCore/ECS/Components/CPostProcessStack.h"

#include <OvWindowing/Dialogs/MessageBox.h>

#include "OvEditor/Core/EditorActions.h"

using namespace OvCore::ECS::Components;
using namespace OvUI::Widgets;

OvEditor::Panels::Inspector::Inspector
(
	const std::string& p_title,
	bool p_opened,
	const OvUI::Settings::PanelWindowSettings & p_windowSettings
) : PanelWindow(p_title, p_opened, p_windowSettings)
{
	m_inspectorHeader = &CreateWidget<OvUI::Widgets::Layout::Group>();
	m_inspectorHeader->enabled = false;
	m_actorInfo = &CreateWidget<OvUI::Widgets::Layout::Group>();

	auto& headerColumns = m_inspectorHeader->CreateWidget<OvUI::Widgets::Layout::Columns<2>>();

	/* Name field */
	auto nameGatherer = [this] { return m_targetActor ? m_targetActor->GetName() : "%undef%"; };
	auto nameProvider = [this](const std::string& p_newName) { if (m_targetActor) m_targetActor->SetName(p_newName); };
	OvCore::Helpers::GUIDrawer::DrawString(headerColumns, "Name", nameGatherer, nameProvider);

	/* Tag field */
	auto tagGatherer = [this] { return m_targetActor ? m_targetActor->GetTag() : "%undef%"; };
	auto tagProvider = [this](const std::string & p_newName) { if (m_targetActor) m_targetActor->SetTag(p_newName); };
	OvCore::Helpers::GUIDrawer::DrawString(headerColumns, "Tag", tagGatherer, tagProvider);

	/* Active field */
	auto activeGatherer = [this] { return m_targetActor ? m_targetActor->IsSelfActive() : false; };
	auto activeProvider = [this](bool p_active) { if (m_targetActor) m_targetActor->SetActive(p_active); };
	OvCore::Helpers::GUIDrawer::DrawBoolean(headerColumns, "Active", activeGatherer, activeProvider);

	/* Component select + button */
	{
		auto& componentSelectorWidget = m_inspectorHeader->CreateWidget<OvUI::Widgets::Selection::ComboBox>(0);
		componentSelectorWidget.lineBreak = false;
		componentSelectorWidget.choices.emplace(0, "Model Renderer");
		componentSelectorWidget.choices.emplace(1, "Camera");
		componentSelectorWidget.choices.emplace(2, "Physical Box");
		componentSelectorWidget.choices.emplace(3, "Physical Sphere");
		componentSelectorWidget.choices.emplace(4, "Physical Capsule");
		componentSelectorWidget.choices.emplace(5, "Point Light");
		componentSelectorWidget.choices.emplace(6, "Directional Light");
		componentSelectorWidget.choices.emplace(7, "Spot Light");
		componentSelectorWidget.choices.emplace(8, "Ambient Box Light");
		componentSelectorWidget.choices.emplace(9, "Ambient Sphere Light");
		componentSelectorWidget.choices.emplace(10, "Material Renderer");
		componentSelectorWidget.choices.emplace(11, "Audio Source");
		componentSelectorWidget.choices.emplace(12, "Audio Listener");
		componentSelectorWidget.choices.emplace(13, "Post Process Stack");

		auto& addComponentButton = m_inspectorHeader->CreateWidget<OvUI::Widgets::Buttons::Button>("Add Component", OvMaths::FVector2{ 100.f, 0 });
		addComponentButton.idleBackgroundColor = OvUI::Types::Color{ 0.7f, 0.5f, 0.f };
		addComponentButton.textColor = OvUI::Types::Color::White;
		addComponentButton.ClickedEvent += [&componentSelectorWidget, this]
		{
			switch (componentSelectorWidget.currentChoice)
			{
			case 0: GetTargetActor()->AddComponent<CModelRenderer>(); GetTargetActor()->AddComponent<CMaterialRenderer>(); break;
			case 1: GetTargetActor()->AddComponent<CCamera>();				break;
			case 2: GetTargetActor()->AddComponent<CPhysicalBox>();			break;
			case 3: GetTargetActor()->AddComponent<CPhysicalSphere>();		break;
			case 4: GetTargetActor()->AddComponent<CPhysicalCapsule>();		break;
			case 5: GetTargetActor()->AddComponent<CPointLight>();			break;
			case 6: GetTargetActor()->AddComponent<CDirectionalLight>();	break;
			case 7: GetTargetActor()->AddComponent<CSpotLight>();			break;
			case 8: GetTargetActor()->AddComponent<CAmbientBoxLight>();		break;
			case 9: GetTargetActor()->AddComponent<CAmbientSphereLight>();	break;
			case 10: GetTargetActor()->AddComponent<CMaterialRenderer>();	break;
			case 11: GetTargetActor()->AddComponent<CAudioSource>();		break;
			case 12: GetTargetActor()->AddComponent<CAudioListener>();		break;
			case 13: GetTargetActor()->AddComponent<CPostProcessStack>();	break;
			}

			componentSelectorWidget.ValueChangedEvent.Invoke(componentSelectorWidget.currentChoice);
		};

		componentSelectorWidget.ValueChangedEvent += [this, &addComponentButton](int p_value)
		{
			auto defineButtonsStates = [&addComponentButton](bool p_componentExists)
			{
				addComponentButton.disabled = p_componentExists;
			};

			switch (p_value)
			{
			case 0: defineButtonsStates(GetTargetActor()->GetComponent<CModelRenderer>());		return;
			case 1: defineButtonsStates(GetTargetActor()->GetComponent<CCamera>());				return;
			case 2: defineButtonsStates(GetTargetActor()->GetComponent<CPhysicalObject>());		return;
			case 3: defineButtonsStates(GetTargetActor()->GetComponent<CPhysicalObject>());		return;
			case 4: defineButtonsStates(GetTargetActor()->GetComponent<CPhysicalObject>());		return;
			case 5: defineButtonsStates(GetTargetActor()->GetComponent<CPointLight>());			return;
			case 6: defineButtonsStates(GetTargetActor()->GetComponent<CDirectionalLight>());	return;
			case 7: defineButtonsStates(GetTargetActor()->GetComponent<CSpotLight>());			return;
			case 8: defineButtonsStates(GetTargetActor()->GetComponent<CAmbientBoxLight>());	return;
			case 9: defineButtonsStates(GetTargetActor()->GetComponent<CAmbientSphereLight>());	return;
			case 10: defineButtonsStates(GetTargetActor()->GetComponent<CMaterialRenderer>());	return;
			case 11: defineButtonsStates(GetTargetActor()->GetComponent<CAudioSource>());		return;
			case 12: defineButtonsStates(GetTargetActor()->GetComponent<CAudioListener>());		return;
			case 13: defineButtonsStates(GetTargetActor()->GetComponent<CPostProcessStack>());	return;
			}
		};

		m_componentSelectorWidget = &componentSelectorWidget;
	}

	/* Script selector + button */
	{
		m_scriptSelectorWidget = &m_inspectorHeader->CreateWidget<OvUI::Widgets::InputFields::InputText>("");
		m_scriptSelectorWidget->lineBreak = false;
		auto& ddTarget = m_scriptSelectorWidget->AddPlugin<OvUI::Plugins::DDTarget<std::pair<std::string, Layout::Group*>>>("File");
		
		auto& addScriptButton = m_inspectorHeader->CreateWidget<OvUI::Widgets::Buttons::Button>("Add Script", OvMaths::FVector2{ 100.f, 0 });
		addScriptButton.idleBackgroundColor = OvUI::Types::Color{ 0.7f, 0.5f, 0.f };
		addScriptButton.textColor = OvUI::Types::Color::White;

		// Add script button state updater
		const auto updateAddScriptButton = [&addScriptButton, this](const std::string& p_script)
		{
			const std::string defaultScriptExtension = OVSERVICE(OvCore::Scripting::ScriptEngine).GetDefaultExtension();

			const std::string realScriptPath =
				EDITOR_CONTEXT(projectScriptsPath) +
				p_script +
				defaultScriptExtension;

			const auto targetActor = GetTargetActor();
			const bool isScriptValid = std::filesystem::exists(realScriptPath) && targetActor && !targetActor->GetBehaviour(p_script);

			addScriptButton.disabled = !isScriptValid;
		};

		m_scriptSelectorWidget->ContentChangedEvent += updateAddScriptButton;

		addScriptButton.ClickedEvent += [updateAddScriptButton, this] {
			const std::string defaultScriptExtension = OVSERVICE(OvCore::Scripting::ScriptEngine).GetDefaultExtension();

			const std::string realScriptPath =
				EDITOR_CONTEXT(projectScriptsPath) +
				m_scriptSelectorWidget->content +
				defaultScriptExtension;

			// Ensure that the script is a valid one
			if (std::filesystem::exists(realScriptPath))
			{
				GetTargetActor()->AddBehaviour(m_scriptSelectorWidget->content);
				updateAddScriptButton(m_scriptSelectorWidget->content);
			}
		};

		ddTarget.DataReceivedEvent += [updateAddScriptButton, this](std::pair<std::string, Layout::Group*> p_data)
		{
			m_scriptSelectorWidget->content = EDITOR_EXEC(GetScriptPath(p_data.first));
			updateAddScriptButton(m_scriptSelectorWidget->content);
		};
	}

	m_inspectorHeader->CreateWidget<OvUI::Widgets::Visual::Separator>();

	m_destroyedListener = OvCore::ECS::Actor::DestroyedEvent += [this](OvCore::ECS::Actor& p_destroyed)
	{ 
		if (&p_destroyed == m_targetActor)
			UnFocus();
	};
}

OvEditor::Panels::Inspector::~Inspector()
{
	OvCore::ECS::Actor::DestroyedEvent -= m_destroyedListener;

	UnFocus();
}

void OvEditor::Panels::Inspector::FocusActor(OvCore::ECS::Actor& p_target)
{
	if (m_targetActor)
		UnFocus();

	m_actorInfo->RemoveAllWidgets();

	m_targetActor = &p_target;

	m_componentAddedListener = m_targetActor->ComponentAddedEvent += [this] (auto& useless) { EDITOR_EXEC(DelayAction([this] { Refresh(); })); };
	m_behaviourAddedListener =		m_targetActor->BehaviourAddedEvent += [this](auto& useless) { EDITOR_EXEC(DelayAction([this] { Refresh(); })); };
	m_componentRemovedListener =	m_targetActor->ComponentRemovedEvent += [this](auto& useless) { EDITOR_EXEC(DelayAction([this] { Refresh(); })); };
	m_behaviourRemovedListener =	m_targetActor->BehaviourRemovedEvent += [this](auto& useless) { EDITOR_EXEC(DelayAction([this] { Refresh(); })); };

	m_inspectorHeader->enabled = true;

	CreateActorInspector(p_target);

    // Force component and script selectors to trigger their ChangedEvent to update button states
	m_componentSelectorWidget->ValueChangedEvent.Invoke(m_componentSelectorWidget->currentChoice);
    m_scriptSelectorWidget->ContentChangedEvent.Invoke(m_scriptSelectorWidget->content);

	EDITOR_EVENT(ActorSelectedEvent).Invoke(*m_targetActor);
}

void OvEditor::Panels::Inspector::UnFocus()
{
	if (m_targetActor)
	{
		m_targetActor->ComponentAddedEvent		-= m_componentAddedListener;
		m_targetActor->ComponentRemovedEvent	-= m_componentRemovedListener;
		m_targetActor->BehaviourAddedEvent		-= m_behaviourAddedListener;
		m_targetActor->BehaviourRemovedEvent	-= m_behaviourRemovedListener;
	}

	SoftUnFocus();
}

void OvEditor::Panels::Inspector::SoftUnFocus()
{
    if (m_targetActor)
    {
        EDITOR_EVENT(ActorUnselectedEvent).Invoke(*m_targetActor);
        m_inspectorHeader->enabled = false;
        m_targetActor = nullptr;
        m_actorInfo->RemoveAllWidgets();
    }
}

OvCore::ECS::Actor * OvEditor::Panels::Inspector::GetTargetActor() const
{
	return m_targetActor;
}

void OvEditor::Panels::Inspector::CreateActorInspector(OvCore::ECS::Actor& p_target)
{
	std::map<std::string, OvCore::ECS::Components::AComponent*> components;

	for (auto component : p_target.GetComponents())
		if (component->GetName() != "Transform")
			components[component->GetName()] = component.get();

	auto transform = p_target.GetComponent<OvCore::ECS::Components::CTransform>();
	if (transform)
		DrawComponent(*transform);

	for (auto& [name, instance] : components)
		DrawComponent(*instance);

	auto& behaviours = p_target.GetBehaviours();

	for (auto&[name, behaviour] : behaviours)
		DrawBehaviour(behaviour);
}

void OvEditor::Panels::Inspector::DrawComponent(OvCore::ECS::Components::AComponent& p_component)
{
	if (auto inspectorItem = dynamic_cast<OvCore::API::IInspectorItem*>(&p_component); inspectorItem)
	{
		auto& header = m_actorInfo->CreateWidget<OvUI::Widgets::Layout::GroupCollapsable>(p_component.GetName());
		header.closable = !dynamic_cast<OvCore::ECS::Components::CTransform*>(&p_component);
		header.CloseEvent += [this, &header, &p_component]
		{ 
			if (p_component.owner.RemoveComponent(p_component))
				m_componentSelectorWidget->ValueChangedEvent.Invoke(m_componentSelectorWidget->currentChoice);
		};
		auto& columns = header.CreateWidget<OvUI::Widgets::Layout::Columns<2>>();
		columns.widths[0] = 200;
		inspectorItem->OnInspector(columns);
	}
}

void OvEditor::Panels::Inspector::DrawBehaviour(OvCore::ECS::Components::Behaviour & p_behaviour)
{
	if (auto inspectorItem = dynamic_cast<OvCore::API::IInspectorItem*>(&p_behaviour); inspectorItem)
	{
		auto& header = m_actorInfo->CreateWidget<OvUI::Widgets::Layout::GroupCollapsable>(p_behaviour.name);
		header.closable = true;
		header.CloseEvent += [this, &header, &p_behaviour]
		{
			p_behaviour.owner.RemoveBehaviour(p_behaviour);
		};

		auto& columns = header.CreateWidget<OvUI::Widgets::Layout::Columns<2>>();
		columns.widths[0] = 200;
		inspectorItem->OnInspector(columns);
	}
}

void OvEditor::Panels::Inspector::Refresh()
{
	if (m_targetActor)
	{
		m_actorInfo->RemoveAllWidgets();
		CreateActorInspector(*m_targetActor);
	}
}
