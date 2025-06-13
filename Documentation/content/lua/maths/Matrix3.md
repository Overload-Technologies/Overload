+++
title = 'Matrix3'
+++

### Description
Mathematic representation of a 3x3 Matrix of floats

- C++ related class: [`OvMaths::FMatrix3`](https://github.com/Overload-Technologies/Overload/tree/main/Sources/Overload/OvMaths/include/OvMaths/FMatrix3.h)

### Variables
_This usertype has no variables_

### Constructors
|Name|Input|Output|Description|
|-|-|-|-|
|new||`Matrix3`|Default constructor|
|new|`number`:&nbsp;value<br>|`Matrix3`|Constructor setting all components to the given value|
|new|`number`:&nbsp;m00<br>`number`:&nbsp;m01<br>`number`:&nbsp;m02<br>`number`:&nbsp;m10<br>`number`:&nbsp;m11<br>`number`:&nbsp;m12<br>`number`:&nbsp;m20<br>`number`:&nbsp;m21<br>`number`:&nbsp;m22<br>|`Matrix3`|Instantiate a Matrix3 with all the given component values|

### Functions
|Name|Input|Output|Description|
|-|-|-|-|
|Identity||`Matrix3`|Returns an identity Matrix3|
|IsIdentity|`Matrix3`:&nbsp;instance<br>|`boolean`|Returns true if the matrix is identity|
|Determinant|`Matrix3`:&nbsp;instance<br>|`number`|Returns matrix determinant|
|Transpose|`Matrix3`:&nbsp;instance<br>|`Matrix3`|Returns the transpose of the given matrix|
|Cofactor|`Matrix3`:&nbsp;instance<br>|`Matrix3`|Returns the cofactor of the given matrix|
|Minor|`Matrix3`:&nbsp;instance<br>|`Matrix3`|Returns the minor of the given matrix|
|Adjoint|`Matrix3`:&nbsp;instance<br>|`Matrix3`|Returns the adjoint of the given matrix|
|Inverse|`Matrix3`:&nbsp;instance<br>|`Matrix3`|Returns the inverse of the given matrix|
|Translation|`Vector2`:&nbsp;translation<br>|`Matrix3`|Returns a translation matrix from the given translation|
|Translate|`Matrix3`:&nbsp;instance<br>`Vector2`:&nbsp;translation<br>|`Matrix3`|Returns a matrix translated using the given translation|
|Rotation|`number`:&nbsp;angle<br>|`Matrix3`|Returns a rotation matrix from the given rotation angle (degrees)|
|Rotate|`Matrix3`:&nbsp;instance<br>`number`:&nbsp;angle<br>|`Matrix3`|Returns a matrix rotated using the given rotation angle (degrees)|
|Scaling|`Vector2`:&nbsp;scale<br>|`Matrix3`|Returns a scaling matrix from the given scale|
|Scale|`Matrix3`:&nbsp;instance<br>`Vector2`:&nbsp;scale<br>|`Matrix3`|Returns a scaled matrix using the given scale|
|GetRow|`Matrix3`:&nbsp;instance<br>`number`:&nbsp;index<br>|`Vector3`|Returns the given row|
|GetColumn|`Matrix3`:&nbsp;instance<br>`number`:&nbsp;index<br>|`Vector3`|Returns the given column|
|Get|`Matrix3`:&nbsp;instance<br>`number`:&nbsp;row<br>`number`:&nbsp;column<br>|`number`|Returns element at index (row,column)|
|Set|`Matrix3`:&nbsp;instance<br>`number`:&nbsp;row<br>`number`:&nbsp;column<br>`number`:&nbsp;value<br>||Sets element at index (row,column)|

### Operators
|Operation|Description|
|-|-|
|`Matrix3 + Matrix3`|Adds two Matrix3|
|`Matrix3 - Matrix3`|Substracts two Matrix3|
|`Matrix3 * number`|Multiplies each components of a Matrix3 by a number|
|`Matrix3 * Vector3`|Multiplies a Matrix3 by a Vector3|
|`Matrix3 * Matrix3`|Multiplies two Matrix3|
|`Matrix3 / number`|Divides each components of a Matrix3 by a number|
|`Matrix3 / Matrix3`|Divides two Matrix3|
|`to_string(Matrix3)`|Converts a Matrix3 to a string|
