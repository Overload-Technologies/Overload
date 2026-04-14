---@meta

--- A ModelRenderer is necessary in combination with a MaterialRenderer to render a model in the world
---@class ModelRenderer : Component
ModelRenderer = {}

--- Returns the actor that owns this component
---@return Actor
function ModelRenderer:GetOwner() end

--- Returns the current model
---@return Model|nil
function ModelRenderer:GetModel() end

--- Defines the model to use
---@param model Model
function ModelRenderer:SetModel(model) end

--- Returns the current bounding mode
---@return FrustumBehaviour
function ModelRenderer:GetFrustumBehaviour() end

--- Sets a bounding mode
---@param behaviour FrustumBehaviour
function ModelRenderer:SetFrustumBehaviour(behaviour) end
