---@meta

---@class Light : Component
Light = {}

---@return Actor
function Light:GetOwner() end

---@return Vector3
function Light:GetColor() end

---@return number
function Light:GetIntensity() end

---@param color Vector3
function Light:SetColor(color) end

---@param intensity number
function Light:SetIntensity(intensity) end
