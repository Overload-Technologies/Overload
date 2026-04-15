---@meta

--- A component responsible for skeletal animation playback and skinning runtime control
---@class SkinnedMeshRenderer : Component
SkinnedMeshRenderer = {}

--- Returns the actor that owns this component
---@return Actor
function SkinnedMeshRenderer:GetOwner() end

--- Starts/resumes animation playback
function SkinnedMeshRenderer:Play() end

--- Pauses animation playback
function SkinnedMeshRenderer:Pause() end

--- Stops playback and resets time to start
function SkinnedMeshRenderer:Stop() end

--- Returns whether playback is active
---@return boolean
function SkinnedMeshRenderer:IsPlaying() end

--- Sets looping mode
---@param loop boolean
function SkinnedMeshRenderer:SetLooping(loop) end

--- Returns whether looping is enabled
---@return boolean
function SkinnedMeshRenderer:IsLooping() end

--- Sets animation playback speed
---@param speed number
function SkinnedMeshRenderer:SetPlaybackSpeed(speed) end

--- Returns current playback speed
---@return number
function SkinnedMeshRenderer:GetPlaybackSpeed() end

--- Sets the bounds scale used during frustum culling for skinned meshes
--- Values below 1.0 are clamped to 1.0
---@param scale number
function SkinnedMeshRenderer:SetMeshBoundsScale(scale) end

--- Returns the bounds scale used during frustum culling for skinned meshes
--- Returned value is always >= 1.0
---@return number
function SkinnedMeshRenderer:GetMeshBoundsScale() end

--- Sets playback time in seconds
---@param timeSeconds number
function SkinnedMeshRenderer:SetTime(timeSeconds) end

--- Returns playback time in seconds
---@return number
function SkinnedMeshRenderer:GetTime() end

--- Returns the number of available animation clips
---@return integer
function SkinnedMeshRenderer:GetAnimationCount() end

--- Returns clip name at index or nil
---@param index integer
---@return string|nil
function SkinnedMeshRenderer:GetAnimationName(index) end

--- Sets the active animation clip by index or name, returns true on success
---@overload fun(self: SkinnedMeshRenderer, index: integer|nil): boolean
---@overload fun(self: SkinnedMeshRenderer, name: string): boolean
---@return boolean
function SkinnedMeshRenderer:SetAnimation(...) end

--- Returns current clip index or nil
---@return integer|nil
function SkinnedMeshRenderer:GetActiveAnimationIndex() end

--- Returns current clip name or nil
---@return string|nil
function SkinnedMeshRenderer:GetActiveAnimationName() end

--- Returns the number of available bones
---@return integer
function SkinnedMeshRenderer:GetBoneCount() end

--- Returns bone name at index or nil
---@param index integer
---@return string|nil
function SkinnedMeshRenderer:GetBoneName(index) end

--- Returns bone index by name or nil
---@param name string
---@return integer|nil
function SkinnedMeshRenderer:GetBoneIndex(name) end

--- Returns local bone position or nil
---@param boneIndex integer
---@return Vector3|nil
function SkinnedMeshRenderer:GetBoneLocalPosition(boneIndex) end

--- Returns local bone rotation or nil
---@param boneIndex integer
---@return Quaternion|nil
function SkinnedMeshRenderer:GetBoneLocalRotation(boneIndex) end

--- Returns local bone scale or nil
---@param boneIndex integer
---@return Vector3|nil
function SkinnedMeshRenderer:GetBoneLocalScale(boneIndex) end

--- Sets local bone position
---@param boneIndex integer
---@param position Vector3
---@return boolean
function SkinnedMeshRenderer:SetBoneLocalPosition(boneIndex, position) end

--- Sets local bone rotation
---@param boneIndex integer
---@param rotation Quaternion
---@return boolean
function SkinnedMeshRenderer:SetBoneLocalRotation(boneIndex, rotation) end

--- Sets local bone scale
---@param boneIndex integer
---@param scale Vector3
---@return boolean
function SkinnedMeshRenderer:SetBoneLocalScale(boneIndex, scale) end
