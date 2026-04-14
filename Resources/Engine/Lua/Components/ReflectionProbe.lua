---@meta

--- Component that captures the surrounding environment into a cubemap texture
---@class ReflectionProbe : Component
ReflectionProbe = {}

--- Returns the actor that owns this component
---@return Actor
function ReflectionProbe:GetOwner() end

--- Sets the refresh mode of the reflection probe
---@param mode ReflectionProbeRefreshMode
function ReflectionProbe:SetRefreshMode(mode) end

--- Returns the refresh mode of the reflection probe
---@return ReflectionProbeRefreshMode
function ReflectionProbe:GetRefreshMode() end

--- Determines how many faces the reflection probe should capture per frame
---@param speed ReflectionProbeCaptureSpeed
function ReflectionProbe:SetCaptureSpeed(speed) end

--- Returns the capture speed (number of faces captured per frame)
---@return ReflectionProbeCaptureSpeed
function ReflectionProbe:GetCaptureSpeed() end

--- Sets the cubemap resolution (must be a power of 2)
---@param resolution integer
function ReflectionProbe:SetCubemapResolution(resolution) end

--- Returns the cubemap resolution
---@return integer
function ReflectionProbe:GetCubemapResolution() end

--- Sets the position offset for the capture of the reflection probe
---@param position Vector3
function ReflectionProbe:SetCapturePosition(position) end

--- Returns the position offset for the capture of the reflection probe
---@return Vector3
function ReflectionProbe:GetCapturePosition() end

--- Determines the influence policy of the reflection probe
---@param policy ReflectionProbeInfluencePolicy
function ReflectionProbe:SetInfluencePolicy(policy) end

--- Returns the influence policy of the reflection probe
---@return ReflectionProbeInfluencePolicy
function ReflectionProbe:GetInfluencePolicy() end

--- Sets the size of the influence volume of the reflection probe
---@param size Vector3
function ReflectionProbe:SetInfluenceSize(size) end

--- Returns the size of the reflection probe volume
---@return Vector3
function ReflectionProbe:GetInfluenceSize() end

--- Sets if the reflection probe should use box projection
---@param enabled boolean
function ReflectionProbe:SetBoxProjection(enabled) end

--- Returns if the reflection probe uses box projection
---@return boolean
function ReflectionProbe:IsBoxProjectionEnabled() end

--- Requests the cubemap to be updated
function ReflectionProbe:RequestCapture() end

--- Returns the last complete cubemap captured by the reflection probe
---@return Texture|nil
function ReflectionProbe:GetCubemap() end
