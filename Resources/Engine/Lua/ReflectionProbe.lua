---@meta

---@enum ReflectionProbeRefreshMode
ReflectionProbeRefreshMode = {
    REALTIME = 0,
    ONCE = 1,
    ON_DEMAND = 2,
}

---@enum ReflectionProbeCaptureSpeed
ReflectionProbeCaptureSpeed = {
    ONE_FACE = 0,
    TWO_FACES = 1,
    THREE_FACES = 2,
    SIX_FACES = 3,
}

---@enum ReflectionProbeInfluencePolicy
ReflectionProbeInfluencePolicy = {
    GLOBAL = 0,
    LOCAL = 1,
}

---@class ReflectionProbe : Component
ReflectionProbe = {}

---@return Actor
function ReflectionProbe:GetOwner() end

---@param mode ReflectionProbeRefreshMode
function ReflectionProbe:SetRefreshMode(mode) end

---@return ReflectionProbeRefreshMode
function ReflectionProbe:GetRefreshMode() end

---@param speed ReflectionProbeCaptureSpeed
function ReflectionProbe:SetCaptureSpeed(speed) end

---@return ReflectionProbeCaptureSpeed
function ReflectionProbe:GetCaptureSpeed() end

---@param resolution integer
function ReflectionProbe:SetCubemapResolution(resolution) end

---@return integer
function ReflectionProbe:GetCubemapResolution() end

---@param position Vector3
function ReflectionProbe:SetCapturePosition(position) end

---@return Vector3
function ReflectionProbe:GetCapturePosition() end

---@param policy ReflectionProbeInfluencePolicy
function ReflectionProbe:SetInfluencePolicy(policy) end

---@return ReflectionProbeInfluencePolicy
function ReflectionProbe:GetInfluencePolicy() end

---@param size Vector3
function ReflectionProbe:SetInfluenceSize(size) end

---@return Vector3
function ReflectionProbe:GetInfluenceSize() end

---@param enabled boolean
function ReflectionProbe:SetBoxProjection(enabled) end

---@return boolean
function ReflectionProbe:IsBoxProjectionEnabled() end

function ReflectionProbe:RequestCapture() end

---@return userdata|nil
function ReflectionProbe:GetCubemap() end
