---@meta

--- Mathematic representation of a 3D vector of floats
---@class Vector3
---@field x number X component of the Vector3
---@field y number Y component of the Vector3
---@field z number Z component of the Vector3
Vector3 = {}

--- Creates an instance of Vector3
---@overload fun(): Vector3
---@overload fun(x: number, y: number, z: number): Vector3
---@param x number
---@param y number
---@param z number
---@return Vector3
function Vector3.new(x, y, z) end

--- Returns a (1,1,1) Vector3
---@return Vector3
function Vector3.One() end

--- Returns a (0,0,0) Vector3
---@return Vector3
function Vector3.Zero() end

--- Returns a (0,0,1) Vector3
---@return Vector3
function Vector3.Forward() end

--- Returns a (0,0,-1) Vector3
---@return Vector3
function Vector3.Backward() end

--- Returns a (0,1,0) Vector3
---@return Vector3
function Vector3.Up() end

--- Returns a (0,-1,0) Vector3
---@return Vector3
function Vector3.Down() end

--- Returns a (1,0,0) Vector3
---@return Vector3
function Vector3.Right() end

--- Returns a (-1,0,0) Vector3
---@return Vector3
function Vector3.Left() end

---@param other Vector3
---@return Vector3
function Vector3:__add(other) end

---@param other Vector3
---@return Vector3
function Vector3:__sub(other) end

---@return Vector3
function Vector3:__unm() end

---@param scalar number|Vector3
---@return Vector3
function Vector3:__mul(scalar) end

---@param scalar number
---@return Vector3
function Vector3:__div(scalar) end

---@return string
function Vector3:__tostring() end

--- Returns the length of the given Vector3
---@return number
function Vector3:Length() end

--- Returns the dot product of the two given Vector3
---@param other Vector3
---@return number
function Vector3:Dot(other) end

--- Returns the cross product of the two given Vector3
---@param other Vector3
---@return Vector3
function Vector3:Cross(other) end

--- Returns the normalized version of the given Vector3
---@return Vector3
function Vector3:Normalize() end

--- Calculates the linear interpolation between two Vector3 using the given alpha
---@param start Vector3
---@param end_ Vector3
---@param alpha number
---@return Vector3
function Vector3.Lerp(start, end_, alpha) end

--- Calculates the angle between two Vector3
---@param from Vector3
---@param to Vector3
---@return number
function Vector3.AngleBetween(from, to) end

--- Returns the distance between the given Vector3
---@param a Vector3
---@param b Vector3
---@return number
function Vector3.Distance(a, b) end
