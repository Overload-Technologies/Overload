---@meta

---@class Vector4
---@field x number
---@field y number
---@field z number
---@field w number
Vector4 = {}

---@overload fun(): Vector4
---@overload fun(x: number, y: number, z: number, w: number): Vector4
---@param x number
---@param y number
---@param z number
---@param w number
---@return Vector4
function Vector4.new(x, y, z, w) end

---@return Vector4
function Vector4.One() end

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

---@return number
function Vector4:Length() end

---@param other Vector4
---@return number
function Vector4:Dot(other) end

---@return Vector4
function Vector4:Normalize() end

---@param start Vector4
---@param end_ Vector4
---@param alpha number
---@return Vector4
function Vector4.Lerp(start, end_, alpha) end
