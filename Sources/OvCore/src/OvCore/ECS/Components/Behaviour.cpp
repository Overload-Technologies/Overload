/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include <limits>
#include <utility>

#include <tinyxml2.h>

#include <OvCore/ECS/Actor.h>
#include <OvCore/ECS/Components/Behaviour.h>
#include <OvCore/Global/ServiceLocator.h>
#include <OvCore/Helpers/GUIDrawer.h>
#include <OvCore/Scripting/ScriptEngine.h>

#include <OvDebug/Logger.h>

#include <OvUI/Plugins/DataDispatcher.h>
#include <OvUI/Widgets/Drags/DragSingleScalar.h>
#include <OvUI/Widgets/InputFields/InputText.h>
#include <OvUI/Widgets/Layout/Dummy.h>
#include <OvUI/Widgets/Layout/Group.h>
#include <OvUI/Widgets/Selection/CheckBox.h>
#include <OvUI/Widgets/Texts/TextColored.h>

OvCore::ECS::Components::Behaviour::Behaviour(ECS::Actor& p_owner, const std::string& p_name) :
	name(p_name), AComponent(p_owner)
{
	OVSERVICE(Scripting::ScriptEngine).AddBehaviour(*this);
}

OvCore::ECS::Components::Behaviour::~Behaviour()
{
	OVSERVICE(Scripting::ScriptEngine).RemoveBehaviour(*this);
}

std::string OvCore::ECS::Components::Behaviour::GetName()
{
	return "Behaviour";
}

std::string OvCore::ECS::Components::Behaviour::GetTypeName()
{
	return std::string{ComponentTraits<Behaviour>::Name};
}

void OvCore::ECS::Components::Behaviour::SetScript(std::unique_ptr<Scripting::Script> &&p_scriptContext)
{
	m_script = std::move(p_scriptContext);

	if (!m_script || !m_script->IsValid())
	{
		m_scriptProperties.clear();
		m_scriptDefaults.clear();
		m_unlockedProperties.clear();
		return;
	}

	m_scriptDefaults = m_script->GetDefaultProperties();

	auto old = std::exchange(m_scriptProperties, {});
	auto oldUnlocked = std::exchange(m_unlockedProperties, {});

	for (const auto& [key, defaultVal] : m_scriptDefaults)
	{
		const bool wasUnlocked = oldUnlocked.contains(key);
		auto it = old.find(key);
		const bool canPreserve = wasUnlocked && it != old.end() && it->second.index() == defaultVal.index();

		m_scriptProperties[key] = canPreserve ? std::move(it->second) : defaultVal;

		if (wasUnlocked)
			m_unlockedProperties.insert(key);
	}

	// Push user overrides back into the live script.
	for (const auto& [key, val] : m_scriptProperties)
		m_script->SetProperty(key, val);
}

OvTools::Utils::OptRef<OvCore::Scripting::Script> OvCore::ECS::Components::Behaviour::GetScript()
{
	if (m_script)
	{
		return { *m_script };
	}

	return std::nullopt;
}

void OvCore::ECS::Components::Behaviour::RemoveScript()
{
	m_script.reset();
}

void OvCore::ECS::Components::Behaviour::OnAwake()
{
	OVSERVICE(Scripting::ScriptEngine).OnAwake(*this);
}

void OvCore::ECS::Components::Behaviour::OnStart()
{
	OVSERVICE(Scripting::ScriptEngine).OnStart(*this);
}

void OvCore::ECS::Components::Behaviour::OnEnable()
{
	OVSERVICE(Scripting::ScriptEngine).OnEnable(*this);
}

void OvCore::ECS::Components::Behaviour::OnDisable()
{
	OVSERVICE(Scripting::ScriptEngine).OnDisable(*this);
}

void OvCore::ECS::Components::Behaviour::OnDestroy()
{
	OVSERVICE(Scripting::ScriptEngine).OnDestroy(*this);
}

void OvCore::ECS::Components::Behaviour::OnUpdate(float p_deltaTime)
{
	OVSERVICE(Scripting::ScriptEngine).OnUpdate(*this, p_deltaTime);
}

void OvCore::ECS::Components::Behaviour::OnFixedUpdate(float p_deltaTime)
{
	OVSERVICE(Scripting::ScriptEngine).OnFixedUpdate(*this, p_deltaTime);
}

