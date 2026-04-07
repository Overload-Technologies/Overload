---@meta

--- Mathematic representation of a 3x3 Matrix of floats
---@class Matrix3
Matrix3 = {}

--- Creates a default Matrix3
---@overload fun(): Matrix3
--- Creates a Matrix3 with all diagonal components set to the given value
---@overload fun(diagonal: number): Matrix3
--- Creates a Matrix3 with all given component values
---@overload fun(m00: number, m01: number, m02: number, m10: number, m11: number, m12: number, m20: number, m21: number, m22: number): Matrix3
---@return Matrix3
function Matrix3.new(...) end

--- Returns an identity Matrix3
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

--- Returns true if the matrix is identity
---@return boolean
function Matrix3:IsIdentity() end

--- Returns matrix determinant
---@return number
function Matrix3:Determinant() end

--- Returns the transpose of the given matrix
---@return Matrix3
function Matrix3:Transpose() end

--- Returns the cofactor of the given matrix
---@return Matrix3
function Matrix3:Cofactor() end

--- Returns the minor of the given matrix
---@return Matrix3
function Matrix3:Minor() end

--- Returns the adjoint of the given matrix
---@return Matrix3
function Matrix3:Adjoint() end

--- Returns the inverse of the given matrix
---@return Matrix3
function Matrix3:Inverse() end

--- Returns a translation matrix from the given translation
---@param translation Vector2
---@return Matrix3
function Matrix3.Translation(translation) end

--- Returns a matrix translated using the given translation
---@param translation Vector2
---@return Matrix3
function Matrix3:Translate(translation) end

--- Returns a rotation matrix from the given rotation angle (degrees)
---@param angle number
---@return Matrix3
function Matrix3.Rotation(angle) end

--- Returns a matrix rotated using the given rotation angle (degrees)
---@param angle number
---@return Matrix3
function Matrix3:Rotate(angle) end

--- Returns a scaling matrix from the given scale
---@param scale Vector2
---@return Matrix3
function Matrix3.Scaling(scale) end

--- Returns a scaled matrix using the given scale
---@param scale Vector2
---@return Matrix3
function Matrix3:Scale(scale) end

--- Returns the given row
---@param row integer
---@return Vector3
function Matrix3:GetRow(row) end

--- Returns the given column
---@param col integer
---@return Vector3
function Matrix3:GetColumn(col) end

--- Returns element at index (row, column)
---@param row integer
---@param col integer
---@return number
function Matrix3:Get(row, col) end

--- Sets element at index (row, column)
---@param row integer
---@param col integer
---@param value number
function Matrix3:Set(row, col, value) end
