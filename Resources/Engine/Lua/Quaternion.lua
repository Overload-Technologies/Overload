---@meta

---@class Quaternion
---@field x number
---@field y number
---@field z number
---@field w number
Quaternion = {}

---@overload fun(): Quaternion
---@overload fun(uniformValue: number): Quaternion
---@overload fun(x: number, y: number, z: number, w: number): Quaternion
---@overload fun(eulerAngles: Vector3): Quaternion
---@return Quaternion
function Quaternion.new(...) end

---@param other Quaternion
---@return Quaternion
function Quaternion:__add(other) end

---@param other Quaternion
---@return Quaternion
function Quaternion:__sub(other) end

---@param rhs number|Quaternion|Matrix3|Vector3
---@return Quaternion|Matrix3|Vector3
function Quaternion:__mul(rhs) end

---@param scalar number
---@return Quaternion
function Quaternion:__div(scalar) end

---@return string
function Quaternion:__tostring() end

---@return boolean
function Quaternion:IsIdentity() end

---@return boolean
function Quaternion:IsPure() end

---@return boolean
function Quaternion:IsNormalized() end

---@param other Quaternion
---@return number
function Quaternion:Dot(other) end

---@return Quaternion
function Quaternion:Normalize() end

---@return number
function Quaternion:Length() end

---@return number
function Quaternion:LengthSquare() end

---@return number
function Quaternion:GetAngle() end

---@return Vector3
function Quaternion:GetRotationAxis() end

---@return Quaternion
function Quaternion:Inverse() end

---@return Quaternion
function Quaternion:Conjugate() end

---@return Quaternion
function Quaternion:Square() end

---@return Vector3, number
function Quaternion:GetAxisAndAngle() end

---@param other Quaternion
---@return number
function Quaternion:AngularDistance(other) end

---@param start Quaternion
---@param end_ Quaternion
---@param alpha number
---@return Quaternion
function Quaternion.Lerp(start, end_, alpha) end

---@param start Quaternion
---@param end_ Quaternion
---@param alpha number
---@return Quaternion
function Quaternion.Slerp(start, end_, alpha) end

---@param start Quaternion
---@param end_ Quaternion
---@param alpha number
---@return Quaternion
function Quaternion.Nlerp(start, end_, alpha) end

---@overload fun(point: Vector3, rotation: Quaternion): Vector3
---@overload fun(point: Vector3, rotation: Quaternion, pivot: Vector3): Vector3
---@param point Vector3
---@param rotation Quaternion
---@return Vector3
function Quaternion.RotatePoint(point, rotation) end

---@return Vector3
function Quaternion:EulerAngles() end

---@return Matrix3
function Quaternion:ToMatrix3() end

---@return Matrix4
function Quaternion:ToMatrix4() end
