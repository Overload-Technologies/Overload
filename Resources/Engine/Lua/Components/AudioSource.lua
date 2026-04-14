---@meta

--- Represents an audio source. Its position in the world is important if the spatial sound settings is on
---@class AudioSource : Component
AudioSource = {}

--- Returns the actor that owns this component
---@return Actor
function AudioSource:GetOwner() end

--- Plays the attached sounds
function AudioSource:Play() end

--- Stops playback
function AudioSource:Stop() end

--- Suspend playback
function AudioSource:Pause() end

--- Resume the playback from where it get suspended
function AudioSource:Resume() end

--- Returns the sound attached to the audio source
---@return Sound|nil
function AudioSource:GetSound() end

--- Returns the current volume
---@return number
function AudioSource:GetVolume() end

--- Returns the pan
---@return number
function AudioSource:GetPan() end

--- Returns true if the AudioSource is setup to loop
---@return boolean
function AudioSource:IsLooped() end

--- Returns the pitch
---@return number
function AudioSource:GetPitch() end

--- Returns true if the audio source is currently playing
---@return boolean
function AudioSource:IsPlaying() end

--- Returns true if the audio source is spatialized
---@return boolean
function AudioSource:IsSpatial() end

--- Returns the audio source attenuation threshold (Minimum distance before applying sound attenuation)
---@return number
function AudioSource:GetAttenuationThreshold() end

--- Defines the sound to play on the audio source
---@param sound Sound
function AudioSource:SetSound(sound) end

--- Defines the audio source volume
---@param volume number
function AudioSource:SetVolume(volume) end

--- Defines the audio source pan (Left / Right)
---@param pan number
function AudioSource:SetPan(pan) end

--- Defines if the audio source should loop
---@param looped boolean
function AudioSource:SetLooped(looped) end

--- Defines the audio source pitch (= frequency or playback speed)
---@param pitch number
function AudioSource:SetPitch(pitch) end

--- Defines if the audio source should be spatialized or not
---@param spatial boolean
function AudioSource:SetSpatial(spatial) end

--- Defines the audio source attenuation threshold (Minimum distance before applying sound attenuation)
---@param threshold number
function AudioSource:SetAttenuationThreshold(threshold) end