void OvCore::ECS::Components::Behaviour::OnLateUpdate(float p_deltaTime)
{
	OVSERVICE(Scripting::ScriptEngine).OnLateUpdate(*this, p_deltaTime);
}

void OvCore::ECS::Components::Behaviour::OnCollisionEnter(Components::CPhysicalObject& p_otherObject)
{
	OVSERVICE(Scripting::ScriptEngine).OnCollisionEnter(*this, p_otherObject);
}

void OvCore::ECS::Components::Behaviour::OnCollisionStay(Components::CPhysicalObject& p_otherObject)
{
	OVSERVICE(Scripting::ScriptEngine).OnCollisionStay(*this, p_otherObject);
}

void OvCore::ECS::Components::Behaviour::OnCollisionExit(Components::CPhysicalObject& p_otherObject)
{
	OVSERVICE(Scripting::ScriptEngine).OnCollisionExit(*this, p_otherObject);
}

void OvCore::ECS::Components::Behaviour::OnTriggerEnter(Components::CPhysicalObject& p_otherObject)
{
	OVSERVICE(Scripting::ScriptEngine).OnTriggerEnter(*this, p_otherObject);
}

void OvCore::ECS::Components::Behaviour::OnTriggerStay(Components::CPhysicalObject& p_otherObject)
{
	OVSERVICE(Scripting::ScriptEngine).OnTriggerStay(*this, p_otherObject);
}

void OvCore::ECS::Components::Behaviour::OnTriggerExit(Components::CPhysicalObject& p_otherObject)
{
	OVSERVICE(Scripting::ScriptEngine).OnTriggerExit(*this, p_otherObject);
}

void OvCore::ECS::Components::Behaviour::OnSerialize(tinyxml2::XMLDocument & p_doc, tinyxml2::XMLNode * p_node)
{
	if (m_unlockedProperties.empty()) return;

	tinyxml2::XMLNode* propsNode = p_doc.NewElement("script_properties");
	p_node->InsertEndChild(propsNode);

	for (const auto& [fieldKey, fieldValue] : m_scriptProperties)
	{
		if (!m_unlockedProperties.contains(fieldKey)) continue;

		tinyxml2::XMLElement* elem = p_doc.NewElement(fieldKey.c_str());

		std::visit([elem]<typename T>(const T& v) {
			if constexpr (std::is_same_v<T, bool>)
				{ elem->SetAttribute("type", "bool");   elem->SetText(v); }
			else if constexpr (std::is_same_v<T, double>)
				{ elem->SetAttribute("type", "number"); elem->SetText(v); }
			else
				{ elem->SetAttribute("type", "string"); elem->SetText(v.c_str()); }
		}, fieldValue);

		propsNode->InsertEndChild(elem);
	}
}

void OvCore::ECS::Components::Behaviour::OnDeserialize(tinyxml2::XMLDocument & p_doc, tinyxml2::XMLNode * p_node)
{
	const tinyxml2::XMLElement* propsNode = p_node->FirstChildElement("script_properties");
	if (!propsNode) return;

	for (const tinyxml2::XMLElement* elem = propsNode->FirstChildElement(); elem; elem = elem->NextSiblingElement())
	{
		const char* const name = elem->Name();
		const char* const type = elem->Attribute("type");
		if (!name || !type) continue;

		auto it = m_scriptProperties.find(name);
		if (it == m_scriptProperties.end()) continue;

		const std::string_view typeStr{type};
		auto& val = it->second;

		if (typeStr == "bool" && std::holds_alternative<bool>(val))
		{
			bool v = false;
			elem->QueryBoolText(&v);
			val = v;
		}
		else if (typeStr == "number" && std::holds_alternative<double>(val))
		{
			double v = 0.0;
			elem->QueryDoubleText(&v);
			val = v;
		}
		else if (typeStr == "string" && std::holds_alternative<std::string>(val))
			val = elem->GetText() ? elem->GetText() : "";
		else
			continue;

		m_unlockedProperties.insert(name);

		if (m_script)
			m_script->SetProperty(name, val);
	}
}

