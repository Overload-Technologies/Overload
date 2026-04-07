---@meta

--- Tonemapping modes
---@enum TonemappingMode
TonemappingMode = {
    NEUTRAL = 0,
    REINHARD = 1,
    REINHARD_JODIE = 2,
    UNCHARTED2 = 3,
    UNCHARTED2_FILMIC = 4,
    ACES = 5,
}

--- Base effect settings structure
---@class EffectSettings
---@field Enabled boolean
EffectSettings = {}

--- Contains all the settings for the bloom effect
---@class BloomSettings : EffectSettings
---@field Intensity number
---@field Passes integer
BloomSettings = {}

--- Contains all the settings for the auto-exposure effect
---@class AutoExposureSettings : EffectSettings
---@field CenterWeightBias number
---@field MinLuminanceEV number
---@field MaxLuminanceEV number
---@field ExposureCompensationEV number
---@field Progressive boolean
---@field SpeedDown number
---@field SpeedUp number
AutoExposureSettings = {}

--- Contains all the settings for the tone mapping effect
---@class TonemappingSettings : EffectSettings
---@field Exposure number
---@field Mode TonemappingMode
---@field GammaCorrection number
TonemappingSettings = {}

--- Contains all the settings for the FXAA effect
---@class FXAASettings : EffectSettings
FXAASettings = {}

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
