/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include <OvCore/ECS/Actor.h>
#include <OvCore/ECS/Components/Behaviour.h>
#include <OvCore/Global/ServiceLocator.h>
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

void OvCore::ECS::Components::Behaviour::SetScript(std::unique_ptr<Scripting::Script>&& p_scriptContext)
{
	m_script = std::move(p_scriptContext);
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
}

void OvCore::ECS::Components::Behaviour::OnDeserialize(tinyxml2::XMLDocument & p_doc, tinyxml2::XMLNode * p_node)
{
}

void OvCore::ECS::Components::Behaviour::OnInspector(OvUI::Internal::WidgetContainer & p_root)
{
	using namespace OvMaths;
	using namespace OvCore::Helpers;

	if (!m_script)
	{
		p_root.CreateWidget<OvUI::Widgets::Texts::TextColored>("No scripting context", OvUI::Types::Color::White);
	}
	else if (m_script && m_script->IsValid())
	{
		p_root.CreateWidget<OvUI::Widgets::Texts::TextColored>("Ready", OvUI::Types::Color::Green);
		p_root.CreateWidget<OvUI::Widgets::Texts::TextColored>("Your script will execute in play mode.", OvUI::Types::Color::White);
	}
	else
	{
		p_root.CreateWidget<OvUI::Widgets::Texts::TextColored>("Invalid Script", OvUI::Types::Color::Red);
		p_root.CreateWidget<OvUI::Widgets::Texts::TextColored>("Check the console for more information.", OvUI::Types::Color::White);
	}
}
