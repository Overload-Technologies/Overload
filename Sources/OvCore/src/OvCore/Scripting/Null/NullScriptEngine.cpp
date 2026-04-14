/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include <filesystem>

#include <OvCore/Scripting/Null/NullScriptEngine.h>
#include <unordered_set>

template<>
OvCore::Scripting::NullScriptEngineBase::TScriptEngine(
	const std::filesystem::path& p_projectAssetsPath,
	const std::filesystem::path& p_engineAssetsPath
) {}

template<>
OvCore::Scripting::NullScriptEngineBase::~TScriptEngine() {}

template<>
bool OvCore::Scripting::NullScriptEngineBase::CreateProjectFiles(const std::filesystem::path& p_projectFolder, bool p_force) { return true; }

template<>
std::string OvCore::Scripting::NullScriptEngineBase::GetDefaultExtension()
{
	return ".ovscript";
}

template<>
std::unordered_set<std::string> OvCore::Scripting::NullScriptEngineBase::GetValidExtensions()
{
	return { GetDefaultExtension() };
}

template<>
std::string OvCore::Scripting::NullScriptEngineBase::GetDefaultScriptContent(const std::string& p_name)
{
	return "class " + p_name + " {\n}";
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
