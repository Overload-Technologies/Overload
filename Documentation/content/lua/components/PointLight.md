+++
title = 'PointLight'
+++

### Description
A light that is localized and has attenuation

- C++ related class: [`OvCore::ECS::Components::CPointLight`](https://github.com/Overload-Technologies/Overload/tree/main/Sources/Overload/OvCore/include/OvCore/ECS/Components/CPointLight.h)
- Inherits from lua usertype: [`Light`](../Light)

### Variables
_This usertype has no variables_

### Constructors
|Name|Input|Output|Description|
|-|-|-|-|
|new||`PointLight`||

### Functions
|Name|Input|Output|Description|
|-|-|-|-|
|GetConstant|`PointLight`:&nbsp;instance<br>|`number`|Returns the light constant|
|GetLinear|`PointLight`:&nbsp;instance<br>|`number`|Returns the light linear|
|GetQuadratic|`PointLight`:&nbsp;instance<br>|`number`|Returns the light quadratic|
|SetConstant|`PointLight`:&nbsp;instance<br>`number`:&nbsp;constant<br>||Sets the light constant|
|SetLinear|`PointLight`:&nbsp;instance<br>`number`:&nbsp;linear<br>||Sets the light linear|
|SetQuadratic|`PointLight`:&nbsp;instance<br>`number`:&nbsp;quadratic<br>||Sets the light quadratic|

### Operators
_This usertype has no operators_
