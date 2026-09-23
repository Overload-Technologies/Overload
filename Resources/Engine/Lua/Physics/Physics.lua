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

--- Returns the index of the collision layer having the given name, or nil if no layer matches
---@param name string
---@return integer|nil
function Physics.GetLayerIndex(name) end

--- Returns the name of the given collision layer
---@param layer integer
---@return string
function Physics.GetLayerName(layer) end

--- Defines if the two given collision layers should collide together
---@param first integer
---@param second integer
---@param collide boolean
function Physics.SetLayerCollision(first, second, collide) end

--- Returns true if the two given collision layers collide together
---@param first integer
---@param second integer
---@return boolean
function Physics.GetLayerCollision(first, second) end
