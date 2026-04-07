---@meta

---@enum ProjectionMode
ProjectionMode = {
    ORTHOGRAPHIC = 0,
    PERSPECTIVE = 1,
}

---@class Camera : Component
Camera = {}

---@return Actor
function Camera:GetOwner() end

---@return number
function Camera:GetFov() end

---@return number
function Camera:GetSize() end

---@return number
function Camera:GetNear() end

---@return number
function Camera:GetFar() end

---@return Vector3
function Camera:GetClearColor() end

---@param fov number
function Camera:SetFov(fov) end

---@param size number
function Camera:SetSize(size) end

---@param near number
function Camera:SetNear(near) end

---@param far number
function Camera:SetFar(far) end

---@param color Vector3
function Camera:SetClearColor(color) end

---@return boolean
function Camera:HasFrustumGeometryCulling() end

---@return boolean
function Camera:HasFrustumLightCulling() end

---@return ProjectionMode
function Camera:GetProjectionMode() end

---@param enabled boolean
function Camera:SetFrustumGeometryCulling(enabled) end

---@param enabled boolean
function Camera:SetFrustumLightCulling(enabled) end

---@param mode ProjectionMode
function Camera:SetProjectionMode(mode) end
