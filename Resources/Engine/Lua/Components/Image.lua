---@meta

--- Represents a renderable user interface image
---@class Image : Component
Image = {}

--- Returns the actor that owns this component
---@return Actor
function Image:GetOwner() end

--- Returns the texture rendered by the image
---@return Texture|nil
function Image:GetTexture() end

--- Defines the texture rendered by the image
---@param texture Texture|nil
function Image:SetTexture(texture) end

--- Returns the image display size stored by Transform UI data
---@return Vector2
function Image:GetSize() end

--- Defines the image display size stored by Transform UI data
---@param size Vector2
function Image:SetSize(size) end

--- Returns the image tint
---@return Vector4
function Image:GetTint() end

--- Defines the image tint
---@param tint Vector4
function Image:SetTint(tint) end

--- Returns whether the image keeps its intrinsic aspect ratio when resized
---@return boolean
function Image:GetPreserveAspect() end

--- Defines whether the image keeps its intrinsic aspect ratio when resized
---@param preserveAspect boolean
function Image:SetPreserveAspect(preserveAspect) end
