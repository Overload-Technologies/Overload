---@meta

---@class AudioSource : Component
AudioSource = {}

---@return Actor
function AudioSource:GetOwner() end

function AudioSource:Play() end

function AudioSource:Stop() end

function AudioSource:Pause() end

function AudioSource:Resume() end

---@return userdata|nil
function AudioSource:GetSound() end

---@return number
function AudioSource:GetVolume() end

---@return number
function AudioSource:GetPan() end

---@return boolean
function AudioSource:IsLooped() end

---@return number
function AudioSource:GetPitch() end

---@return boolean
function AudioSource:IsPlaying() end

---@return boolean
function AudioSource:IsSpatial() end

---@return number
function AudioSource:GetAttenuationThreshold() end

---@param sound userdata
function AudioSource:SetSound(sound) end

---@param volume number
function AudioSource:SetVolume(volume) end

---@param pan number
function AudioSource:SetPan(pan) end

---@param looped boolean
function AudioSource:SetLooped(looped) end

---@param pitch number
function AudioSource:SetPitch(pitch) end

---@param spatial boolean
function AudioSource:SetSpatial(spatial) end

---@param threshold number
function AudioSource:SetAttenuationThreshold(threshold) end
