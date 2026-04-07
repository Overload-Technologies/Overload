---@meta

--- Mathematic representation of a 2D vector of floats
---@class Vector2
---@field x number X component of the Vector2
---@field y number Y component of the Vector2
Vector2 = {}

--- Creates an instance of Vector2
---@overload fun(): Vector2
---@overload fun(x: number, y: number): Vector2
---@param x number
---@param y number
---@return Vector2
function Vector2.new(x, y) end

--- Returns a (1,1) Vector2
---@return Vector2
function Vector2.One() end

--- Returns a (0,0) Vector2
---@return Vector2
function Vector2.Zero() end

---@param other Vector2
---@return Vector2
function Vector2:__add(other) end

---@param other Vector2
---@return Vector2
function Vector2:__sub(other) end

---@return Vector2
function Vector2:__unm() end

---@param scalar number
---@return Vector2
function Vector2:__mul(scalar) end

---@param scalar number
---@return Vector2
function Vector2:__div(scalar) end

---@return string
function Vector2:__tostring() end

--- Returns the length of the given Vector2
---@return number
function Vector2:Length() end

--- Returns the dot product of the two given Vector2
---@param other Vector2
---@return number
function Vector2:Dot(other) end

--- Returns the normalized version of the given Vector2
---@return Vector2
function Vector2:Normalize() end

--- Calculates the linear interpolation between two Vector2 using the given alpha
---@param start Vector2
---@param end_ Vector2
---@param alpha number
---@return Vector2
function Vector2.Lerp(start, end_, alpha) end

--- Calculates the angle between two Vector2
---@param from Vector2
---@param to Vector2
---@return number
function Vector2.AngleBetween(from, to) end
