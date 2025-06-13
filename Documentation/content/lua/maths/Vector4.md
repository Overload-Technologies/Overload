+++
title = 'Vector4'
+++

### Description
Mathematic representation of a 4D vector of floats

- C++ related class: [`OvMaths::FVector4`](https://github.com/Overload-Technologies/Overload/tree/main/Sources/Overload/OvMaths/include/OvMaths/FVector4.h)

### Variables
|Name|Type|Description|
|-|-|-|
|x|`number`|X component of the Vector4|
|y|`number`|Y component of the Vector4|
|z|`number`|Z component of the Vector4|
|w|`number`|W component of the Vector4|

### Constructors
|Name|Input|Output|Description|
|-|-|-|-|
|new||`Vector4`|Creates an instance of Vector4 with x, y, z and w equals to zero|
|new|`number`:&nbsp;x<br>`number`:&nbsp;y<br>`number`:&nbsp;z<br>`number`:&nbsp;w<br>|`Vector4`|Creates an instance of Vector4 with the given values|

### Functions
|Name|Input|Output|Description|
|-|-|-|-|
|One||`Vector4`|Returns a (1,1,1,1) Vector4|
|Zero||`Vector4`|Returns a (0,0,0,0) Vector4|
|Length|`Vector4`:&nbsp;instance<br>|`number`|Returns the length of the given Vector4|
|Dot|`Vector4`:&nbsp;a<br>`Vector4`:&nbsp;b<br>|`number`|Returns the dot product of the two given Vector4|
|Normalize|`Vector4`:&nbsp;instance<br>|`Vector4`|Returns the normalized version of the given Vector4|
|Lerp|`Vector4`:&nbsp;a<br>`Vector4`:&nbsp;b<br>`number`:&nbsp;t<br>|`Vector4`|Calculate the linear interpolation between two Vector4 using the given alpha|

### Operators
|Operation|Description|
|-|-|
|`Vector4 + Vector4`|Adds two Vector4|
|`Vector4 - Vector4`|Substracts two Vector4|
|`-Vector4`|Multiplies each component of a Vector4 by -1|
|`Vector4 * number`|Multiplies each component of a Vector4 by a number|
|`Vector4 / number`|Divides each component of a Vector4 by a number|
|`to_string(Vector4)`|Converts a Vector4 to a string|
