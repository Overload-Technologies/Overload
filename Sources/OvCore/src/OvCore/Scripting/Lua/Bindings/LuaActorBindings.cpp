/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include <algorithm>
#include <array>
#include <filesystem>
#include <string_view>

#include <sol/sol.hpp>

#include <OvCore/ECS/Actor.h>  
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
#include <OvCore/Scripting/Lua/LuaScriptEngine.h>

namespace
{
	template<typename TComponent>
	sol::object GetComponentByType(sol::state_view p_luaState, OvCore::ECS::Actor& p_actor)
	{
		if (auto component = p_actor.GetComponent<TComponent>())
		{
			return sol::make_object(p_luaState, component);
		}

		return sol::make_object(p_luaState, sol::nil);
	}

	using ComponentResolver = sol::object(*)(sol::state_view, OvCore::ECS::Actor&);

	struct ComponentResolverEntry
	{
		std::string_view type;
		ComponentResolver resolver;
	};

	template<typename TComponent>
	constexpr ComponentResolverEntry MakeComponentResolverEntry(const std::string_view p_type)
	{
		return { p_type, &GetComponentByType<TComponent> };
	}

	sol::object ResolveComponentByType(sol::this_state p_luaState, OvCore::ECS::Actor& p_actor, const std::string& p_type)
	{
		using namespace OvCore::ECS::Components;

		static const auto kComponentResolvers = std::to_array<ComponentResolverEntry>({
			MakeComponentResolverEntry<CTransform>("Transform"),
			MakeComponentResolverEntry<CPhysicalObject>("PhysicalObject"),
			MakeComponentResolverEntry<CPhysicalBox>("PhysicalBox"),
			MakeComponentResolverEntry<CPhysicalSphere>("PhysicalSphere"),
			MakeComponentResolverEntry<CPhysicalCapsule>("PhysicalCapsule"),
			MakeComponentResolverEntry<CCamera>("Camera"),
			MakeComponentResolverEntry<CLight>("Light"),
			MakeComponentResolverEntry<CPointLight>("PointLight"),
			MakeComponentResolverEntry<CSpotLight>("SpotLight"),
			MakeComponentResolverEntry<CDirectionalLight>("DirectionalLight"),
			MakeComponentResolverEntry<CAmbientBoxLight>("AmbientBoxLight"),
			MakeComponentResolverEntry<CAmbientSphereLight>("AmbientSphereLight"),
			MakeComponentResolverEntry<CModelRenderer>("ModelRenderer"),
			MakeComponentResolverEntry<CMaterialRenderer>("MaterialRenderer"),
			MakeComponentResolverEntry<CSkinnedMeshRenderer>("SkinnedMeshRenderer"),
			MakeComponentResolverEntry<CAudioSource>("AudioSource"),
			MakeComponentResolverEntry<CAudioListener>("AudioListener"),
			MakeComponentResolverEntry<CPostProcessStack>("PostProcessStack"),
			MakeComponentResolverEntry<CReflectionProbe>("ReflectionProbe")
		});

		sol::state_view lua(p_luaState);

		if (auto resolver = std::find_if(
			kComponentResolvers.begin(),
			kComponentResolvers.end(),
			[&p_type](const ComponentResolverEntry& p_entry)
			{
				return p_entry.type == p_type;
			}
		); resolver != kComponentResolvers.end())
		{
			return resolver->resolver(lua, p_actor);
		}

		return sol::make_object(lua, sol::nil);
	}
}

