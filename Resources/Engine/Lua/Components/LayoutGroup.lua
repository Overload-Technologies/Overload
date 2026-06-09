---@meta

--- Defines how a LayoutGroup arranges direct UI children
---@enum LayoutDirection
LayoutDirection = {
	HORIZONTAL = 0,
	VERTICAL = 1
}

---@enum LayoutHorizontalAlignment
LayoutHorizontalAlignment = {
	LEFT = 0,
	CENTER = 1,
	RIGHT = 2
}

---@enum LayoutVerticalAlignment
LayoutVerticalAlignment = {
	TOP = 0,
	CENTER = 1,
	BOTTOM = 2
}

--- Arranges direct user interface children along an axis
---@class LayoutGroup : Component
LayoutGroup = {}

--- Returns the actor that owns this component
---@return Actor
function LayoutGroup:GetOwner() end

--- Returns the layout direction
---@return LayoutDirection
function LayoutGroup:GetDirection() end

--- Defines the layout direction
---@param direction LayoutDirection
function LayoutGroup:SetDirection(direction) end

--- Returns the spacing between children
---@return number
function LayoutGroup:GetSpacing() end

--- Defines the non-negative spacing between children
---@param spacing number
function LayoutGroup:SetSpacing(spacing) end

--- Returns the layout padding as left, right, top, bottom
---@return Vector4
function LayoutGroup:GetPadding() end

--- Defines the layout padding as left, right, top, bottom
---@param padding Vector4
function LayoutGroup:SetPadding(padding) end

--- Returns the horizontal children alignment
---@return LayoutHorizontalAlignment
function LayoutGroup:GetHorizontalAlignment() end

--- Defines the horizontal children alignment
---@param alignment LayoutHorizontalAlignment
function LayoutGroup:SetHorizontalAlignment(alignment) end

--- Returns the vertical children alignment
---@return LayoutVerticalAlignment
function LayoutGroup:GetVerticalAlignment() end

--- Defines the vertical children alignment
---@param alignment LayoutVerticalAlignment
function LayoutGroup:SetVerticalAlignment(alignment) end

--- Returns whether the layout controls children width
---@return boolean
function LayoutGroup:GetControlChildrenWidth() end

--- Defines whether the layout controls children width
---@param value boolean
function LayoutGroup:SetControlChildrenWidth(value) end

--- Returns whether the layout controls children height
---@return boolean
function LayoutGroup:GetControlChildrenHeight() end

--- Defines whether the layout controls children height
---@param value boolean
function LayoutGroup:SetControlChildrenHeight(value) end

--- Returns whether the layout expands children width
---@return boolean
function LayoutGroup:GetForceExpandWidth() end

--- Defines whether the layout expands children width
---@param value boolean
function LayoutGroup:SetForceExpandWidth(value) end

--- Returns whether the layout expands children height
---@return boolean
function LayoutGroup:GetForceExpandHeight() end

--- Defines whether the layout expands children height
---@param value boolean
function LayoutGroup:SetForceExpandHeight(value) end
