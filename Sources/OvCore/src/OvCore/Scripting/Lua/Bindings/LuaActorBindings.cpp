/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include <filesystem>
#include <format>

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
#include <OvCore/Scripting/Common/ScriptPropertyValue.h>
#include <OvCore/Scripting/Lua/LuaScriptEngine.h>


// ActorRef is a GUID-based handle, not a raw Actor pointer. Lua holds the
// handle; each method resolves it through the current scene via
// Scene::FindActorByGUID. Methods that need a live actor call Resolve(),
// which raises a Lua error if the actor has been destroyed. Inspection
// methods (IsAlive, GetGUID) read the handle directly so a script can
// still examine a dead handle without erroring.
// 
// 
void BindLuaActor(sol::state& p_luaState)
{
	using namespace OvCore::ECS;
	using namespace OvCore::ECS::Components;
	using namespace OvCore::Scripting;

	p_luaState.new_usertype<ActorRef>("Actor",
		/* Methods */
		"GetName", [](ActorRef& r) -> const std::string& { return r.Resolve().GetName(); },
		"SetName", [](ActorRef& r, const std::string& p_name) { r.Resolve().SetName(p_name); },
		"GetTag", [](ActorRef& r) -> const std::string& { return r.Resolve().GetTag(); },
		"GetChildren", [](ActorRef& r) {
			std::vector<ActorRef> result;
			for (auto* child : r.Resolve().GetChildren())
				result.push_back(ActorRef{child->GetGUID()});
			return result;
		},
		"FindChild", [](ActorRef& r, const std::string& p_name, bool p_recursive) -> ActorRef {
			auto* child = r.Resolve().FindChild(p_name, p_recursive);
			return child ? ActorRef{child->GetGUID()} : ActorRef{0};
		},
		"SetTag", [](ActorRef& r, const std::string& p_tag) { r.Resolve().SetTag(p_tag); },
		"GetID", [](ActorRef& r) { return r.Resolve().GetID(); },
		"GetGUID", [](ActorRef& r) { return std::format("{:016X}", r.guid); }, // no Resolve(); reads the handle directly
		"GetParent", [](ActorRef& r) -> ActorRef {
			auto* parent = r.Resolve().GetParent();
			return parent ? ActorRef{parent->GetGUID()} : ActorRef{0};
		},
		"SetParent", [](ActorRef& r, ActorRef& p_parent) { r.Resolve().SetParent(p_parent.Resolve()); },
		"DetachFromParent", [](ActorRef& r) { r.Resolve().DetachFromParent(); },
		"Destroy", [](ActorRef& r) { r.Resolve().MarkAsDestroy(); },
		"IsSelfActive", [](ActorRef& r) { return r.Resolve().IsSelfActive(); },
		"IsActive", [](ActorRef& r) { return r.Resolve().IsActive(); },
		"SetActive", [](ActorRef& r, bool p_active) { r.Resolve().SetActive(p_active); },
		"IsDescendantOf", [](ActorRef& r, ActorRef& p_other) { return r.Resolve().IsDescendantOf(&p_other.Resolve()); },
		"IsAlive", [](ActorRef& r) { return r.IsAlive(); }, // no Resolve(); reads the handle directly

		/* Components Getters */
		"GetTransform", [](ActorRef& r) { return r.Resolve().GetComponent<CTransform>(); },
		"GetPhysicalObject", [](ActorRef& r) { return r.Resolve().GetComponent<CPhysicalObject>(); },
		"GetPhysicalBox", [](ActorRef& r) { return r.Resolve().GetComponent<CPhysicalBox>(); },
		"GetPhysicalSphere", [](ActorRef& r) { return r.Resolve().GetComponent<CPhysicalSphere>(); },
		"GetPhysicalCapsule", [](ActorRef& r) { return r.Resolve().GetComponent<CPhysicalCapsule>(); },
		"GetCamera", [](ActorRef& r) { return r.Resolve().GetComponent<CCamera>(); },
		"GetLight", [](ActorRef& r) { return r.Resolve().GetComponent<CLight>(); },
		"GetPointLight", [](ActorRef& r) { return r.Resolve().GetComponent<CPointLight>(); },
		"GetSpotLight", [](ActorRef& r) { return r.Resolve().GetComponent<CSpotLight>(); },
		"GetDirectionalLight", [](ActorRef& r) { return r.Resolve().GetComponent<CDirectionalLight>(); },
		"GetAmbientBoxLight", [](ActorRef& r) { return r.Resolve().GetComponent<CAmbientBoxLight>(); },
		"GetAmbientSphereLight", [](ActorRef& r) { return r.Resolve().GetComponent<CAmbientSphereLight>(); },
		"GetModelRenderer", [](ActorRef& r) { return r.Resolve().GetComponent<CModelRenderer>(); },
		"GetMaterialRenderer", [](ActorRef& r) { return r.Resolve().GetComponent<CMaterialRenderer>(); },
		"GetSkinnedMeshRenderer", [](ActorRef& r) { return r.Resolve().GetComponent<CSkinnedMeshRenderer>(); },
		"GetAudioSource", [](ActorRef& r) { return r.Resolve().GetComponent<CAudioSource>(); },
		"GetAudioListener", [](ActorRef& r) { return r.Resolve().GetComponent<CAudioListener>(); },
		"GetPostProcessStack", [](ActorRef& r) { return r.Resolve().GetComponent<CPostProcessStack>(); },
		"GetReflectionProbe", [](ActorRef& r) { return r.Resolve().GetComponent<CReflectionProbe>(); },

		/* Behaviours relatives */
		"GetBehaviour", [](ActorRef& r, const std::string& p_name) -> sol::table {
			auto& p_this = r.Resolve();
			// First try matching by script name (stem without path or extension)
			OvCore::ECS::Components::Behaviour* behaviour = nullptr;
			for (auto& [key, b] : p_this.GetBehaviours())
			{
				if (std::filesystem::path(b.name).stem().string() == p_name)
				{
					behaviour = &b;
					break;
				}
			}

			// Fall back to path-based match: try as-is, then with .lua appended if no extension given
			if (!behaviour)
			{
				behaviour = p_this.GetBehaviour(p_name);
			}

			if (!behaviour && std::filesystem::path(p_name).extension().empty())
			{
				behaviour = p_this.GetBehaviour(p_name + ".lua");
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
		"AddTransform", [](ActorRef& r) -> CTransform& { return r.Resolve().AddComponent<CTransform>(); },
		"AddModelRenderer", [](ActorRef& r) -> CModelRenderer& { return r.Resolve().AddComponent<CModelRenderer>(); },
		"AddPhysicalBox", [](ActorRef& r) -> CPhysicalBox& { return r.Resolve().AddComponent<CPhysicalBox>(); },
		"AddPhysicalSphere", [](ActorRef& r) -> CPhysicalSphere& { return r.Resolve().AddComponent<CPhysicalSphere>(); },
		"AddPhysicalCapsule", [](ActorRef& r) -> CPhysicalCapsule& { return r.Resolve().AddComponent<CPhysicalCapsule>(); },
		"AddCamera", [](ActorRef& r) -> CCamera& { return r.Resolve().AddComponent<CCamera>(); },
		"AddPointLight", [](ActorRef& r) -> CPointLight& { return r.Resolve().AddComponent<CPointLight>(); },
		"AddSpotLight", [](ActorRef& r) -> CSpotLight& { return r.Resolve().AddComponent<CSpotLight>(); },
		"AddDirectionalLight", [](ActorRef& r) -> CDirectionalLight& { return r.Resolve().AddComponent<CDirectionalLight>(); },
		"AddAmbientBoxLight", [](ActorRef& r) -> CAmbientBoxLight& { return r.Resolve().AddComponent<CAmbientBoxLight>(); },
		"AddAmbientSphereLight", [](ActorRef& r) -> CAmbientSphereLight& { return r.Resolve().AddComponent<CAmbientSphereLight>(); },
		"AddMaterialRenderer", [](ActorRef& r) -> CMaterialRenderer& { return r.Resolve().AddComponent<CMaterialRenderer>(); },
		"AddSkinnedMeshRenderer", [](ActorRef& r) -> CSkinnedMeshRenderer& { return r.Resolve().AddComponent<CSkinnedMeshRenderer>(); },
		"AddAudioSource", [](ActorRef& r) -> CAudioSource& { return r.Resolve().AddComponent<CAudioSource>(); },
		"AddAudioListener", [](ActorRef& r) -> CAudioListener& { return r.Resolve().AddComponent<CAudioListener>(); },
		"AddPostProcessStack", [](ActorRef& r) -> CPostProcessStack& { return r.Resolve().AddComponent<CPostProcessStack>(); },
		"AddReflectionProbe", [](ActorRef& r) -> CReflectionProbe& { return r.Resolve().AddComponent<CReflectionProbe>(); },

		/* Components Destructors */
		"RemoveModelRenderer", [](ActorRef& r) { return r.Resolve().RemoveComponent<CModelRenderer>(); },
		"RemovePhysicalBox", [](ActorRef& r) { return r.Resolve().RemoveComponent<CPhysicalBox>(); },
		"RemovePhysicalSphere", [](ActorRef& r) { return r.Resolve().RemoveComponent<CPhysicalSphere>(); },
		"RemovePhysicalCapsule", [](ActorRef& r) { return r.Resolve().RemoveComponent<CPhysicalCapsule>(); },
		"RemoveCamera", [](ActorRef& r) { return r.Resolve().RemoveComponent<CCamera>(); },
		"RemovePointLight", [](ActorRef& r) { return r.Resolve().RemoveComponent<CPointLight>(); },
		"RemoveSpotLight", [](ActorRef& r) { return r.Resolve().RemoveComponent<CSpotLight>(); },
		"RemoveDirectionalLight", [](ActorRef& r) { return r.Resolve().RemoveComponent<CDirectionalLight>(); },
		"RemoveAmbientBoxLight", [](ActorRef& r) { return r.Resolve().RemoveComponent<CAmbientBoxLight>(); },
		"RemoveAmbientSphereLight", [](ActorRef& r) { return r.Resolve().RemoveComponent<CAmbientSphereLight>(); },
		"RemoveMaterialRenderer", [](ActorRef& r) { return r.Resolve().RemoveComponent<CMaterialRenderer>(); },
		"RemoveSkinnedMeshRenderer", [](ActorRef& r) { return r.Resolve().RemoveComponent<CSkinnedMeshRenderer>(); },
		"RemoveAudioSource", [](ActorRef& r) { return r.Resolve().RemoveComponent<CAudioSource>(); },
		"RemoveAudioListener", [](ActorRef& r) { return r.Resolve().RemoveComponent<CAudioListener>(); },
		"RemovePostProcessStack", [](ActorRef& r) { return r.Resolve().RemoveComponent<CPostProcessStack>(); },
		"RemoveReflectionProbe", [](ActorRef& r) { return r.Resolve().RemoveComponent<CReflectionProbe>(); },

		/* Behaviour management */
		"AddBehaviour", [](ActorRef& r, const std::string& p_name) -> Behaviour& { return r.Resolve().AddBehaviour(p_name); },
		"RemoveBehaviour", sol::overload(
			[](ActorRef& r, Behaviour& p_behaviour) { return r.Resolve().RemoveBehaviour(p_behaviour); },
			[](ActorRef& r, const std::string& p_name) { return r.Resolve().RemoveBehaviour(p_name); }
		)
	);
}
