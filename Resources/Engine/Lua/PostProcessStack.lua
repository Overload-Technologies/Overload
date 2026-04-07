---@meta

---@enum TonemappingMode
TonemappingMode = {
    NEUTRAL = 0,
    REINHARD = 1,
    REINHARD_JODIE = 2,
    UNCHARTED2 = 3,
    UNCHARTED2_FILMIC = 4,
    ACES = 5,
}

---@class EffectSettings
---@field Enabled boolean
EffectSettings = {}

---@class BloomSettings : EffectSettings
---@field Intensity number
---@field Passes integer
BloomSettings = {}

---@class AutoExposureSettings : EffectSettings
---@field CenterWeightBias number
---@field MinLuminanceEV number
---@field MaxLuminanceEV number
---@field ExposureCompensationEV number
---@field Progressive boolean
---@field SpeedDown number
---@field SpeedUp number
AutoExposureSettings = {}

---@class TonemappingSettings : EffectSettings
---@field Exposure number
---@field Mode TonemappingMode
---@field GammaCorrection number
TonemappingSettings = {}

---@class FXAASettings : EffectSettings
FXAASettings = {}

---@class PostProcessStack : Component
PostProcessStack = {}

---@return Actor
function PostProcessStack:GetOwner() end

---@return TonemappingSettings
function PostProcessStack:GetTonemappingSettings() end

---@return BloomSettings
function PostProcessStack:GetBloomSettings() end

---@return AutoExposureSettings
function PostProcessStack:GetAutoExposureSettings() end

---@return FXAASettings
function PostProcessStack:GetFXAASettings() end

---@param settings TonemappingSettings
function PostProcessStack:SetTonemappingSettings(settings) end

---@param settings BloomSettings
function PostProcessStack:SetBloomSettings(settings) end

---@param settings AutoExposureSettings
function PostProcessStack:SetAutoExposureSettings(settings) end

---@param settings FXAASettings
function PostProcessStack:SetFXAASettings(settings) end
