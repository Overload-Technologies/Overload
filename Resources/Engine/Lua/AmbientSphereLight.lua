---@meta

--- Represents an ambient light with a sphere shape
---@class AmbientSphereLight : Light
AmbientSphereLight = {}

--- Returns the radius of the sphere
---@return number
function AmbientSphereLight:GetRadius() end

--- Defines the radius of the sphere
---@param radius number
function AmbientSphereLight:SetRadius(radius) end
