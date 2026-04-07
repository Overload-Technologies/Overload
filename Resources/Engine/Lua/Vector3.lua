---@meta

---@class Vector3
---@field x number
---@field y number
---@field z number
Vector3 = {}

---@overload fun(): Vector3
---@overload fun(x: number, y: number, z: number): Vector3
---@param x number
---@param y number
---@param z number
---@return Vector3
function Vector3.new(x, y, z) end

---@return Vector3
function Vector3.One() end

---@return Vector3
function Vector3.Zero() end

---@return Vector3
function Vector3.Forward() end

---@return Vector3
function Vector3.Backward() end

---@return Vector3
function Vector3.Up() end

---@return Vector3
function Vector3.Down() end

---@return Vector3
function Vector3.Right() end

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

---@return number
function Vector3:Length() end

---@param other Vector3
---@return number
function Vector3:Dot(other) end

---@param other Vector3
---@return Vector3
function Vector3:Cross(other) end

---@return Vector3
function Vector3:Normalize() end

---@param start Vector3
---@param end_ Vector3
---@param alpha number
---@return Vector3
function Vector3.Lerp(start, end_, alpha) end

---@param from Vector3
---@param to Vector3
---@return number
function Vector3.AngleBetween(from, to) end

---@param a Vector3
---@param b Vector3
---@return number
function Vector3.Distance(a, b) end
