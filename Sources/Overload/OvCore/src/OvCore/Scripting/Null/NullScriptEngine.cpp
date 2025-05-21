/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include <filesystem>

#include <OvCore/Scripting/Null/NullScriptEngine.h>

template<>
OvCore::Scripting::NullScriptEngineBase::TScriptEngine() {}

template<>
OvCore::Scripting::NullScriptEngineBase::~TScriptEngine() {}

template<>
void OvCore::Scripting::NullScriptEngineBase::SetScriptRootFolder(const std::filesystem::path& p_scriptRootFolder) {}

template<>
std::vector<std::string> OvCore::Scripting::NullScriptEngineBase::GetValidExtensions()
{
	return { GetDefaultExtension() };
}

template<>
std::string OvCore::Scripting::NullScriptEngineBase::GetDefaultScriptContent(const std::string& p_name)
{
	return "class " + p_name + " {\n}";
}

template<>
std::string OvCore::Scripting::NullScriptEngineBase::GetDefaultExtension()
{
	return ".ovscript";
}

template<>
void OvCore::Scripting::NullScriptEngineBase::AddBehaviour(OvCore::ECS::Components::Behaviour& p_toAdd)
{
}

template<>
void OvCore::Scripting::NullScriptEngineBase::RemoveBehaviour(OvCore::ECS::Components::Behaviour& p_toRemove)
{
}

template<>
void OvCore::Scripting::NullScriptEngineBase::Reload()
{
}

template<>
bool OvCore::Scripting::NullScriptEngineBase::IsOk() const
{
	return true;
}

template<>
void OvCore::Scripting::NullScriptEngineBase::OnAwake(OvCore::ECS::Components::Behaviour& p_target)
{
}

template<>
void OvCore::Scripting::NullScriptEngineBase::OnStart(OvCore::ECS::Components::Behaviour& p_target)
{
}

template<>
void OvCore::Scripting::NullScriptEngineBase::OnEnable(OvCore::ECS::Components::Behaviour& p_target)
{
}

template<>
void OvCore::Scripting::NullScriptEngineBase::OnDisable(OvCore::ECS::Components::Behaviour& p_target)
{
}

template<>
void OvCore::Scripting::NullScriptEngineBase::OnDestroy(OvCore::ECS::Components::Behaviour& p_target)
{
}

template<>
void OvCore::Scripting::NullScriptEngineBase::OnUpdate(OvCore::ECS::Components::Behaviour& p_target, float p_deltaTime)
{
}

template<>
void OvCore::Scripting::NullScriptEngineBase::OnFixedUpdate(OvCore::ECS::Components::Behaviour& p_target, float p_deltaTime)
{
}

template<>
void OvCore::Scripting::NullScriptEngineBase::OnLateUpdate(OvCore::ECS::Components::Behaviour& p_target, float p_deltaTime)
{
}

template<>
void OvCore::Scripting::NullScriptEngineBase::OnCollisionEnter(OvCore::ECS::Components::Behaviour& p_target, OvCore::ECS::Components::CPhysicalObject& p_otherObject)
{
}

template<>
void OvCore::Scripting::NullScriptEngineBase::OnCollisionStay(OvCore::ECS::Components::Behaviour& p_target, OvCore::ECS::Components::CPhysicalObject& p_otherObject)
{
}

template<>
void OvCore::Scripting::NullScriptEngineBase::OnCollisionExit(OvCore::ECS::Components::Behaviour& p_target, OvCore::ECS::Components::CPhysicalObject& p_otherObject)
{
}

template<>
void OvCore::Scripting::NullScriptEngineBase::OnTriggerEnter(OvCore::ECS::Components::Behaviour& p_target, OvCore::ECS::Components::CPhysicalObject& p_otherObject)
{
}

template<>
void OvCore::Scripting::NullScriptEngineBase::OnTriggerStay(OvCore::ECS::Components::Behaviour& p_target, OvCore::ECS::Components::CPhysicalObject& p_otherObject)
{
}

template<>
void OvCore::Scripting::NullScriptEngineBase::OnTriggerExit(OvCore::ECS::Components::Behaviour& p_target, OvCore::ECS::Components::CPhysicalObject& p_otherObject)
{
}
