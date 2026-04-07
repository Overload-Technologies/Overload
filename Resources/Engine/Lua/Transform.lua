---@meta

---@class Transform : Component
Transform = {}

---@param position Vector3
function Transform:SetPosition(position) end

---@param rotation Quaternion
function Transform:SetRotation(rotation) end

---@param scale Vector3
function Transform:SetScale(scale) end

---@param position Vector3
function Transform:SetLocalPosition(position) end

---@param rotation Quaternion
function Transform:SetLocalRotation(rotation) end

---@param scale Vector3
function Transform:SetLocalScale(scale) end

---@param position Vector3
function Transform:SetWorldPosition(position) end

---@param rotation Quaternion
function Transform:SetWorldRotation(rotation) end

---@param scale Vector3
function Transform:SetWorldScale(scale) end

---@return Vector3
function Transform:GetPosition() end

---@return Quaternion
function Transform:GetRotation() end

---@return Vector3
function Transform:GetScale() end

---@return Vector3
function Transform:GetLocalPosition() end

---@return Quaternion
function Transform:GetLocalRotation() end

---@return Vector3
function Transform:GetLocalScale() end

---@return Vector3
function Transform:GetWorldPosition() end

---@return Quaternion
function Transform:GetWorldRotation() end

---@return Vector3
function Transform:GetWorldScale() end

---@return Vector3
function Transform:GetForward() end

---@return Vector3
function Transform:GetUp() end

---@return Vector3
function Transform:GetRight() end

---@return Vector3
function Transform:GetLocalForward() end

---@return Vector3
function Transform:GetLocalUp() end

---@return Vector3
function Transform:GetLocalRight() end

---@return Vector3
function Transform:GetWorldForward() end

---@return Vector3
function Transform:GetWorldUp() end

---@return Vector3
function Transform:GetWorldRight() end

---@return Actor
function Transform:GetOwner() end
