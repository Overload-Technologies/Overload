/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include <tinyxml2.h>

#include <OvCore/ECS/Actor.h>
#include <OvCore/ECS/Components/Behaviour.h>
#include <OvCore/Global/ServiceLocator.h>
#include <OvCore/Helpers/GUIDrawer.h>
#include <OvCore/Scripting/ScriptEngine.h>

#include <OvDebug/Logger.h>

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
		return;
	}

	// Collect fresh defaults from the script, then merge with any same-type user overrides.
	auto defaults = m_script->GetDefaultProperties();
	std::map<std::string, Scripting::ScriptPropertyValue> newProperties;

	for (auto& [key, defaultVal] : defaults)
	{
		const auto it = m_scriptProperties.find(key);
		if (it != m_scriptProperties.end() && it->second.index() == defaultVal.index())
			newProperties[key] = it->second;
		else
			newProperties[key] = defaultVal;
	}

	m_scriptProperties = std::move(newProperties);

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
	if (m_scriptProperties.empty()) return;

	tinyxml2::XMLNode* propsNode = p_doc.NewElement("script_properties");
	p_node->InsertEndChild(propsNode);

	for (const auto& [fieldKey, fieldValue] : m_scriptProperties)
	{
		tinyxml2::XMLElement* elem = p_doc.NewElement(fieldKey.c_str());

		std::visit([elem](auto&& v) {
			using T = std::decay_t<decltype(v)>;
			if constexpr (std::is_same_v<T, bool>)
			{
				elem->SetAttribute("type", "bool");
				elem->SetText(v);
			}
			else if constexpr (std::is_same_v<T, double>)
			{
				elem->SetAttribute("type", "number");
				elem->SetText(v);
			}
			else if constexpr (std::is_same_v<T, std::string>)
			{
				elem->SetAttribute("type", "string");
				elem->SetText(v.c_str());
			}
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
		const char* name = elem->Name();
		const char* type = elem->Attribute("type");
		if (!name || !type) continue;

		const std::string nameStr(name);
		const std::string typeStr(type);

		// Only apply the loaded value when the key exists in m_scriptProperties (populated by
		// SetScript) AND the type matches, to guard against stale or type-changed fields.
		auto it = m_scriptProperties.find(nameStr);
		if (it == m_scriptProperties.end()) continue;

		if (typeStr == "bool" && std::holds_alternative<bool>(it->second))
		{
			bool val = false;
			elem->QueryBoolText(&val);
			it->second = val;
		}
		else if (typeStr == "number" && std::holds_alternative<double>(it->second))
		{
			double val = 0.0;
			elem->QueryDoubleText(&val);
			it->second = val;
		}
		else if (typeStr == "string" && std::holds_alternative<std::string>(it->second))
		{
			it->second = std::string(elem->GetText() ? elem->GetText() : "");
		}
		else
		{
			continue;
		}

		if (m_script)
			m_script->SetProperty(nameStr, it->second);
	}
}

void OvCore::ECS::Components::Behaviour::OnInspector(OvUI::Internal::WidgetContainer & p_root)
{
	using namespace OvCore::Helpers;

	if (!m_script)
	{
		p_root.CreateWidget<OvUI::Widgets::Texts::TextColored>("No scripting context", OvUI::Types::Color::White);
	}
	else if (m_script->IsValid())
	{
		p_root.CreateWidget<OvUI::Widgets::Texts::TextColored>("Ready", OvUI::Types::Color::Green);

		for (const auto& [fieldKey, fieldValue] : m_scriptProperties)
		{
			std::visit([&, key = fieldKey](auto&& v) {
				using T = std::decay_t<decltype(v)>;

				if constexpr (std::is_same_v<T, bool>)
				{
					GUIDrawer::DrawBoolean(p_root, key,
						[this, key]() -> bool {
							if (m_script)
								if (auto liveVal = m_script->GetProperty(key))
									if (auto* b = std::get_if<bool>(&*liveVal))
										return *b;
							auto it = m_scriptProperties.find(key);
							return it != m_scriptProperties.end() ? std::get<bool>(it->second) : false;
						},
						[this, key](bool newVal) {
							m_scriptProperties[key] = newVal;
							if (m_script) m_script->SetProperty(key, newVal);
						}
					);
				}
				else if constexpr (std::is_same_v<T, double>)
				{
					GUIDrawer::DrawScalar<double>(p_root, key,
						[this, key]() -> double {
							if (m_script)
								if (auto liveVal = m_script->GetProperty(key))
									if (auto* d = std::get_if<double>(&*liveVal))
										return *d;
							auto it = m_scriptProperties.find(key);
							return it != m_scriptProperties.end() ? std::get<double>(it->second) : 0.0;
						},
						[this, key](double newVal) {
							m_scriptProperties[key] = newVal;
							if (m_script) m_script->SetProperty(key, newVal);
						}
					);
				}
				else if constexpr (std::is_same_v<T, std::string>)
				{
					GUIDrawer::DrawString(p_root, key,
						[this, key]() -> std::string {
							if (m_script)
								if (auto liveVal = m_script->GetProperty(key))
									if (auto* s = std::get_if<std::string>(&*liveVal))
										return *s;
							auto it = m_scriptProperties.find(key);
							return it != m_scriptProperties.end() ? std::get<std::string>(it->second) : "";
						},
						[this, key](std::string newVal) {
							m_scriptProperties[key] = newVal;
							if (m_script) m_script->SetProperty(key, std::move(newVal));
						}
					);
				}
			}, fieldValue);
		}
	}
	else
	{
		p_root.CreateWidget<OvUI::Widgets::Texts::TextColored>("Invalid Script", OvUI::Types::Color::Red);
		p_root.CreateWidget<OvUI::Widgets::Texts::TextColored>("Check the console for more information.", OvUI::Types::Color::White);
	}
}
