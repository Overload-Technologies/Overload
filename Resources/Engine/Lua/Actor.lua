---@meta

---@class Actor
Actor = {}

---@return string
function Actor:GetName() end

---@param name string
function Actor:SetName(name) end

---@return string
function Actor:GetTag() end

---@param tag string
function Actor:SetTag(tag) end

---@return integer
function Actor:GetID() end

---@return Actor[]
function Actor:GetChildren() end

---@return Actor|nil
function Actor:GetParent() end

---@param parent Actor
function Actor:SetParent(parent) end

function Actor:DetachFromParent() end

function Actor:Destroy() end

---@return boolean
function Actor:IsSelfActive() end

---@return boolean
function Actor:IsActive() end

---@param active boolean
function Actor:SetActive(active) end

---@param potentialAncestor Actor
---@return boolean
function Actor:IsDescendantOf(potentialAncestor) end

---@return Transform|nil
function Actor:GetTransform() end

---@return PhysicalObject|nil
function Actor:GetPhysicalObject() end

---@return PhysicalBox|nil
function Actor:GetPhysicalBox() end

---@return PhysicalSphere|nil
function Actor:GetPhysicalSphere() end

---@return PhysicalCapsule|nil
function Actor:GetPhysicalCapsule() end

---@return Camera|nil
function Actor:GetCamera() end

---@return Light|nil
function Actor:GetLight() end

---@return PointLight|nil
function Actor:GetPointLight() end

---@return SpotLight|nil
function Actor:GetSpotLight() end

---@return DirectionalLight|nil
function Actor:GetDirectionalLight() end

---@return AmbientBoxLight|nil
function Actor:GetAmbientBoxLight() end

---@return AmbientSphereLight|nil
function Actor:GetAmbientSphereLight() end

---@return ModelRenderer|nil
function Actor:GetModelRenderer() end

---@return MaterialRenderer|nil
function Actor:GetMaterialRenderer() end

---@return SkinnedMeshRenderer|nil
function Actor:GetSkinnedMeshRenderer() end

---@return AudioSource|nil
function Actor:GetAudioSource() end

---@return AudioListener|nil
function Actor:GetAudioListener() end

---@return PostProcessStack|nil
function Actor:GetPostProcessStack() end

---@return ReflectionProbe|nil
function Actor:GetReflectionProbe() end

---@param name string
---@return table|nil
function Actor:GetBehaviour(name) end

---@return Transform
function Actor:AddTransform() end

---@return ModelRenderer
function Actor:AddModelRenderer() end

---@return PhysicalBox
function Actor:AddPhysicalBox() end

---@return PhysicalSphere
function Actor:AddPhysicalSphere() end

---@return PhysicalCapsule
function Actor:AddPhysicalCapsule() end

---@return Camera
function Actor:AddCamera() end

---@return PointLight
function Actor:AddPointLight() end

---@return SpotLight
function Actor:AddSpotLight() end

---@return DirectionalLight
function Actor:AddDirectionalLight() end

---@return AmbientBoxLight
function Actor:AddAmbientBoxLight() end

---@return AmbientSphereLight
function Actor:AddAmbientSphereLight() end

---@return MaterialRenderer
function Actor:AddMaterialRenderer() end

---@return SkinnedMeshRenderer
function Actor:AddSkinnedMeshRenderer() end

---@return AudioSource
function Actor:AddAudioSource() end

---@return AudioListener
function Actor:AddAudioListener() end

---@return PostProcessStack
function Actor:AddPostProcessStack() end

---@return ReflectionProbe
function Actor:AddReflectionProbe() end

function Actor:RemoveModelRenderer() end
function Actor:RemovePhysicalBox() end
function Actor:RemovePhysicalSphere() end
function Actor:RemovePhysicalCapsule() end
function Actor:RemoveCamera() end
function Actor:RemovePointLight() end
function Actor:RemoveSpotLight() end
function Actor:RemoveDirectionalLight() end
function Actor:RemoveAmbientBoxLight() end
function Actor:RemoveAmbientSphereLight() end
function Actor:RemoveMaterialRenderer() end
function Actor:RemoveSkinnedMeshRenderer() end
function Actor:RemoveAudioSource() end
function Actor:RemoveAudioListener() end
function Actor:RemovePostProcessStack() end
function Actor:RemoveReflectionProbe() end

---@param name string
---@param scriptPath string
function Actor:AddBehaviour(name, scriptPath) end

---@overload fun(self: Actor, name: string): boolean
---@param name string
---@return boolean
function Actor:RemoveBehaviour(name) end
