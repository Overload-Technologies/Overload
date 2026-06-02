---@meta

--- Represents the 3D transformations applied to an actor
---@class Transform : Component
Transform = {}

--- Defines the anchor preset used by Transform UI data
---@enum AnchorPreset
AnchorPreset = {
	TOP_LEFT = 0,
	TOP_CENTER = 1,
	TOP_RIGHT = 2,
	MIDDLE_LEFT = 3,
	CENTER = 4,
	MIDDLE_RIGHT = 5,
	BOTTOM_LEFT = 6,
	BOTTOM_CENTER = 7,
	BOTTOM_RIGHT = 8,
	HORIZONTAL_STRETCH_TOP = 9,
	HORIZONTAL_STRETCH_MIDDLE = 10,
	HORIZONTAL_STRETCH_BOTTOM = 11,
	VERTICAL_STRETCH_LEFT = 12,
	VERTICAL_STRETCH_CENTER = 13,
	VERTICAL_STRETCH_RIGHT = 14,
	STRETCH_BOTH = 15
}

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

--- Enables UI transform data on this transform
function Transform:EnableUIData() end

--- Disables UI transform data on this transform
function Transform:DisableUIData() end

--- Returns whether this transform carries UI data
---@return boolean
function Transform:HasUIData() end

--- Returns whether this transform is currently driven by a parent Canvas hierarchy
---@return boolean
function Transform:HasActiveUIData() end

--- Returns the anchored UI position
---@return Vector2
function Transform:GetUIPosition() end

--- Defines the anchored UI position
---@param position Vector2
function Transform:SetUIPosition(position) end

--- Returns the UI rotation in degrees
---@return number
function Transform:GetUIRotation() end

--- Defines the UI rotation in degrees
---@param rotation number
function Transform:SetUIRotation(rotation) end

--- Returns the UI scale
---@return Vector2
function Transform:GetUIScale() end

--- Defines the UI scale
---@param scale Vector2
function Transform:SetUIScale(scale) end

--- Returns the UI size
---@return Vector2
function Transform:GetUISize() end

--- Defines the UI size
---@param size Vector2
function Transform:SetUISize(size) end

--- Returns the normalized pivot in range [-1, 1]
---@return Vector2
function Transform:GetUIPivot() end

--- Defines the normalized pivot in range [-1, 1]
---@param pivot Vector2
function Transform:SetUIPivot(pivot) end

--- Returns the UI anchor preset
---@return AnchorPreset
function Transform:GetUIAnchorPreset() end

--- Defines the UI anchor preset
---@param anchorPreset AnchorPreset
function Transform:SetUIAnchorPreset(anchorPreset) end

--- Returns the actor that owns this component
---@return Actor
function Transform:GetOwner() end
