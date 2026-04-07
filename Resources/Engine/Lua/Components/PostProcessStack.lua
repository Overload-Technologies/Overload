---@meta

--- Component that holds settings values for each post-processing effect
---@class PostProcessStack : Component
PostProcessStack = {}

--- Returns the actor that owns this component
---@return Actor
function PostProcessStack:GetOwner() end

--- Returns the tonemapping settings
---@return TonemappingSettings
function PostProcessStack:GetTonemappingSettings() end

--- Returns the bloom settings
---@return BloomSettings
function PostProcessStack:GetBloomSettings() end

--- Returns the auto-exposure settings
---@return AutoExposureSettings
function PostProcessStack:GetAutoExposureSettings() end

--- Returns the FXAA settings
---@return FXAASettings
function PostProcessStack:GetFXAASettings() end

--- Defines the tonemapping settings
---@param settings TonemappingSettings
function PostProcessStack:SetTonemappingSettings(settings) end

--- Defines the bloom settings
---@param settings BloomSettings
function PostProcessStack:SetBloomSettings(settings) end

--- Defines the auto-exposure settings
---@param settings AutoExposureSettings
function PostProcessStack:SetAutoExposureSettings(settings) end

--- Defines the FXAA settings
---@param settings FXAASettings
function PostProcessStack:SetFXAASettings(settings) end
