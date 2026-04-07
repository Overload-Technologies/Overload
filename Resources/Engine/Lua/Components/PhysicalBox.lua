---@meta

--- PhysicalObject with a box shape
---@class PhysicalBox : PhysicalObject
PhysicalBox = {}

--- Returns the size of the physical box
---@return Vector3
function PhysicalBox:GetSize() end

--- Sets the size of the physical box
---@param size Vector3
function PhysicalBox:SetSize(size) end
