+++
title = 'PostProcessStack'
+++

### Description
Component that holds settings values for each post-processing effect.

- C++ related class: [`OvCore::ECS::Components::CPostProcessStack`](https://github.com/Overload-Technologies/Overload/tree/main/Sources/Overload/OvCore/include/OvCore/ECS/Components/CPostProcessStack.h)
- Inherits from lua usertype: [`Component`](../Component)

### Variables
_This usertype has no variables_

### Constructors
|Name|Input|Output|Description|
|-|-|-|-|
|new||`PostProcessStack`|Default Constructor|

### Functions
|Name|Input|Output|Description|
|-|-|-|-|
|GetTonemappingSettings|`PostProcessStack`:&nbsp;instance<br>|[`TonemappingSettings`](TonemappingSettings)||
|GetBloomSettings|`PostProcessStack`:&nbsp;instance<br>|[`BloomSettings`](BloomSettings)||
|GetAutoExposureSettings|`PostProcessStack`:&nbsp;instance<br>|[`AutoExposureSettings`](AutoExposureSettings)||
|GetFXAASettings|`PostProcessStack`:&nbsp;instance<br>|[`FXAASettings`](FXAASettings)||
|SetTonemappingSettings|`PostProcessStack`:&nbsp;instance<br>[`TonemappingSettings`](TonemappingSettings):&nbsp;settings|||
|SetBloomSettings|`PostProcessStack`:&nbsp;instance<br>[`BloomSettings`](BloomSettings):&nbsp;settings|||
|SetAutoExposureSettings|`PostProcessStack`:&nbsp;instance<br>[`AutoExposureSettings`](AutoExposureSettings):&nbsp;settings|||
|SetFXAASettings|`PostProcessStack`:&nbsp;instance<br>[`FXAASettings`](FXAASettings):&nbsp;settings|||


### Operators
_This usertype has no operators_
