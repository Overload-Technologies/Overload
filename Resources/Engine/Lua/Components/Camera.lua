---@meta

--- Represents a camera entity. Its position will determine its view matrix
---@class Camera : Component
Camera = {}

--- Returns the actor that owns this component
---@return Actor
function Camera:GetOwner() end

--- Returns the field of view of the camera (Perspective)
---@return number
function Camera:GetFov() end

--- Returns the size of the camera (Orthographic)
---@return number
function Camera:GetSize() end

--- Returns the distance between the camera and its near plane
---@return number
function Camera:GetNear() end

--- Returns the distance between the camera and its far plane
---@return number
function Camera:GetFar() end

--- Returns the background color used before rendering the scene with this camera
---@return Vector3
function Camera:GetClearColor() end

--- Defines the field of view of the camera (Perspective)
---@param fov number
function Camera:SetFov(fov) end

--- Defines the size of the camera (Orthographic)
---@param size number
function Camera:SetSize(size) end

--- Defines the distance between the camera and its near plane
---@param near number
function Camera:SetNear(near) end

--- Defines the distance between the camera and its far plane
---@param far number
function Camera:SetFar(far) end

--- Defines the background color used before rendering the scene with this camera
---@param color Vector3
function Camera:SetClearColor(color) end

--- Returns true if the camera is culling geometry outside of its frustum
---@return boolean
function Camera:HasFrustumGeometryCulling() end

--- Returns true if the camera is culling lights outside of its frustum
---@return boolean
function Camera:HasFrustumLightCulling() end

--- Returns the projection mode used by the camera (Orthographic or Perspective)
---@return ProjectionMode
function Camera:GetProjectionMode() end

--- Defines if the camera should cull geometry outside of its frustum
---@param enabled boolean
function Camera:SetFrustumGeometryCulling(enabled) end

--- Defines if the camera should cull lights outside of its frustum
---@param enabled boolean
function Camera:SetFrustumLightCulling(enabled) end

--- Defines the projection mode the camera should be using (Orthographic or Perspective)
---@param mode ProjectionMode
function Camera:SetProjectionMode(mode) end
