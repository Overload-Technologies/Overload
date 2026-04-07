---@meta

--- Represents a physical object with a capsule shape
---@class PhysicalCapsule : PhysicalObject
PhysicalCapsule = {}

--- Returns the radius of the physical capsule
---@return number
function PhysicalCapsule:GetRadius() end

--- Sets the radius of the physical capsule
---@param radius number
function PhysicalCapsule:SetRadius(radius) end

--- Returns the height of the physical capsule
---@return number
function PhysicalCapsule:GetHeight() end

--- Sets the height of the physical capsule
---@param height number
function PhysicalCapsule:SetHeight(height) end
