---@meta

--- The Actor is the main class of the ECS, it corresponds to the entity and is composed of components and behaviours (scripts)
---@class Actor
Actor = {}

--- Returns the name of the actor
---@return string
function Actor:GetName() end

--- Defines a name for the actor
---@param name string
function Actor:SetName(name) end

--- Returns the tag of the actor
---@return string
function Actor:GetTag() end

--- Defines a tag for the actor
---@param tag string
function Actor:SetTag(tag) end

--- Returns the ID of this actor
---@return integer
function Actor:GetID() end

--- Returns children of this actor
---@return Actor[]
function Actor:GetChildren() end

--- Returns the parent (or nil) of this actor
---@return Actor|nil
function Actor:GetParent() end

--- Defines a new parent to this actor
---@param parent Actor
function Actor:SetParent(parent) end

--- Detaches the actor from its parent
function Actor:DetachFromParent() end

--- Removes the actor from the scene
function Actor:Destroy() end

--- Returns true if the actor is active, ignoring his parent (if any) active state
---@return boolean
function Actor:IsSelfActive() end

--- Returns true if the actor is active
---@return boolean
function Actor:IsActive() end

--- Defines if the actor is active
---@param active boolean
function Actor:SetActive(active) end

--- Returns true if the otherActor is the topmost parent of the instance in the hierarchy
---@param potentialAncestor Actor
---@return boolean
function Actor:IsDescendantOf(potentialAncestor) end

--- Returns the Transform attached to this actor
---@return Transform|nil
function Actor:GetTransform() end

--- Returns the PhysicalObject attached to this actor (If any)
---@return PhysicalObject|nil
function Actor:GetPhysicalObject() end

--- Returns the PhysicalBox attached to this actor (If any)
---@return PhysicalBox|nil
function Actor:GetPhysicalBox() end

--- Returns the PhysicalSphere attached to this actor (If any)
---@return PhysicalSphere|nil
function Actor:GetPhysicalSphere() end

--- Returns the PhysicalCapsule attached to this actor (If any)
---@return PhysicalCapsule|nil
function Actor:GetPhysicalCapsule() end

--- Returns the Camera attached to this actor (If any)
---@return Camera|nil
function Actor:GetCamera() end

--- Returns the Light attached to this actor (If any)
---@return Light|nil
function Actor:GetLight() end

--- Returns the PointLight attached to this actor (If any)
---@return PointLight|nil
function Actor:GetPointLight() end

--- Returns the SpotLight attached to this actor (If any)
---@return SpotLight|nil
function Actor:GetSpotLight() end

--- Returns the DirectionalLight attached to this actor (If any)
---@return DirectionalLight|nil
function Actor:GetDirectionalLight() end

--- Returns the AmbientBoxLight attached to this actor (If any)
---@return AmbientBoxLight|nil
function Actor:GetAmbientBoxLight() end

--- Returns the AmbientSphereLight attached to this actor (If any)
---@return AmbientSphereLight|nil
function Actor:GetAmbientSphereLight() end

--- Returns the ModelRenderer attached to this actor (If any)
---@return ModelRenderer|nil
function Actor:GetModelRenderer() end

--- Returns the MaterialRenderer attached to this actor (If any)
---@return MaterialRenderer|nil
function Actor:GetMaterialRenderer() end

--- Returns the SkinnedMeshRenderer attached to this actor (If any)
---@return SkinnedMeshRenderer|nil
function Actor:GetSkinnedMeshRenderer() end

--- Returns the AudioSource attached to this actor (If any)
---@return AudioSource|nil
function Actor:GetAudioSource() end

--- Returns the AudioListener attached to this actor (If any)
---@return AudioListener|nil
function Actor:GetAudioListener() end

--- Returns the PostProcessStack attached to this actor (If any)
---@return PostProcessStack|nil
function Actor:GetPostProcessStack() end

--- Returns the ReflectionProbe attached to this actor (If any)
---@return ReflectionProbe|nil
function Actor:GetReflectionProbe() end

--- Returns the Behaviour of the given type attached to this actor (If any)
---@param name string
---@return table|nil
function Actor:GetBehaviour(name) end

