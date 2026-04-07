---@meta

---@class Matrix4
Matrix4 = {}

---@overload fun(): Matrix4
---@overload fun(m00: number, m01: number, m02: number, m03: number, m10: number, m11: number, m12: number, m13: number, m20: number, m21: number, m22: number, m23: number, m30: number, m31: number, m32: number, m33: number): Matrix4
---@return Matrix4
function Matrix4.new(...) end

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

---@return boolean
function Matrix4:IsIdentity() end

---@return number
function Matrix4:Determinant() end

---@return Matrix4
function Matrix4:Transpose() end

---@param row integer
---@param col integer
---@return number
function Matrix4:Minor(row, col) end

---@return Matrix4
function Matrix4:Inverse() end

---@param translation Vector3
---@return Matrix4
function Matrix4.Translation(translation) end

---@param translation Vector3
---@return Matrix4
function Matrix4:Translate(translation) end

---@param angle number
---@return Matrix4
function Matrix4.RotationOnAxisX(angle) end

---@param angle number
---@return Matrix4
function Matrix4:RotateOnAxisX(angle) end

---@param angle number
---@return Matrix4
function Matrix4.RotationOnAxisY(angle) end

---@param angle number
---@return Matrix4
function Matrix4:RotateOnAxisY(angle) end

---@param angle number
---@return Matrix4
function Matrix4.RotationOnAxisZ(angle) end

---@param angle number
---@return Matrix4
function Matrix4:RotateOnAxisZ(angle) end

---@param rotation Vector3
---@return Matrix4
function Matrix4.RotationYXZ(rotation) end

---@param rotation Vector3
---@return Matrix4
function Matrix4:RotateYXZ(rotation) end

---@param rotation Quaternion
---@return Matrix4
function Matrix4.Rotation(rotation) end

---@param rotation Quaternion
---@return Matrix4
function Matrix4:Rotate(rotation) end

---@param scale Vector3
---@return Matrix4
function Matrix4.Scaling(scale) end

---@param scale Vector3
---@return Matrix4
function Matrix4:Scale(scale) end

---@param row integer
---@return Vector4
function Matrix4:GetRow(row) end

---@param col integer
---@return Vector4
function Matrix4:GetColumn(col) end

---@param fov number
---@param aspectRatio number
---@param near number
---@param far number
---@return Matrix4
function Matrix4.CreatePerspective(fov, aspectRatio, near, far) end

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

---@param left number
---@param right number
---@param bottom number
---@param top number
---@param near number
---@param far number
---@return Matrix4
function Matrix4.CreateFrustum(left, right, bottom, top, near, far) end

---@param row integer
---@param col integer
---@return number
function Matrix4:Get(row, col) end

---@param row integer
---@param col integer
---@param value number
function Matrix4:Set(row, col, value) end
