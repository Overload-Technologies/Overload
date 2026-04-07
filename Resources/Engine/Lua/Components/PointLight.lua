---@meta

--- A light that is localized and has attenuation
---@class PointLight : Light
PointLight = {}

--- Returns the light constant
---@return number
function PointLight:GetConstant() end

--- Returns the light linear
---@return number
function PointLight:GetLinear() end

--- Returns the light quadratic
---@return number
function PointLight:GetQuadratic() end

--- Sets the light constant
---@param constant number
function PointLight:SetConstant(constant) end

--- Sets the light linear
---@param linear number
function PointLight:SetLinear(linear) end

--- Sets the light quadratic
---@param quadratic number
function PointLight:SetQuadratic(quadratic) end
