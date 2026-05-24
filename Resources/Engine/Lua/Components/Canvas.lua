---@meta

--- Defines how a Canvas scales UI elements
---@enum CanvasScalerMode
CanvasScalerMode = {
	CONSTANT_PIXEL_SIZE = 0,
	SCALE_WITH_SCREEN_SIZE = 1
}

---@enum CanvasScreenMatchMode
CanvasScreenMatchMode = {
	MATCH_WIDTH_OR_HEIGHT = 0,
	EXPAND = 1,
	SHRINK = 2
}

--- Represents a root canvas for in-game user interface elements
---@class Canvas : Component
Canvas = {}

--- Returns the actor that owns this component
---@return Actor
function Canvas:GetOwner() end

--- Returns the reference resolution used by the canvas
---@return Vector2
function Canvas:GetReferenceResolution() end

--- Defines the reference resolution used by the canvas
---@param referenceResolution Vector2
function Canvas:SetReferenceResolution(referenceResolution) end

--- Returns the canvas scale factor
---@return number
function Canvas:GetScaleFactor() end

--- Defines the canvas scale factor
---@param scaleFactor number
function Canvas:SetScaleFactor(scaleFactor) end

--- Returns the number of UI pixels represented by one world unit
---@return number
function Canvas:GetPixelsPerUnit() end

--- Defines the number of UI pixels represented by one world unit
---@param pixelsPerUnit number
function Canvas:SetPixelsPerUnit(pixelsPerUnit) end

--- Returns the canvas scaler mode
---@return CanvasScalerMode
function Canvas:GetScalerMode() end

--- Defines the canvas scaler mode
---@param scalerMode CanvasScalerMode
function Canvas:SetScalerMode(scalerMode) end

--- Returns the screen match mode used with SCALE_WITH_SCREEN_SIZE
---@return CanvasScreenMatchMode
function Canvas:GetScreenMatchMode() end

--- Defines the screen match mode used with SCALE_WITH_SCREEN_SIZE
---@param screenMatchMode CanvasScreenMatchMode
function Canvas:SetScreenMatchMode(screenMatchMode) end

--- Returns the match width/height factor in range [0, 1]
---@return number
function Canvas:GetMatchWidthOrHeight() end

--- Defines the match width/height factor in range [0, 1]
---@param value number
function Canvas:SetMatchWidthOrHeight(value) end
