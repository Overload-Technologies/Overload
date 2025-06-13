+++
title = 'SpotLight'
+++

### Description
A light that is localized, has attenuation and has a direction

- C++ related class: [`OvCore::ECS::Components::CSpotLight`](https://github.com/Overload-Technologies/Overload/tree/main/Sources/Overload/OvCore/include/OvCore/ECS/Components/CSpotLight.h)
- Inherits from lua usertype: [`Light`](../Light)

### Variables
_This usertype has no variables_

### Constructors
|Name|Input|Output|Description|
|-|-|-|-|
|new||`SpotLight`||

### Functions
|Name|Input|Output|Description|
|-|-|-|-|
|GetConstant|`SpotLight`:&nbsp;instance<br>|`number`|Returns the light constant|
|GetLinear|`SpotLight`:&nbsp;instance<br>|`number`|Returns the light linear|
|GetQuadratic|`SpotLight`:&nbsp;instance<br>|`number`|Returns the light quadratic|
|GetCutOff|`SpotLight`:&nbsp;instance<br>|`number`|Returns the light cutoff|
|GetOuterCutOff|`SpotLight`:&nbsp;instance<br>|`number`|Returns the light outercutoff|
|SetConstant|`SpotLight`:&nbsp;instance<br>`number`:&nbsp;constant<br>|||Sets the light constant|
|SetLinear|`SpotLight`:&nbsp;instance<br>`number`:&nbsp;linear<br>||Sets the light linear|
|SetQuadratic|`SpotLight`:&nbsp;instance<br>`number`:&nbsp;quadratic<br>||Sets the light quadratic|
|SetCutOff|`SpotLight`:&nbsp;instance<br>`number`:&nbsp;cutOff<br>||Sets the light cutoff|
|SetOuterCutOff|`SpotLight`:&nbsp;instance<br>`number`:&nbsp;outerCutfOff<br>||Sets the light outercutoff|

### Operators
_This usertype has no operators_
