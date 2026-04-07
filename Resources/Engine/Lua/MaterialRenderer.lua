---@meta

---@class MaterialRenderer : Component
MaterialRenderer = {}

---@return Actor
function MaterialRenderer:GetOwner() end

---@param index integer
---@param material userdata
function MaterialRenderer:SetMaterial(index, material) end

---@param row integer
---@param column integer
---@param value number
function MaterialRenderer:SetUserMatrixElement(row, column, value) end

---@param row integer
---@param column integer
---@return number
function MaterialRenderer:GetUserMatrixElement(row, column) end
