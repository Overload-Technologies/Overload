---@meta

---@class SkinnedMeshRenderer : Component
SkinnedMeshRenderer = {}

---@return Actor
function SkinnedMeshRenderer:GetOwner() end

function SkinnedMeshRenderer:Play() end

function SkinnedMeshRenderer:Pause() end

function SkinnedMeshRenderer:Stop() end

---@return boolean
function SkinnedMeshRenderer:IsPlaying() end

---@param loop boolean
function SkinnedMeshRenderer:SetLooping(loop) end

---@return boolean
function SkinnedMeshRenderer:IsLooping() end

---@param speed number
function SkinnedMeshRenderer:SetPlaybackSpeed(speed) end

---@return number
function SkinnedMeshRenderer:GetPlaybackSpeed() end

---@param timeSeconds number
function SkinnedMeshRenderer:SetTime(timeSeconds) end

---@return number
function SkinnedMeshRenderer:GetTime() end

---@return integer
function SkinnedMeshRenderer:GetAnimationCount() end

---@param index integer
---@return string|nil
function SkinnedMeshRenderer:GetAnimationName(index) end

---@overload fun(self: SkinnedMeshRenderer, index: integer|nil): boolean
---@overload fun(self: SkinnedMeshRenderer, name: string): boolean
---@return boolean
function SkinnedMeshRenderer:SetAnimation(...) end

---@return integer|nil
function SkinnedMeshRenderer:GetActiveAnimationIndex() end

---@return string|nil
function SkinnedMeshRenderer:GetActiveAnimationName() end
