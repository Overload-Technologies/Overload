+++
title = 'ReflectionProbe'
+++

### Description
Component that captures the surrounding environment into a cubemap texture.

- C++ related class: [`OvCore::ECS::Components::CReflectionProbe`](https://github.com/Overload-Technologies/Overload/blob/main/Sources/Overload/OvCore/include/OvCore/ECS/Components/CReflectionProbe.h)
- Inherits from lua usertype: [`Component`](../Component)

### Variables
_This usertype has no variables_

### Constructors
_This usertype has no constructors_

### Functions
|Name|Input|Output|Description|
|-|-|-|-|
|SetRefreshMode|`ReflectionProbe`: instance<br>`ReflectionProbeRefreshMode`: mode||Sets the refresh mode of the reflection probe|
|GetRefreshMode|`ReflectionProbe`: instance|`ReflectionProbeRefreshMode`|Returns the refresh mode of the reflection probe|
|SetCaptureSpeed|`ReflectionProbe`: instance<br>`ReflectionProbeCaptureSpeed`: speed||Determines how many faces the reflection probe should capture per frame|
|GetCaptureSpeed|`ReflectionProbe`: instance|`ReflectionProbeCaptureSpeed`|Returns the capture speed (number of faces captured per frame)|
|SetCubemapResolution|`ReflectionProbe`: instance<br>`number`: resolution||Sets the cubemap resolution (must be a power of 2)|
|GetCubemapResolution|`ReflectionProbe`: instance|`number`|Returns the cubemap resolution|
|SetCapturePosition|`ReflectionProbe`: instance<br>`Vector3`: position||Sets the position offset for the capture of the reflection probe|
|GetCapturePosition|`ReflectionProbe`: instance|`Vector3`|Returns the position offset for the capture of the reflection probe|
|SetBrightness|`ReflectionProbe`: instance<br>`numeric`: brightness||Sets the brightness of the reflection probe|
|GetBrightness|`ReflectionProbe`: instance|`numeric`|Returns the brightness of the reflection probe|
|SetInfluencePolicy|`ReflectionProbe`: instance<br>`ReflectionProbeInfluencePolicy`: policy||Determines the influence policy of the reflection probe|
|GetInfluencePolicy|`ReflectionProbe`: instance|`ReflectionProbeInfluencePolicy`|Returns the influence policy of the reflection probe|
|SetInfluenceSize|`ReflectionProbe`: instance<br>`Vector3`: size||Sets the size of the influence volume of the reflection probe|
|GetInfluenceSize|`ReflectionProbe`: instance|`Vector3`|Returns the size of the reflection probe volume|
|SetBoxProjection|`ReflectionProbe`: instance<br>`boolean`: enabled||Sets if the reflection probe should use box projection|
|IsBoxProjectionEnabled|`ReflectionProbe`: instance|`boolean`|Returns if the reflection probe uses box projection|
|RequestCapture|`ReflectionProbe`: instance<br>`boolean`: forceImmediate = false||Requests the cubemap to be updated|
|GetCubemap|`ReflectionProbe`: instance|`Texture`|Returns the last complete cubemap captured by the reflection probe|

### Operators
_This usertype has no operators_
