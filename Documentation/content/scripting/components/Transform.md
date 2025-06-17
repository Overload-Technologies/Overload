+++
title = 'Transform'
+++

### Description
Represents the 3D transformations applied to an actor

- C++ related class: [`OvCore::ECS::Components::CTransform`](https://github.com/Overload-Technologies/Overload/tree/main/Sources/Overload/OvCore/include/OvCore/ECS/Components/CTransform.h)
- Inherits from lua usertype: [`Component`](../Component)

### Variables
_This usertype has no variables_

### Constructors
|Name|Input|Output|Description|
|-|-|-|-|
|new||`Transform`||

### Functions
|Name|Input|Output|Description|
|-|-|-|-|
|SetPosition|`Transform`:&nbsp;instance<br>`Vector3`:&nbsp;position<br>||Alias for SetLocalPosition|
|SetRotation|`Transform`:&nbsp;instance<br>`Quaternion`:&nbsp;rotation<br>||Alias for SetLocalRotation|
|SetScale|`Transform`:&nbsp;instance<br>`Vector3`:&nbsp;scale<br>||Alias for SetLocalScale|
|SetLocalPosition|`Transform`:&nbsp;instance<br>`Vector3`:&nbsp;position<br>||Sets the position of the transform in the local space|
|SetLocalRotation|`Transform`:&nbsp;instance<br>`Quaternion`:&nbsp;rotation<br>||Sets the rotation of the transform in the local space|
|SetLocalScale|`Transform`:&nbsp;instance<br>`Vector3`:&nbsp;scale<br>||Sets the scale of the transform in the local space|
|SetWorldPosition|`Transform`:&nbsp;instance<br>`Vector3`:&nbsp;position<br>||Sets the position of the transform in world space|
|SetWorldRotation|`Transform`:&nbsp;instance<br>`Quaternion`:&nbsp;rotation<br>||Sets the rotation of the transform in world space|
|SetWorldScale|`Transform`:&nbsp;instance<br>`Vector3`:&nbsp;scale<br>||Sets the scale of the transform in world space|
|GetPosition|`Transform`:&nbsp;instance<br>|`Vector3`|Alias for GetWorldPosition|
|GetRotation|`Transform`:&nbsp;instance<br>|`Quaternion`|Alias for GetWorldRotation|
|GetScale|`Transform`:&nbsp;instance<br>|`Vector3`|Alias for GetWorldScale|
|GetLocalPosition|`Transform`:&nbsp;instance<br>|`Vector3`|Returns the position in local space|
|GetLocalRotation|`Transform`:&nbsp;instance<br>|`Quaternion`|Returns the rotation in local space|
|GetLocalScale|`Transform`:&nbsp;instance<br>|`Vector3`|Returns the scale in local space|
|GetWorldPosition|`Transform`:&nbsp;instance<br>|`Vector3`|Returns the position in world space|
|GetWorldRotation|`Transform`:&nbsp;instance<br>|`Quaternion`|Returns the rotation in world space|
|GetWorldScale|`Transform`:&nbsp;instance<br>|`Vector3`|Returns the scale in world space|
|GetForward|`Transform`:&nbsp;instance<br>|`Vector3`|Alias for GetWorldForward|
|GetUp|`Transform`:&nbsp;instance<br>|`Vector3`|Alias for GetWorldUp|
|GetRight|`Transform`:&nbsp;instance<br>|`Vector3`|Alias for GetWorldRight|
|GetLocalForward|`Transform`:&nbsp;instance<br>|`Vector3`|Returns the transform local forward|
|GetLocalUp|`Transform`:&nbsp;instance<br>|`Vector3`|Returns the transform local up|
|GetLocalRight|`Transform`:&nbsp;instance<br>|`Vector3`|Returns the transform local right|
|GetWorldForward|`Transform`:&nbsp;instance<br>|`Vector3`|Returns the transform world forward|
|GetWorldUp|`Transform`:&nbsp;instance<br>|`Vector3`|Returns the transform world up|
|GetWorldRight|`Transform`:&nbsp;instance<br>|`Vector3`|Returns the transform world right|

### Operators
_This usertype has no operators_
