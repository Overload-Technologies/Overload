+++
title = 'DirectionalLight'
+++

### Description
A simple light that has no attenuation and that has a direction

- C++ related class: [`OvCore::ECS::Components::CDirectionalLight`](https://github.com/Overload-Technologies/Overload/tree/main/Sources/Overload/OvCore/include/OvCore/ECS/Components/CDirectionalLight.h)
- Inherits from lua usertype: [`Light`](../Light)

### Variables
_This usertype has no variables_

### Constructors
|Name|Input|Output|Description|
|-|-|-|-|
|new||`DirectionalLight`|Default constructor|

### Functions
|Name|Input|Output|Description|
|-|-|-|-|
|GetCastShadow|`DirectionalLight`:&nbsp;instance<br>|`boolean`|Returns true if the light should cast shadows|
|SetCastShadow|`DirectionalLight`:&nbsp;instance<br>`boolean`:&nbsp;enabled||Defines if the light should cast shadows|
|GetShadowAreaSize|`DirectionalLight`:&nbsp;instance<br>|`number`|Returns the area size of the shadow|
|SetShadowAreaSize|`DirectionalLight`:&nbsp;instance<br>`number`:&nbsp;size||Defines the area size of the shadow|
|GetShadowFollowCamera|`DirectionalLight`:&nbsp;instance<br>|`boolean`|Returns true if the light position should snap to the camera position|
|SetShadowFollowCamera|`DirectionalLight`:&nbsp;instance<br>`boolean`:&nbsp;enabled||Defines whether or not the light position should snap to the camera position|
|GetShadowMapResolution|`DirectionalLight`:&nbsp;instance<br>|`number`|Returns the shadow map resolution|
|SetShadowMapResolution|`DirectionalLight`:&nbsp;instance<br>`number`:&nbsp;resolution||Sets the shadow map resolution (The resolution should be a power of 2 for better results)|

### Operators
_This usertype has no operators_
