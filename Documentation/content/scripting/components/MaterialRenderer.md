+++
title = 'MaterialRenderer'
+++

### Description
A component that handle a material list, necessary for model rendering

- C++ related class: [`OvCore::ECS::Components::CMaterialRenderer`](https://github.com/Overload-Technologies/Overload/tree/main/Sources/Overload/OvCore/include/OvCore/ECS/Components/CMaterialRenderer.h)
- Inherits from lua usertype: [`Component`](../Component)

### Variables
_This usertype has no variables_

### Constructors
|Name|Input|Output|Description|
|-|-|-|-|
|new||`MaterialRenderer`|Default constructor|

### Functions
|Name|Input|Output|Description|
|-|-|-|-|
|SetMaterial|`MaterialRenderer`:&nbsp;instance<br>`number`:&nbsp;index<br>`Material`:&nbsp;material<br>||Defines the material to use for the given index|
|SetUserMatrixElement|`MaterialRenderer`:&nbsp;instance<br>`number`:&nbsp;row<br>`number`:&nbsp;column<br>`number`:&nbsp;value<br>||Defines an element of the user matrix|
|GetUserMatrixElement|`MaterialRenderer`:&nbsp;instance<br>`number`:&nbsp;row<br>`number`:&nbsp;column<br>|`number`|Returns an element of the user matrix|

### Operators
_This usertype has no operators_
