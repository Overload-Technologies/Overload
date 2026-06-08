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

--- Returns the image display size stored by Transform2D
---@return Vector2
function Image:GetSize() end

--- Defines the image display size stored by Transform2D
---@param size Vector2
function Image:SetSize(size) end

--- Returns the image tint
---@return Vector4
function Image:GetTint() end

--- Defines the image tint
---@param tint Vector4
function Image:SetTint(tint) end
