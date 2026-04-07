---@meta

--- A light that is localized, has attenuation and has a direction
---@class SpotLight : Light
SpotLight = {}

--- Returns the light constant
---@return number
function SpotLight:GetConstant() end

--- Returns the light linear
---@return number
function SpotLight:GetLinear() end

--- Returns the light quadratic
---@return number
function SpotLight:GetQuadratic() end

--- Returns the light cutoff
---@return number
function SpotLight:GetCutOff() end

--- Returns the light outer cutoff
---@return number
function SpotLight:GetOuterCutOff() end

--- Sets the light constant
---@param constant number
function SpotLight:SetConstant(constant) end

--- Sets the light linear
---@param linear number
function SpotLight:SetLinear(linear) end

--- Sets the light quadratic
---@param quadratic number
function SpotLight:SetQuadratic(quadratic) end

--- Sets the light cutoff
---@param cutOff number
function SpotLight:SetCutOff(cutOff) end

--- Sets the light outer cutoff
---@param outerCutOff number
function SpotLight:SetOuterCutOff(outerCutOff) end
