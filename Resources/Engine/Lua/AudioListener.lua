---@meta

--- Represents an audio listener. The distance between audio sources and audio listener will affect the sound attenuation of spatialized sounds
---@class AudioListener : Component
AudioListener = {}

--- Returns the actor that owns this component
---@return Actor
function AudioListener:GetOwner() end
