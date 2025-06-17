+++
title = 'Vector2'
+++

### Description
Mathematic representation of a 2D vector of floats

- C++ related class: [`OvMaths::FVector2`](https://github.com/Overload-Technologies/Overload/tree/main/Sources/Overload/OvMaths/include/OvMaths/FVector2.h)

### Variables
|Name|Type|Description|
|-|-|-|
|x|`number`|X component of the Vector2|
|y|`number`|Y component of the Vector2|

### Constructors
|Name|Input|Output|Description|
|-|-|-|-|
|new||`Vector2`|Creates an instance of Vector2 with x and y equals to zero|
|new|`number`:&nbsp;x<br>`number`:&nbsp;y<br>|`Vector2`|Creates an instance of Vector2 with the given values|

### Functions
|Name|Input|Output|Description|
|-|-|-|-|
|One||`Vector2`|Returns a (1,1) Vector2|
|Zero||`Vector`|Returns a (0,0) Vector2|
|Length|`Vector2`:&nbsp;instance<br>|`number`|Returns the length of the given Vector2|
|Dot|`Vector2`:&nbsp;a<br>`Vector2`:&nbsp;b<br>|`number`|Returns the dot product of the two given Vector2|
|Normalize|`Vector2`:&nbsp;instance<br>|`Vector2`|Returns the normalized version of the given Vector2|
|Lerp|`Vector2`:&nbsp;a<br>`Vector2`:&nbsp;b<br>`number`:&nbsp;t<br>|`Vector2`|Calculates the linear interpolation between two Vector2 using the given alpha|
|AngleBetween|`Vector2`:&nbsp;a<br>`Vector2`:&nbsp;b<br>|`number`|Calculates the angle between two Vector2|

### Operators
|Operation|Description|
|-|-|
|`Vector2 + Vector2`|Adds two Vector2|
|`Vector2 - Vector2`|Substracts two Vector2|
|`-Vector2`|Multiplies each components of a Vector2 by -1|
|`Vector2 * number`|Multiplies each component of a Vector2 by a number|
|`Vector2 / number`|Divides each component of a Vector2 by a number|
|`to_string(Vector2)`|Converts a Vector2 to a string|
