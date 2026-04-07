---@meta

--- Mathematic representation of a 4D vector of floats
---@class Vector4
---@field x number X component of the Vector4
---@field y number Y component of the Vector4
---@field z number Z component of the Vector4
---@field w number W component of the Vector4
Vector4 = {}

--- Creates an instance of Vector4
---@overload fun(): Vector4
---@overload fun(x: number, y: number, z: number, w: number): Vector4
---@param x number
---@param y number
---@param z number
---@param w number
---@return Vector4
function Vector4.new(x, y, z, w) end

--- Returns a (1,1,1,1) Vector4
---@return Vector4
function Vector4.One() end

--- Returns a (0,0,0,0) Vector4
---@return Vector4
function Vector4.Zero() end

---@param other Vector4
---@return Vector4
function Vector4:__add(other) end

---@param other Vector4
---@return Vector4
function Vector4:__sub(other) end

---@return Vector4
function Vector4:__unm() end

---@param scalar number
---@return Vector4
function Vector4:__mul(scalar) end

---@param scalar number
---@return Vector4
function Vector4:__div(scalar) end

---@return string
function Vector4:__tostring() end

--- Returns the length of the given Vector4
---@return number
function Vector4:Length() end

--- Returns the dot product of the two given Vector4
---@param other Vector4
---@return number
function Vector4:Dot(other) end

--- Returns the normalized version of the given Vector4
---@return Vector4
function Vector4:Normalize() end

--- Calculates the linear interpolation between two Vector4 using the given alpha
---@param start Vector4
---@param end_ Vector4
---@param alpha number
---@return Vector4
function Vector4.Lerp(start, end_, alpha) end
