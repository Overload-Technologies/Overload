---@meta

---@class PointLight : Light
PointLight = {}

---@return number
function PointLight:GetConstant() end

---@return number
function PointLight:GetLinear() end

---@return number
function PointLight:GetQuadratic() end

---@param constant number
function PointLight:SetConstant(constant) end

---@param linear number
function PointLight:SetLinear(linear) end

---@param quadratic number
function PointLight:SetQuadratic(quadratic) end