--- Adds a Transform component to the actor and returns it
---@return Transform
function Actor:AddTransform() end

--- Adds a ModelRenderer component to the actor and returns it
---@return ModelRenderer
function Actor:AddModelRenderer() end

--- Adds a PhysicalBox component to the actor and returns it
---@return PhysicalBox
function Actor:AddPhysicalBox() end

--- Adds a PhysicalSphere component to the actor and returns it
---@return PhysicalSphere
function Actor:AddPhysicalSphere() end

--- Adds a PhysicalCapsule component to the actor and returns it
---@return PhysicalCapsule
function Actor:AddPhysicalCapsule() end

--- Adds a Camera component to the actor and returns it
---@return Camera
function Actor:AddCamera() end

--- Adds a PointLight component to the actor and returns it
---@return PointLight
function Actor:AddPointLight() end

--- Adds a SpotLight component to the actor and returns it
---@return SpotLight
function Actor:AddSpotLight() end

--- Adds a DirectionalLight component to the actor and returns it
---@return DirectionalLight
function Actor:AddDirectionalLight() end

--- Adds an AmbientBoxLight component to the actor and returns it
---@return AmbientBoxLight
function Actor:AddAmbientBoxLight() end

--- Adds an AmbientSphereLight component to the actor and returns it
---@return AmbientSphereLight
function Actor:AddAmbientSphereLight() end

--- Adds a MaterialRenderer component to the actor and returns it
---@return MaterialRenderer
function Actor:AddMaterialRenderer() end

--- Adds a SkinnedMeshRenderer component to the actor and returns it
---@return SkinnedMeshRenderer
function Actor:AddSkinnedMeshRenderer() end

--- Adds an AudioSource component to the actor and returns it
---@return AudioSource
function Actor:AddAudioSource() end

--- Adds an AudioListener component to the actor and returns it
---@return AudioListener
function Actor:AddAudioListener() end

--- Adds a PostProcessStack component to the actor and returns it
---@return PostProcessStack
function Actor:AddPostProcessStack() end

--- Adds a ReflectionProbe component to the actor and returns it
---@return ReflectionProbe
function Actor:AddReflectionProbe() end

--- Removes the ModelRenderer component from the actor
function Actor:RemoveModelRenderer() end
--- Removes the PhysicalBox component from the actor
function Actor:RemovePhysicalBox() end
--- Removes the PhysicalSphere component from the actor
function Actor:RemovePhysicalSphere() end
--- Removes the PhysicalCapsule component from the actor
function Actor:RemovePhysicalCapsule() end
--- Removes the Camera component from the actor
function Actor:RemoveCamera() end
--- Removes the PointLight component from the actor
function Actor:RemovePointLight() end
--- Removes the SpotLight component from the actor
function Actor:RemoveSpotLight() end
--- Removes the DirectionalLight component from the actor
function Actor:RemoveDirectionalLight() end
--- Removes the AmbientBoxLight component from the actor
function Actor:RemoveAmbientBoxLight() end
--- Removes the AmbientSphereLight component from the actor
function Actor:RemoveAmbientSphereLight() end
--- Removes the MaterialRenderer component from the actor
function Actor:RemoveMaterialRenderer() end
--- Removes the SkinnedMeshRenderer component from the actor
function Actor:RemoveSkinnedMeshRenderer() end
--- Removes the AudioSource component from the actor
function Actor:RemoveAudioSource() end
--- Removes the AudioListener component from the actor
function Actor:RemoveAudioListener() end
--- Removes the PostProcessStack component from the actor
function Actor:RemovePostProcessStack() end
--- Removes the ReflectionProbe component from the actor
function Actor:RemoveReflectionProbe() end

--- Adds a behaviour of given type to the actor and returns it
---@param name string
---@param scriptPath string
function Actor:AddBehaviour(name, scriptPath) end

--- Removes the behaviour of given type (Returns true on success)
---@overload fun(self: Actor, name: string): boolean
---@param name string
---@return boolean
function Actor:RemoveBehaviour(name) end
