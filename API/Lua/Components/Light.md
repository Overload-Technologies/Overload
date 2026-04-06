### Description
Base class for any light

- C++ related class: [`OvCore::ECS::Components::CLight`](/Sources/OvCore/include/OvCore/ECS/Components/CLight.h)
- Inherits from lua usertype: [`Component`](Component.md)
- Inherited by lua usertypes:
    - [`PointLight`](PointLight.md)
    - [`SpotLight`](SpotLight.md)
    - [`DirectionalLight`](DirectionalLight.md)
    - [`AmbientBoxLight`](AmbientBoxLight.md)
    - [`AmbientSphereLight`](AmbientSphereLight.md)


### Variables
_This usertype has no variables_

### Constructors
|Name|Input|Output|Description|
|-|-|-|-|
|new||`Light`||

### Functions
|Name|Input|Output|Description|
|-|-|-|-|
|GetColor|`Light`:&nbsp;instance<br>|`Vector3`|Returns light color|
|GetIntensity|`Light`:&nbsp;instance<br>|`number`|Returns light intensity|
|SetColor|`Light`:&nbsp;instance<br>`Vector3`:&nbsp;color<br>||Defines light color|
|SetIntensity|`Light`:&nbsp;instance<br>`number`:&nbsp;intensity<br>||Defines light intensity|

### Operators
_This usertype has no operators_
