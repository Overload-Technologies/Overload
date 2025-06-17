+++
title = 'Matrix4'
+++

### Description
Mathematic representation of a 4x4 Matrix of floats

- C++ related class: [`OvMaths::FMatrix4`](https://github.com/Overload-Technologies/Overload/tree/main/Sources/Overload/OvMaths/include/OvMaths/FMatrix4.h)

### Variables
_This usertype has no variables_

### Constructors
|Name|Input|Output|Description|
|-|-|-|-|
|new||`Matrix4`|Default constructor|
|new|`number`:&nbsp;m00<br>`number`:&nbsp;m01<br>`number`:&nbsp;m02<br>`number`:&nbsp;m03<br>`number`:&nbsp;m10<br>`number`:&nbsp;m11<br>`number`:&nbsp;m12<br>`number`:&nbsp;m13<br>`number`:&nbsp;m20<br>`number`:&nbsp;m21<br>`number`:&nbsp;m22<br>`number`:&nbsp;m23<br>`number`:&nbsp;m30<br>`number`:&nbsp;m31<br>`number`:&nbsp;m32<br>`number`:&nbsp;m33<br>|`Matrix4`||

### Functions
|Name|Input|Output|Description|
|-|-|-|-|
|Identity||`Matrix4`|Returns an identity Matrix4|
|IsIdentity|`Matrix4`:&nbsp;instance<br>|`boolean`|Returns true if the matrix is identity|
|Determinant|`Matrix4`:&nbsp;instance<br>|`number`|Returns matrix determinant|
|Transpose|`Matrix4`:&nbsp;instance<br>|`Matrix4`|Returns the transpose of the given matrix|
|Minor|`Matrix4`:&nbsp;instance<br>|`number`|Returns matrix minor|
|Inverse|`Matrix4`:&nbsp;instance<br>|`Matrix4`|Rturns the inverse of the given matrix|
|Translation|`Vector3`:&nbsp;translation<br>|`Matrix4`|Returns a translation matrix from the given translation|
|Translate|`Matrix4`:&nbsp;instance<br>`Vector3`:&nbsp;translation<br>|`Matrix4`|Returns a matrix translated using the given translation|
|RotationOnAxisX|`number`:&nbsp;xAngle<br>|`Matrix4`|Returns a rotation matrix constructed using the given X axis angle|
|RotateOnAxisX|`Matrix4`:&nbsp;instance<br>`number`:&nbsp;xAngle<br>|`Matrix4`|Returns a matrix rotated around the given X axis angle|
|RotationOnAxisY|`number`:&nbsp;yAngle<br>|`Matrix4`|Returns a rotation matrix constructed using the given Y axis angle|
|RotateOnAxisY|`Matrix4`:&nbsp;instance<br>`number`:&nbsp;yAngle<br>|`Matrix4`|Returns a matrix rotated around the given Y axis angle|
|RotationOnAxisZ|`number`:&nbsp;zAngle<br>|`Matrix4`|Returns a rotation matrix constructed using the given Z axis angle|
|RotateOnAxisZ|`Matrix4`:&nbsp;instance<br>`number`:&nbsp;zAngle<br>|`Matrix4`|Returns a matrix rotated around the given Z axis angle|
|RotationYXZ|`number`:&nbsp;angle<br>|`Matrix4`|Returns a rotation matrix constructed using the given axis angle (Applied to all angles in YXZ order)|
|RotateYXZ|`Matrix4`:&nbsp;instance<br>`number`:&nbsp;angle<br>|`Matrix4`|Returns a rotation matrix rotated using the given axis angle (Applied to all angles in YXZ order)|
|Rotation|`Quaternion`:&nbsp;rotation<br>|`Matrix4`|Returns a rotation matrix created from the given quaternion|
|Rotate|`Matrix4`:&nbsp;instance<br>`Quaternion`:&nbsp;rotation<br>|`Matrix4`|Returns a matrix rotated using the given quaternion|
|Scaling|`Vector3`:&nbsp;scale<br>|`Matrix4`|Returns a scaling matrix from the given scale|
|Scale|`Matrix4`:&nbsp;instance<br>`Vector3`:&nbsp;scale<br>|`Matrix4`|Returns a scaled matrix using the given scale|
|GetRow|`Matrix4`:&nbsp;instance<br>`number`:&nbsp;row<br>|`Vector4`|Returns the given row|
|GetColumn|`Matrix4`:&nbsp;instance<br>`number`:&nbsp;column<br>|`Vector4`|Returns the given column|
|CreatePerspective|`number`:&nbsp;fov<br>`number`:&nbsp;aspectRatio<br>`number`:&nbsp;near<br>`number`:&nbsp;far<br>|`Matrix4`|Creates a perspective matrix|
|CreateView|`number`:&nbsp;eyeX<br>`number`:&nbsp;eyeY<br>`number`:&nbsp;eyeZ<br>`number`:&nbsp;lookX<br>`number`:&nbsp;lookY<br>`number`:&nbsp;lookZ<br>`number`:&nbsp;upX<br>`number`:&nbsp;upY<br>`number`:&nbsp;upZ<br>|`Matrix4`|Creates a view matrix|
|CreateFrustum|`number`:&nbsp;left<br>`number`:&nbsp;right<br>`number`:&nbsp;bottom<br>`number`:&nbsp;top<br>`number`:&nbsp;near<br>`number`:&nbsp;far<br>|`Matrix4`|Create a frustum matrix|
|Get|`Matrix4`:&nbsp;instance<br>`number`:&nbsp;row<br>`number`:&nbsp;column<br>|`number`|Returns element at index (row,column)|
|Set|`Matrix4`:&nbsp;instance<br>`number`:&nbsp;row<br>`number`:&nbsp;column<br>`number`:&nbsp;value<br>||Sets element at index (row,column)|

### Operators
|Operation|Description|
|-|-|
|`Matrix4 + Matrix4`|Adds two Matrix4|
|`Matrix4 - number`|Substracts a Matrix4 and a number|
|`Matrix4 - Matrix4`|Substracts two Matrix4|
|`Matrix4 * number`|Multiplies each component of a Matrix4 by a number|
|`Matrix4 * Vector4`|Multiplies a Matrix4 by a Vector4|
|`Matrix4 * Matrix4`|Multiplies two Matrix4|
|`Matrix4 / number`|Divides each component of a Matrix4 by a number|
|`Matrix4 / Matrix4`|Divides two Matrix4|
|`to_string(Matrix4)`|Converts a Matrix4 to a string|
