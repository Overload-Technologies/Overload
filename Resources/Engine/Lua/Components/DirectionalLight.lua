---@meta

--- A simple light that has no attenuation and that has a direction
---@class DirectionalLight : Light
DirectionalLight = {}

--- Returns true if the light should cast shadows
---@return boolean
function DirectionalLight:GetCastShadow() end

--- Defines if the light should cast shadows
---@param castShadow boolean
function DirectionalLight:SetCastShadow(castShadow) end

--- Returns the area size of the shadow
---@return number
function DirectionalLight:GetShadowAreaSize() end

--- Defines the area size of the shadow
---@param size number
function DirectionalLight:SetShadowAreaSize(size) end

--- Returns true if the light position should snap to the camera position
---@return boolean
function DirectionalLight:GetShadowFollowCamera() end

--- Defines whether or not the light position should snap to the camera position
---@param followCamera boolean
function DirectionalLight:SetShadowFollowCamera(followCamera) end

--- Returns the shadow map resolution
---@return integer
function DirectionalLight:GetShadowMapResolution() end

--- Sets the shadow map resolution (The resolution should be a power of 2 for better results)
---@param resolution integer
function DirectionalLight:SetShadowMapResolution(resolution) end
