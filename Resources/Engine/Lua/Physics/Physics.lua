---@meta

--- Some global physics functions
---@class Physics
Physics = {}

--- Casts a ray against all Physical Object in the Scene and returns information on what was hit
---@param origin Vector3
---@param direction Vector3
---@param distance number
---@return RaycastHit
function Physics.Raycast(origin, direction, distance) end
