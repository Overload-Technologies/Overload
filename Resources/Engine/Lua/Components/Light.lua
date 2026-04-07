---@meta

--- Base class for any light
---@class Light : Component
Light = {}

--- Returns the actor that owns this component
---@return Actor
function Light:GetOwner() end

--- Returns light color
---@return Vector3
function Light:GetColor() end

--- Returns light intensity
---@return number
function Light:GetIntensity() end

--- Defines light color
---@param color Vector3
function Light:SetColor(color) end

--- Defines light intensity
---@param intensity number
function Light:SetIntensity(intensity) end
