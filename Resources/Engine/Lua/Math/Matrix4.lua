---@meta

--- Mathematic representation of a 4x4 Matrix of floats
---@class Matrix4
Matrix4 = {}

---@overload fun(): Matrix4
---@overload fun(m00: number, m01: number, m02: number, m03: number, m10: number, m11: number, m12: number, m13: number, m20: number, m21: number, m22: number, m23: number, m30: number, m31: number, m32: number, m33: number): Matrix4
---@return Matrix4
function Matrix4.new(...) end

--- Returns an identity Matrix4
---@return Matrix4
function Matrix4.Identity() end

---@param other Matrix4
---@return Matrix4
function Matrix4:__add(other) end

---@param rhs number|Matrix4
---@return Matrix4
function Matrix4:__sub(rhs) end

---@param rhs number|Vector4|Matrix4
---@return Matrix4|Vector4
function Matrix4:__mul(rhs) end

---@param rhs number|Matrix4
---@return Matrix4
function Matrix4:__div(rhs) end

---@return string
function Matrix4:__tostring() end

--- Returns true if the matrix is identity
---@return boolean
function Matrix4:IsIdentity() end

--- Returns matrix determinant
---@return number
function Matrix4:Determinant() end

--- Returns the transpose of the given matrix
---@return Matrix4
function Matrix4:Transpose() end

--- Returns matrix minor
---@param row integer
---@param col integer
---@return number
function Matrix4:Minor(row, col) end

--- Returns the inverse of the given matrix
---@return Matrix4
function Matrix4:Inverse() end

--- Returns a translation matrix from the given translation
---@param translation Vector3
---@return Matrix4
function Matrix4.Translation(translation) end

--- Returns a matrix translated using the given translation
---@param translation Vector3
---@return Matrix4
function Matrix4:Translate(translation) end

--- Returns a rotation matrix constructed using the given X axis angle
---@param angle number
---@return Matrix4
function Matrix4.RotationOnAxisX(angle) end

--- Returns a matrix rotated around the given X axis angle
---@param angle number
---@return Matrix4
function Matrix4:RotateOnAxisX(angle) end

--- Returns a rotation matrix constructed using the given Y axis angle
---@param angle number
---@return Matrix4
function Matrix4.RotationOnAxisY(angle) end

--- Returns a matrix rotated around the given Y axis angle
---@param angle number
---@return Matrix4
function Matrix4:RotateOnAxisY(angle) end

--- Returns a rotation matrix constructed using the given Z axis angle
---@param angle number
---@return Matrix4
function Matrix4.RotationOnAxisZ(angle) end

--- Returns a matrix rotated around the given Z axis angle
---@param angle number
---@return Matrix4
function Matrix4:RotateOnAxisZ(angle) end

--- Returns a rotation matrix constructed using the given axis angle (Applied to all angles in YXZ order)
---@param rotation Vector3
---@return Matrix4
function Matrix4.RotationYXZ(rotation) end

--- Returns a rotation matrix rotated using the given axis angle (Applied to all angles in YXZ order)
---@param rotation Vector3
---@return Matrix4
function Matrix4:RotateYXZ(rotation) end

--- Returns a rotation matrix created from the given quaternion
---@param rotation Quaternion
---@return Matrix4
function Matrix4.Rotation(rotation) end

--- Returns a matrix rotated using the given quaternion
---@param rotation Quaternion
---@return Matrix4
function Matrix4:Rotate(rotation) end

--- Returns a scaling matrix from the given scale
---@param scale Vector3
---@return Matrix4
function Matrix4.Scaling(scale) end

--- Returns a scaled matrix using the given scale
---@param scale Vector3
---@return Matrix4
function Matrix4:Scale(scale) end

--- Returns the given row
---@param row integer
---@return Vector4
function Matrix4:GetRow(row) end

--- Returns the given column
---@param col integer
---@return Vector4
function Matrix4:GetColumn(col) end

--- Creates a perspective matrix
---@param fov number
---@param aspectRatio number
---@param near number
---@param far number
---@return Matrix4
function Matrix4.CreatePerspective(fov, aspectRatio, near, far) end

--- Creates a view matrix
---@param eyeX number
---@param eyeY number
---@param eyeZ number
---@param lookX number
---@param lookY number
---@param lookZ number
---@param upX number
---@param upY number
---@param upZ number
---@return Matrix4
function Matrix4.CreateView(eyeX, eyeY, eyeZ, lookX, lookY, lookZ, upX, upY, upZ) end

--- Creates a frustum matrix
---@param left number
---@param right number
---@param bottom number
---@param top number
---@param near number
---@param far number
---@return Matrix4
function Matrix4.CreateFrustum(left, right, bottom, top, near, far) end

--- Returns element at index (row, column)
---@param row integer
---@param col integer
---@return number
function Matrix4:Get(row, col) end

--- Sets element at index (row, column)
---@param row integer
---@param col integer
---@param value number
function Matrix4:Set(row, col, value) end
