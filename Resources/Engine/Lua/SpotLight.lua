---@meta

---@class SpotLight : Light
SpotLight = {}

---@return number
function SpotLight:GetConstant() end

---@return number
function SpotLight:GetLinear() end

---@return number
function SpotLight:GetQuadratic() end

---@return number
function SpotLight:GetCutOff() end

---@return number
function SpotLight:GetOuterCutOff() end

---@param constant number
function SpotLight:SetConstant(constant) end

---@param linear number
function SpotLight:SetLinear(linear) end

---@param quadratic number
function SpotLight:SetQuadratic(quadratic) end

---@param cutOff number
function SpotLight:SetCutOff(cutOff) end

---@param outerCutOff number
function SpotLight:SetOuterCutOff(outerCutOff) end
