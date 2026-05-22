---@meta

---@enum TextHorizontalAlignment
TextHorizontalAlignment = {
	LEFT = 0,
	CENTER = 1,
	RIGHT = 2
}

---@enum TextVerticalAlignment
TextVerticalAlignment = {
	TOP = 0,
	CENTER = 1,
	BOTTOM = 2
}

--- Represents a renderable user interface text
---@class Text : Component
Text = {}

--- Returns the actor that owns this component
---@return Actor
function Text:GetOwner() end

--- Returns the text content
---@return string
function Text:GetText() end

--- Defines the text content
---@param text string
function Text:SetText(text) end

--- Returns the font resource path
---@return string
function Text:GetFontPath() end

--- Defines the font resource path
---@param fontPath string
function Text:SetFontPath(fontPath) end

--- Returns the font size in canvas pixels
---@return number
function Text:GetFontSize() end

--- Defines the font size in canvas pixels
---@param fontSize number
function Text:SetFontSize(fontSize) end

--- Returns the text color
---@return Vector4
function Text:GetColor() end

--- Defines the text color
---@param color Vector4
function Text:SetColor(color) end

--- Returns the text extents in canvas pixels
---@return Vector2
function Text:GetExtents() end

--- Defines the text extents in canvas pixels
---@param extents Vector2
function Text:SetExtents(extents) end

--- Returns the horizontal text alignment
---@return TextHorizontalAlignment
function Text:GetHorizontalAlignment() end

--- Defines the horizontal text alignment
---@param alignment TextHorizontalAlignment
function Text:SetHorizontalAlignment(alignment) end

--- Returns the vertical text alignment
---@return TextVerticalAlignment
function Text:GetVerticalAlignment() end

--- Defines the vertical text alignment
---@param alignment TextVerticalAlignment
function Text:SetVerticalAlignment(alignment) end
