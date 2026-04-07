---@meta

--- Mathematic representation of a Quaternion with float precision
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

--- Returns true if the quaternion is Identity (x, y and z components are 0)
---@return boolean
function Quaternion:IsIdentity() end

--- Returns true if the quaternion is pure (w is 0)
---@return boolean
function Quaternion:IsPure() end

--- Returns true if the quaternion is normalized
---@return boolean
function Quaternion:IsNormalized() end

--- Calculates the dot product between two quaternions
---@param other Quaternion
---@return number
function Quaternion:Dot(other) end

--- Returns the normalized version of the given quaternion
---@return Quaternion
function Quaternion:Normalize() end

--- Returns the length of the given quaternion
---@return number
function Quaternion:Length() end

--- Returns the squared length of the given quaternion. This function is faster than Quaternion.Length
---@return number
function Quaternion:LengthSquare() end

--- Returns the angle of the given quaternion
---@return number
function Quaternion:GetAngle() end

--- Returns the rotation axis of the given quaternion
---@return Vector3
function Quaternion:GetRotationAxis() end

--- Returns the inverse of the quaternion
---@return Quaternion
function Quaternion:Inverse() end

--- Returns the conjugate of the quaternion
---@return Quaternion
function Quaternion:Conjugate() end

--- Returns the squared version of this quaternion
---@return Quaternion
function Quaternion:Square() end

--- Returns the axis and the angle of the given quaternion
---@return Vector3, number
function Quaternion:GetAxisAndAngle() end

--- Returns the angle between two quaternions
---@param other Quaternion
---@return number
function Quaternion:AngularDistance(other) end

--- Returns the linear interpolation of two quaternions based on the given interpolation coefficient
---@param start Quaternion
---@param end_ Quaternion
---@param alpha number
---@return Quaternion
function Quaternion.Lerp(start, end_, alpha) end

--- Returns the spherical linear interpolation of two quaternions based on the given interpolation coefficient
---@param start Quaternion
---@param end_ Quaternion
---@param alpha number
---@return Quaternion
function Quaternion.Slerp(start, end_, alpha) end

--- Returns the normalized linear interpolation of two quaternions based on the given interpolation coefficient
---@param start Quaternion
---@param end_ Quaternion
---@param alpha number
---@return Quaternion
function Quaternion.Nlerp(start, end_, alpha) end

--- Rotates a point around a pivot using a quaternion
---@overload fun(point: Vector3, rotation: Quaternion): Vector3
---@overload fun(point: Vector3, rotation: Quaternion, pivot: Vector3): Vector3
---@param point Vector3
---@param rotation Quaternion
---@return Vector3
function Quaternion.RotatePoint(point, rotation) end

--- Returns euler axis angles (In degrees)
---@return Vector3
function Quaternion:EulerAngles() end

--- Returns a rotation matrix (3x3) out of the given quaternion
---@return Matrix3
function Quaternion:ToMatrix3() end

--- Returns a rotation matrix (4x4) out of the given quaternion
---@return Matrix4
function Quaternion:ToMatrix4() end
