---@meta

---@class Matrix3
Matrix3 = {}

---@overload fun(): Matrix3
---@overload fun(diagonal: number): Matrix3
---@overload fun(m00: number, m01: number, m02: number, m10: number, m11: number, m12: number, m20: number, m21: number, m22: number): Matrix3
---@return Matrix3
function Matrix3.new(...) end

---@return Matrix3
function Matrix3.Identity() end

---@param other Matrix3
---@return Matrix3
function Matrix3:__add(other) end

---@param other Matrix3
---@return Matrix3
function Matrix3:__sub(other) end

---@param rhs number|Vector3|Matrix3
---@return Matrix3|Vector3
function Matrix3:__mul(rhs) end

---@param rhs number|Matrix3
---@return Matrix3
function Matrix3:__div(rhs) end

---@return string
function Matrix3:__tostring() end

---@return boolean
function Matrix3:IsIdentity() end

---@return number
function Matrix3:Determinant() end

---@return Matrix3
function Matrix3:Transpose() end

---@return Matrix3
function Matrix3:Cofactor() end

---@return Matrix3
function Matrix3:Minor() end

---@return Matrix3
function Matrix3:Adjoint() end

---@return Matrix3
function Matrix3:Inverse() end

---@param translation Vector2
---@return Matrix3
function Matrix3.Translation(translation) end

---@param translation Vector2
---@return Matrix3
function Matrix3:Translate(translation) end

---@param angle number
---@return Matrix3
function Matrix3.Rotation(angle) end

---@param angle number
---@return Matrix3
function Matrix3:Rotate(angle) end

---@param scale Vector2
---@return Matrix3
function Matrix3.Scaling(scale) end

---@param scale Vector2
---@return Matrix3
function Matrix3:Scale(scale) end

---@param row integer
---@return Vector3
function Matrix3:GetRow(row) end

---@param col integer
---@return Vector3
function Matrix3:GetColumn(col) end

---@param row integer
---@param col integer
---@return number
function Matrix3:Get(row, col) end

---@param row integer
---@param col integer
---@param value number
function Matrix3:Set(row, col, value) end