void BindLuaActor(sol::state& p_luaState)
{
	using namespace OvCore::ECS;
	using namespace OvCore::ECS::Components;

	p_luaState.new_usertype<Actor>("Actor",
		/* Methods */
		"GetName", &Actor::GetName,
		"SetName", &Actor::SetName,
		"GetTag", &Actor::GetTag,
		"GetChildren", &Actor::GetChildren,
		"SetTag", &Actor::SetTag,
		"GetID", &Actor::GetID,
		"GetParent", &Actor::GetParent,
		"SetParent", &Actor::SetParent,
		"DetachFromParent", &Actor::DetachFromParent,
		"Destroy", &Actor::MarkAsDestroy,
		"IsSelfActive", &Actor::IsSelfActive,
		"IsActive", &Actor::IsActive,
		"SetActive", &Actor::SetActive,
		"IsDescendantOf", &Actor::IsDescendantOf,

		/* Components Getters */
		"GetTransform", &Actor::GetComponent<CTransform>,
		"GetPhysicalObject", &Actor::GetComponent<CPhysicalObject>,
		"GetPhysicalBox", &Actor::GetComponent<CPhysicalBox>,
		"GetPhysicalSphere", &Actor::GetComponent<CPhysicalSphere>,
		"GetPhysicalCapsule", &Actor::GetComponent<CPhysicalCapsule>,
		"GetCamera", &Actor::GetComponent<CCamera>,
		"GetLight", &Actor::GetComponent<CLight>,
		"GetPointLight", &Actor::GetComponent<CPointLight>,
		"GetSpotLight", &Actor::GetComponent<CSpotLight>,
		"GetDirectionalLight", &Actor::GetComponent<CDirectionalLight>,
		"GetAmbientBoxLight", &Actor::GetComponent<CAmbientBoxLight>,
		"GetAmbientSphereLight", &Actor::GetComponent<CAmbientSphereLight>,
		"GetModelRenderer", &Actor::GetComponent<CModelRenderer>,
		"GetMaterialRenderer", &Actor::GetComponent<CMaterialRenderer>,
		"GetSkinnedMeshRenderer", &Actor::GetComponent<CSkinnedMeshRenderer>,
		"GetAudioSource", &Actor::GetComponent<CAudioSource>,
		"GetAudioListener", &Actor::GetComponent<CAudioListener>,
		"GetPostProcessStack", & Actor::GetComponent<CPostProcessStack>,
		"GetReflectionProbe", &Actor::GetComponent<CReflectionProbe>,
		"GetComponent", [](sol::this_state p_luaState, Actor& p_this, const std::string& p_type) -> sol::object {
			return ResolveComponentByType(p_luaState, p_this, p_type);
		},

		/* Behaviours relatives */
		"GetBehaviour", [](Actor& p_this, const std::string& p_type) -> sol::table {
			// First try matching by script name (stem without path or extension)
			OvCore::ECS::Components::Behaviour* behaviour = nullptr;
			for (auto& [key, b] : p_this.GetBehaviours())
			{
				if (std::filesystem::path(b.name).stem().string() == p_type)
				{
					behaviour = &b;
					break;
				}
			}

			// Fall back to path-based match: try as-is, then with .lua appended if no extension given
			if (!behaviour)
			{
				behaviour = p_this.GetBehaviour(p_type);
			}

			if (!behaviour && std::filesystem::path(p_type).extension().empty())
			{
				behaviour = p_this.GetBehaviour(p_type + ".lua");
			}

			if (behaviour)
			{
				if (auto script = behaviour->GetScript())
				{
					return *static_cast<OvCore::Scripting::LuaScript&>(script.value()).GetContext().table;
				}
			}
			return sol::nil;
		},

		/* Components Creators */
		"AddTransform", &Actor::AddComponent<CTransform>,
		"AddModelRenderer", &Actor::AddComponent<CModelRenderer>,
		"AddPhysicalBox", &Actor::AddComponent<CPhysicalBox>,
		"AddPhysicalSphere", &Actor::AddComponent<CPhysicalSphere>,
		"AddPhysicalCapsule", &Actor::AddComponent<CPhysicalCapsule>,
		"AddCamera", &Actor::AddComponent<CCamera>,
		"AddPointLight", &Actor::AddComponent<CPointLight>,
		"AddSpotLight", &Actor::AddComponent<CSpotLight>,
		"AddDirectionalLight", &Actor::AddComponent<CDirectionalLight>,
		"AddAmbientBoxLight", &Actor::AddComponent<CAmbientBoxLight>,
		"AddAmbientSphereLight", &Actor::AddComponent<CAmbientSphereLight>,
		"AddMaterialRenderer", &Actor::AddComponent<CMaterialRenderer>,
		"AddSkinnedMeshRenderer", &Actor::AddComponent<CSkinnedMeshRenderer>,
		"AddAudioSource", &Actor::AddComponent<CAudioSource>,
		"AddAudioListener", &Actor::AddComponent<CAudioListener>,
		"AddPostProcessStack", & Actor::AddComponent<CPostProcessStack>,
		"AddReflectionProbe", &Actor::AddComponent<CReflectionProbe>,

		/* Components Destructors */
		"RemoveModelRenderer", &Actor::RemoveComponent<CModelRenderer>,
		"RemovePhysicalBox", &Actor::RemoveComponent<CPhysicalBox>,
		"RemovePhysicalSphere", &Actor::RemoveComponent<CPhysicalSphere>,
		"RemovePhysicalCapsule", &Actor::RemoveComponent<CPhysicalCapsule>,
		"RemoveCamera", &Actor::RemoveComponent<CCamera>,
		"RemovePointLight", &Actor::RemoveComponent<CPointLight>,
		"RemoveSpotLight", &Actor::RemoveComponent<CSpotLight>,
		"RemoveDirectionalLight", &Actor::RemoveComponent<CDirectionalLight>,
		"RemoveAmbientBoxLight", &Actor::RemoveComponent<CAmbientBoxLight>,
		"RemoveAmbientSphereLight", &Actor::RemoveComponent<CAmbientSphereLight>,
		"RemoveMaterialRenderer", &Actor::RemoveComponent<CMaterialRenderer>,
		"RemoveSkinnedMeshRenderer", &Actor::RemoveComponent<CSkinnedMeshRenderer>,
		"RemoveAudioSource", &Actor::RemoveComponent<CAudioSource>,
		"RemoveAudioListener", &Actor::RemoveComponent<CAudioListener>,
		"RemovePostProcessStack", & Actor::RemoveComponent<CPostProcessStack>,
		"RemoveReflectionProbe", &Actor::RemoveComponent<CReflectionProbe>,

		/* Behaviour management */
		"AddBehaviour", &Actor::AddBehaviour,
		"RemoveBehaviour", sol::overload(
			sol::resolve<bool(Behaviour&)>(&Actor::RemoveBehaviour),
			sol::resolve<bool(const std::string&)>(&Actor::RemoveBehaviour)
		)
	);
}
