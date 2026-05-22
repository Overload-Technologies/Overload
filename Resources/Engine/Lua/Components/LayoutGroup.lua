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

--- Returns the minimum layout container size
---@return Vector2
function LayoutGroup:GetSize() end

--- Defines the minimum layout container size
---@param size Vector2
function LayoutGroup:SetSize(size) end

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
