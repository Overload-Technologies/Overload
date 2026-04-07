---@meta

---@class DirectionalLight : Light
DirectionalLight = {}

---@return boolean
function DirectionalLight:GetCastShadow() end

---@param castShadow boolean
function DirectionalLight:SetCastShadow(castShadow) end

---@return number
function DirectionalLight:GetShadowAreaSize() end

---@param size number
function DirectionalLight:SetShadowAreaSize(size) end

---@return boolean
function DirectionalLight:GetShadowFollowCamera() end

---@param followCamera boolean
function DirectionalLight:SetShadowFollowCamera(followCamera) end

---@return integer
function DirectionalLight:GetShadowMapResolution() end

---@param resolution integer
function DirectionalLight:SetShadowMapResolution(resolution) end