void OvCore::ECS::Components::Behaviour::OnInspector(OvUI::Internal::WidgetContainer & p_root)
{
	using namespace OvCore::Helpers;

	if (!m_script)
	{
		p_root.CreateWidget<OvUI::Widgets::Texts::TextColored>("No scripting context", OvUI::Types::Color::White);
		p_root.CreateWidget<OvUI::Widgets::Layout::Dummy>();
	}
	else if (m_script->IsValid())
	{
		p_root.CreateWidget<OvUI::Widgets::Texts::TextColored>("Ready", OvUI::Types::Color::Green);
		p_root.CreateWidget<OvUI::Widgets::Texts::TextColored>("Script properties will appear below", OvUI::Types::Color::White);

		for (const auto& [fieldKey, fieldValue] : m_scriptProperties)
		{
			const bool isUnlocked = m_unlockedProperties.contains(fieldKey);

			std::visit([&, key = fieldKey, unlocked = isUnlocked]<typename T>(const T&) {
				auto getter = [this, key]() -> T {
					if (auto live = m_script->GetProperty(key))
						if (auto* val = std::get_if<T>(&*live))
							return *val;
					auto it = m_scriptProperties.find(key);
					return it != m_scriptProperties.end() ? std::get<T>(it->second) : T{};
				};
				auto setter = [this, key](T newVal) {
					m_scriptProperties[key] = newVal;
					if (m_script) m_script->SetProperty(key, std::move(newVal));
				};

				// Label row: [unlock checkbox] [field title], grouped together
				auto& labelGroup = p_root.CreateWidget<OvUI::Widgets::Layout::Group>();
				auto& unlockBox = labelGroup.CreateWidget<OvUI::Widgets::Selection::CheckBox>(unlocked);
				unlockBox.lineBreak = false;
				labelGroup.CreateWidget<OvUI::Widgets::Texts::TextColored>(key, GUIDrawer::TitleColor);

				// Input widget on the row below
				OvUI::Widgets::AWidget* inputPtr = nullptr;
				if constexpr (std::is_same_v<T, bool>)
				{
					auto& w = p_root.CreateWidget<OvUI::Widgets::Selection::CheckBox>();
					auto& d = w.template AddPlugin<OvUI::Plugins::DataDispatcher<bool>>();
					d.RegisterGatherer([getter]() { bool v = getter(); return reinterpret_cast<bool&>(v); });
					d.RegisterProvider([setter](bool v) { setter(reinterpret_cast<bool&>(v)); });
					inputPtr = &w;
				}
				else if constexpr (std::is_same_v<T, double>)
				{
					auto& w = p_root.CreateWidget<OvUI::Widgets::Drags::DragSingleScalar<double>>(
						GUIDrawer::GetDataType<double>(),
						std::numeric_limits<double>::lowest(), std::numeric_limits<double>::max(),
						static_cast<double>(0), 1.f, "", GUIDrawer::GetFormat<double>()
					);
					auto& d = w.template AddPlugin<OvUI::Plugins::DataDispatcher<double>>();
					d.RegisterGatherer(getter);
					d.RegisterProvider(setter);
					inputPtr = &w;
				}
				else
				{
					auto& w = p_root.CreateWidget<OvUI::Widgets::InputFields::InputText>("");
					auto& d = w.template AddPlugin<OvUI::Plugins::DataDispatcher<std::string>>();
					d.RegisterGatherer(getter);
					d.RegisterProvider(setter);
					inputPtr = &w;
				}

				auto& inputWidget = *inputPtr;
				inputWidget.disabled = !unlocked;

				unlockBox.ValueChangedEvent += [this, key, &inputWidget](bool checked) {
					if (checked)
					{
						m_unlockedProperties.insert(key);
					}
					else
					{
						m_unlockedProperties.erase(key);
						if (auto it = m_scriptDefaults.find(key); it != m_scriptDefaults.end())
						{
							m_scriptProperties[key] = it->second;
							if (m_script) m_script->SetProperty(key, it->second);
						}
					}
					inputWidget.disabled = !checked;
				};
			}, fieldValue);
		}
	}
	else
	{
		p_root.CreateWidget<OvUI::Widgets::Texts::TextColored>("Invalid Script", OvUI::Types::Color::Red);
		p_root.CreateWidget<OvUI::Widgets::Texts::TextColored>("Check the console for more information.", OvUI::Types::Color::White);
	}
}
