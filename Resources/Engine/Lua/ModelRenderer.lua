---@meta

---@enum FrustumBehaviour
FrustumBehaviour = {
    DISABLED = 0,
    MESH_BOUNDS = 1,
    CUSTOM_BOUNDS = 2,
}

---@class ModelRenderer : Component
ModelRenderer = {}

---@return Actor
function ModelRenderer:GetOwner() end

---@return userdata|nil
function ModelRenderer:GetModel() end

---@param model userdata
function ModelRenderer:SetModel(model) end

---@return FrustumBehaviour
function ModelRenderer:GetFrustumBehaviour() end

---@param behaviour FrustumBehaviour
function ModelRenderer:SetFrustumBehaviour(behaviour) end
