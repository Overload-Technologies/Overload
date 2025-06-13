+++
title = 'Quaternion'
+++

### Description
Mathematic representation of a Quaternion with float precision

- C++ related class: [`OvMaths::FQuaternion`](https://github.com/Overload-Technologies/Overload/tree/main/Sources/Overload/OvMaths/include/OvMaths/FQuaternion.h)

### Variables
|Name|Type|Description|
|-|-|-|
|x|`number`|X component of the Quaternion|
|y|`number`|Y component of the Quaternion|
|z|`number`|Z component of the Quaternion|
|w|`number`|W component of the Quaternion|

### Constructors
|Name|Input|Output|Description|
|-|-|-|-|
|new||`Quaternion`|Default Quaternion constructor (Creates an identity quaternion with 1 as w)|
|new|`number`:&nbsp;real<br>|`Quaternion`|Creates an identity quaternion with a defined real value|
|new|`number`:&nbsp;x<br>`number`:&nbsp;y<br>`number`:&nbsp;z<br>`number`:&nbsp;w<br>|`Quaternion`|Creates a quaternion from a set of 4 floats (x, y, z, w)|

### Functions
|Name|Input|Output|Description|
|-|-|-|-|
|IsIdentity|`Quaternion`:&nbsp;instance<br>|`boolean`|Returns true if the quaternion is Identity (x, y and z components are 0)|
|IsPure|`Quaternion`:&nbsp;instance<br>|`boolean`|Returns true if the quaternion is pure (w is 0)|
|IsNormalized|`Quaternion`:&nbsp;instance<br>|`boolean`|Returns true if the quaternion is nornalized|
|Dot|`Quaternion`:&nbsp;a<br>`Quaternion`:&nbsp;b<br>|`number`|Calculates the dot product between two quaternions|
|Normalize|`Quaternion`:&nbsp;instance<br>|`Quaternion`|Returns the normalized version of the given quaternion|
|Length|`Quaternion`:&nbsp;instance<br>|`number`|Returns the length of the given quaternion|
|LengthSquare|`Quaternion`:&nbsp;instance<br>|`number`|Returns the squared length of the given quaternion. This function is faster than Quaternion.Length|
|GetAngle|`Quaternion`:&nbsp;instance<br>|`number`|Returns the angle of the given quaternion|
|GetRotationAxis|`Quaternion`:&nbsp;instance<br>|`Vector3`|Returns the rotation axis of the given quaternion|
|Inverse|`Quaternion`:&nbsp;instance<br>|`Quaternion`|Returns the inverse of the quaternion|
|Conjugate|`Quaternion`:&nbsp;instance<br>|`Quaternion`|Returns the conjugate of the quaternion|
|Square|`Quaternion`:&nbsp;instance<br>|`Quaternion`|Returns the squared version of this quaternion|
|GetAxisAndAngle|`Quaternion`:&nbsp;instance<br>|`Vector3`:&nbsp;axis<br>`number`:&nbsp;angle<br>|Returns the axis and the angle of the given quaternion|
|AngularDistance|`Quaternion`:&nbsp;a<br>`Quaternion`:&nbsp;b<br>|`number`|Returns the angle between two quaternions|
|Lerp|`Quaternion`:&nbsp;a<br>`Quaternion`:&nbsp;b<br>`number`:&nbsp;t<br>|`Quaternion`|Returns the linear interpolation of two quaternions based on the given interpolation coefficient|
|Slerp|`Quaternion`:&nbsp;a<br>`Quaternion`:&nbsp;b<br>`number`:&nbsp;t<br>|`Quaternion`|Returns the spherical linear interpolation of two quaternions based on the given interpolation coefficient|
|Nlerp|`Quaternion`:&nbsp;a<br>`Quaternion`:&nbsp;b<br>`number`:&nbsp;t<br>|`Quaternion`|Returns the nornmalized linear interpolation of two quaternions based on the given interpolation coefficient|
|RotatePoint|`Vector3`:&nbsp;point<br>`Quaternion`:&nbsp;quaternion<br>`Vector3`:&nbsp;pivot<br>|`Vector3`|Rotates a point around a pivot using a quaternion|
|EulerAngles|`Quaternion`:&nbsp;instance<br>|`Vector`|Returns euler axis angles (In degrees)|
|ToMatrix3|`Quaternion`:&nbsp;instance<br>|`Matrix3`|Returns a rotation matrix (3x3) out of the given quaternion|
|ToMatrix4|`Quaternion`:&nbsp;instance<br>|`Matrix4`|Returns a rotation matrix (4x4) out of the given quaternion|

### Operators
|Operation|Description|
|-|-|
|`Quaternion + Quaternion`|Adds two Quaternion|
|`Quaternion - Quaternion`|Substracts two Quaternion|
|`Quaternion * number`|Multiplies each components of a Quaternion by a number|
|`Quaternion * Vector3`|Multiplies a Quaternion by a Vector3|
|`Quaternion * Matrix3`|Multiplies a Quaternion by a Matrix3|
|`Quaternion / number`|Divides each component of a Quaternion by a number|
|`to_string(Quaternion)`|Converts a Quaternion to a string|

