---@meta

--- Represents the 3D transformations applied to an actor
---@class Transform : Component
Transform = {}

--- Alias for SetLocalPosition
---@param position Vector3
function Transform:SetPosition(position) end

--- Alias for SetLocalRotation
---@param rotation Quaternion
function Transform:SetRotation(rotation) end

--- Alias for SetLocalScale
---@param scale Vector3
function Transform:SetScale(scale) end

--- Sets the position of the transform in the local space
---@param position Vector3
function Transform:SetLocalPosition(position) end

--- Sets the rotation of the transform in the local space
---@param rotation Quaternion
function Transform:SetLocalRotation(rotation) end

--- Sets the scale of the transform in the local space
---@param scale Vector3
function Transform:SetLocalScale(scale) end

--- Sets the position of the transform in world space
---@param position Vector3
function Transform:SetWorldPosition(position) end

--- Sets the rotation of the transform in world space
---@param rotation Quaternion
function Transform:SetWorldRotation(rotation) end

--- Sets the scale of the transform in world space
---@param scale Vector3
function Transform:SetWorldScale(scale) end

--- Alias for GetWorldPosition
---@return Vector3
function Transform:GetPosition() end

--- Alias for GetWorldRotation
---@return Quaternion
function Transform:GetRotation() end

--- Alias for GetWorldScale
---@return Vector3
function Transform:GetScale() end

--- Returns the position in local space
---@return Vector3
function Transform:GetLocalPosition() end

--- Returns the rotation in local space
---@return Quaternion
function Transform:GetLocalRotation() end

--- Returns the scale in local space
---@return Vector3
function Transform:GetLocalScale() end

--- Returns the position in world space
---@return Vector3
function Transform:GetWorldPosition() end

--- Returns the rotation in world space
---@return Quaternion
function Transform:GetWorldRotation() end

--- Returns the scale in world space
---@return Vector3
function Transform:GetWorldScale() end

--- Alias for GetWorldForward
---@return Vector3
function Transform:GetForward() end

--- Alias for GetWorldUp
---@return Vector3
function Transform:GetUp() end

--- Alias for GetWorldRight
---@return Vector3
function Transform:GetRight() end

--- Returns the transform local forward
---@return Vector3
function Transform:GetLocalForward() end

--- Returns the transform local up
---@return Vector3
function Transform:GetLocalUp() end

--- Returns the transform local right
---@return Vector3
function Transform:GetLocalRight() end

--- Returns the transform world forward
---@return Vector3
function Transform:GetWorldForward() end

--- Returns the transform world up
---@return Vector3
function Transform:GetWorldUp() end

--- Returns the transform world right
---@return Vector3
function Transform:GetWorldRight() end

--- Returns the actor that owns this component
---@return Actor
function Transform:GetOwner() end
