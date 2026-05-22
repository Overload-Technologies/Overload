---@meta

--- Represents 2D transformations applied to a user interface actor
---@class Transform2D : Component
Transform2D = {}

--- Defines the anchor preset used by Transform2D
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
	BOTTOM_RIGHT = 8
}

--- Returns the actor that owns this component
---@return Actor
function Transform2D:GetOwner() end

--- Returns the 2D position
---@return Vector2
function Transform2D:GetPosition() end

--- Defines the 2D position
---@param position Vector2
function Transform2D:SetPosition(position) end

--- Returns the 2D rotation in degrees
---@return number
function Transform2D:GetRotation() end

--- Defines the 2D rotation in degrees
---@param rotation number
function Transform2D:SetRotation(rotation) end

--- Returns the 2D scale
---@return Vector2
function Transform2D:GetScale() end

--- Defines the 2D scale
---@param scale Vector2
function Transform2D:SetScale(scale) end

--- Returns the anchor preset
---@return AnchorPreset
function Transform2D:GetAnchorPreset() end

--- Defines the anchor preset
---@param anchorPreset AnchorPreset
function Transform2D:SetAnchorPreset(anchorPreset) end
