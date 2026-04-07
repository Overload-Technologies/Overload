---@meta

--- Represents an ambient light with a box shape
---@class AmbientBoxLight : Light
AmbientBoxLight = {}

--- Returns the size of the box
---@return Vector3
function AmbientBoxLight:GetSize() end

--- Defines the size of the box
---@param size Vector3
function AmbientBoxLight:SetSize(size) end
