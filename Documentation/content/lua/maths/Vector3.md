+++
title = 'Vector3'
+++

### Description
Mathematic representation of a 3D vector of floats

- C++ related class: [`OvMaths::FVector3`](https://github.com/Overload-Technologies/Overload/tree/main/Sources/Overload/OvMaths/include/OvMaths/FVector3.h)

### Variables
|Name|Type|Description|
|-|-|-|
|x|`number`|X component of the Vector3|
|y|`number`|Y component of the Vector3|
|z|`number`|Z component of the Vector3|

### Constructors
|Name|Input|Output|Description|
|-|-|-|-|
|new||`Vector3`|Create an instance of Vector3 with x, y and z equals to zero|
|new|`number`:&nbsp;x<br>`number`:&nbsp;y<br>`number`:&nbsp;z<br>|`Vector3`|Create an instance of Vector3 with the given values|

### Functions
|Name|Input|Output|Description|
|-|-|-|-|
|One||`Vector3`|Returns a (1,1,1) Vector3|
|Zero||`Vector3`|Returns a (0,0,0) Vector3|
|Forward||`Vector3`|Returns a (0,0,1) Vector3|
|Up||`Vector3`|Returns a (0,1,0) Vector3|
|Right||`Vector3`|Returns a (1,0,0) Vector3|
|Backward||`Vector3`|Returns a (0,0,-1) Vector3|
|Down||`Vector3`|Returns a (0,-1,0) Vector3|
|Left||`Vector3`|Returns a (-1,0,0) Vector3|
|Length|`Vector3`:&nbsp;instance<br>|`number`|Returns the length of the given Vector3|
|Dot|`Vector3`:&nbsp;a<br>`Vector3`:&nbsp;b<br>|`number`|Returns the dot product of the two given Vector3|
|Cross|`Vector3`:&nbsp;a<br>`Vector3`:&nbsp;b<br>|`Vector3`|Returns the cross product of the two given Vector3|
|Normalize|`Vector3`:&nbsp;instance<br>|`Vector3`|Returns the normalized version of the given Vector3|
|Lerp|`Vector3`:&nbsp;a<br>`Vector3`:&nbsp;instance<br>`number`:&nbsp;t<br>|`Vector3`|Calculates the linear interpolation between two Vector3 using the given alpha|
|AngleBetween|`Vector3`:&nbsp;a<br>`Vector3`:&nbsp;b<br>|`number`|Calculates the angle between two Vector3|
|Distance|`Vector3`:&nbsp;a<br>`b`:&nbsp;instance<br>|`number`|Returns the distance between the given Vector3|

### Operators
|Operation|Description|
|-|-|
|`Vector3 + Vector3`|Adds two Vector3|
|`Vector3 - Vector3`|Substracts two Vector3|
|`-Vector3`|Multiplies each components of a Vector3 by -1|
|`Vector3 * number`|Multiplies each component of a Vector3 by a number|
|`Vector3 / Vector3`|Divides each component of a Vector3 by a number|
|`to_string(Vector3)`|Converts a Vector3 to a string|
