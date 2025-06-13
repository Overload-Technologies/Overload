+++
title = 'Camera'
+++

### Description
Represents a camera entity. Its position will determine its view matrix

- C++ related class: [`OvCore::ECS::Components::CCamera`](https://github.com/Overload-Technologies/Overload/tree/main/Sources/Overload/OvCore/include/OvCore/ECS/Components/CCamera.h)
- Inherits from lua usertype: [`Component`](../Component)

### Variables
_This usertype has no variables_

### Constructors
|Name|Input|Output|Description|
|-|-|-|-|
|new||`Camera`|Default constructor|

### Functions
|Name|Input|Output|Description|
|-|-|-|-|
|GetFov|`Camera`:&nbsp;instance<br>|`number`|Returns the field of view of the camera (Perspective)|
|GetSize|`Camera`:&nbsp;instance<br>|`number`|Returns the size of the camera (Orthographic)|
|GetNear|`Camera`:&nbsp;instance<br>|`number`|Returns the distance between the camera and its near plane|
|GetFar|`Camera`:&nbsp;instance<br>|`number`|Returns the distance between the camera and its far plane|
|GetClearColor|`Camera`:&nbsp;instance<br>|`Vector3`|Returns the background color used before rendering the scene with this camera|
|SetFov|`Camera`:&nbsp;instance<br>`number`:&nbsp;fov<br>||Defines the field of view of the camera (Perspective)|
|SetSize|`Camera`:&nbsp;instance<br>`number`:&nbsp;size<br>||Defines the size of the camera (Orthographic)|
|SetNear|`Camera`:&nbsp;instance<br>`number`:&nbsp;near<br>||Defines the distance between the camera and its near plane|
|SetFar|`Camera`:&nbsp;instance<br>`number`:&nbsp;far<br>||Defines the distance between the camera and its far plane|
|SetClearColor|`Camera`:&nbsp;instance<br>`Vector3`:&nbsp;color<br>||Defines the background color used before rendering the scene with this camera|
|HasFrustumGeometryCulling|`Camera`:&nbsp;instance<br>|`boolean`|Returns true if the camera is culling geometry outside of its frustum|
|HasFrustumLightCulling|`Camera`:&nbsp;instance<br>|`boolean`|Returns true if the camera is culling lights outside of its frustum|
|GetProjectionMode|`Camera`:&nbsp;instance<br>|`ProjectionMode`|Returns the projection mode used by the camera (Orthographic or Perspective)|
|SetFrustumGeometryCulling|`Camera`:&nbsp;instance<br>`boolean`:&nbsp;cull<br>||Defines if the camera should cull geometry outside of its frustum|
|SetFrustumLightCulling|`Camera`:&nbsp;instance<br>`boolean`:&nbsp;cull<br>||Defines if the camera should cull lights outside of its frustum|
|SetProjectionMode|`Camera`:&nbsp;instance<br>`ProjectionMode`:&nbsp;mode<br>||Defines the projection mode the camera should be using (Orthographic or Perspective)|

### Operators
_This usertype has no operators_
