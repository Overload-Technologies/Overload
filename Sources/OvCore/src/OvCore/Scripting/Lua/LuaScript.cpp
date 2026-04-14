/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include <sol/sol.hpp>

#include <OvDebug/Logger.h>
#include <OvDebug/Assertion.h>

#include <OvCore/ECS/Actor.h>
#include <OvCore/Global/ServiceLocator.h>
#include <OvCore/ResourceManagement/MaterialManager.h>
#include <OvCore/ResourceManagement/ModelManager.h>
#include <OvCore/ResourceManagement/ShaderManager.h>
#include <OvCore/ResourceManagement/SoundManager.h>
#include <OvCore/ResourceManagement/TextureManager.h>
#include <OvCore/SceneSystem/SceneManager.h>
#include <OvCore/Scripting/Lua/LuaScript.h>

template<>
OvCore::Scripting::LuaScriptBase::TScript() = default;

template<>
OvCore::Scripting::LuaScriptBase::~TScript() = default;

template<>
bool OvCore::Scripting::LuaScriptBase::IsValid() const
{
	return m_context.table && m_context.table->valid();
}

OvCore::Scripting::LuaScript::LuaScript(sol::table table)
{
	m_context.table = std::make_unique<sol::table>(table);
}

void OvCore::Scripting::LuaScript::SetOwner(OvCore::ECS::Actor& p_owner)
{
	(*m_context.table)["owner"] = &p_owner;
}

template<>
std::map<std::string, OvCore::Scripting::ScriptPropertyValue> OvCore::Scripting::LuaScriptBase::GetDefaultProperties() const
{
	std::map<std::string, ScriptPropertyValue> properties;

	if (!IsValid()) return properties;

	m_context.table->for_each([&](const sol::object& key, const sol::object& value) {
		if (!key.is<std::string>()) return;
		const std::string keyStr = key.as<std::string>();
		if (keyStr.starts_with('_')) return;

		switch (value.get_type())
		{
			case sol::type::boolean:  properties[keyStr] = value.as<bool>();        break;
			case sol::type::number:   properties[keyStr] = value.as<double>();      break;
			case sol::type::string:   properties[keyStr] = value.as<std::string>(); break;
			case sol::type::userdata:
				if (value.is<AssetRef>())       properties[keyStr] = value.as<AssetRef>();
				else if (value.is<ActorRef>()) properties[keyStr] = value.as<ActorRef>();
				break;
			default: break;
		}
	});

	return properties;
}

template<>
std::optional<OvCore::Scripting::ScriptPropertyValue> OvCore::Scripting::LuaScriptBase::GetProperty(const std::string& p_key) const
{
	if (!IsValid()) return std::nullopt;

	const sol::object obj = (*m_context.table)[p_key];

	switch (obj.get_type())
	{
		case sol::type::boolean:  return obj.as<bool>();
		case sol::type::number:   return obj.as<double>();
		case sol::type::string:   return obj.as<std::string>();
		case sol::type::userdata:
			if (obj.is<AssetRef>())       return obj.as<AssetRef>();
			if (obj.is<ActorRef>())       return obj.as<ActorRef>();
			return std::nullopt;
		default:                  return std::nullopt;
	}
}

template<>
void OvCore::Scripting::LuaScriptBase::SetProperty(const std::string& p_key, const OvCore::Scripting::ScriptPropertyValue& p_value)
{
	if (!IsValid()) return;

	std::visit([&](auto&& v) {
		using T = std::decay_t<decltype(v)>;
		if constexpr (std::is_same_v<T, AssetRef>)
		{
			// Resolve the path to the actual loaded resource so Lua code can use
			// the value directly (e.g. materialRenderer:SetMaterial(0, self.mat)).
			using EFT = OvTools::Utils::PathParser::EFileType;
			if (v.path.empty())
			{
				(*m_context.table)[p_key] = sol::nil;
			}
			else
			{
				switch (v.fileType)
				{
				case EFT::MODEL:    (*m_context.table)[p_key] = OvCore::Global::ServiceLocator::Get<OvCore::ResourceManagement::ModelManager>()[v.path];    break;
				case EFT::TEXTURE:  (*m_context.table)[p_key] = OvCore::Global::ServiceLocator::Get<OvCore::ResourceManagement::TextureManager>()[v.path];  break;
				case EFT::SHADER:   (*m_context.table)[p_key] = OvCore::Global::ServiceLocator::Get<OvCore::ResourceManagement::ShaderManager>()[v.path];   break;
				case EFT::MATERIAL: (*m_context.table)[p_key] = OvCore::Global::ServiceLocator::Get<OvCore::ResourceManagement::MaterialManager>()[v.path]; break;
				case EFT::SOUND:    (*m_context.table)[p_key] = OvCore::Global::ServiceLocator::Get<OvCore::ResourceManagement::SoundManager>()[v.path];    break;
				default:            (*m_context.table)[p_key] = sol::nil; break;
				}
			}
		}
		else if constexpr (std::is_same_v<T, ActorRef>)
		{
			// Resolve the GUID to the actual actor so Lua code can use the actor directly.
			if (v.guid == 0)
			{
				(*m_context.table)[p_key] = sol::nil;
			}
			else
			{
				auto* scene = OvCore::Global::ServiceLocator::Get<OvCore::SceneSystem::SceneManager>().GetCurrentScene();
				auto* actor = scene ? scene->FindActorByGUID(v.guid) : nullptr;
				if (actor)
					(*m_context.table)[p_key] = actor;
				else
					(*m_context.table)[p_key] = sol::nil;
			}
		}
		else
		{
			(*m_context.table)[p_key] = v;
		}
	}, p_value);
}
