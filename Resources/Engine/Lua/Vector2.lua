---@meta

---@class Vector2
---@field x number
---@field y number
Vector2 = {}

---@overload fun(): Vector2
---@overload fun(x: number, y: number): Vector2
---@param x number
---@param y number
---@return Vector2
function Vector2.new(x, y) end

---@return Vector2
function Vector2.One() end

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

---@return number
function Vector2:Length() end

---@param other Vector2
---@return number
function Vector2:Dot(other) end

---@return Vector2
function Vector2:Normalize() end

---@param start Vector2
---@param end_ Vector2
---@param alpha number
---@return Vector2
function Vector2.Lerp(start, end_, alpha) end

---@param from Vector2
---@param to Vector2
---@return number
function Vector2.AngleBetween(from, to) end
