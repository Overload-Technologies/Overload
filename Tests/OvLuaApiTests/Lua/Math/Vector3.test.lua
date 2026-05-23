local epsilon = 0.0001

local defaultVector = Vector3.new()
AssertVector3Equals(0, 0, 0, defaultVector, epsilon, "default constructor")

local a = Vector3.new(1, 2, 3)
local b = Vector3.new(4, 5, 6)

AssertVector3Equals(1, 2, 3, a, epsilon, "constructor")
AssertVector3Equals(1, 1, 1, Vector3.One(), epsilon, "Vector3.One")
AssertVector3Equals(0, 0, 0, Vector3.Zero(), epsilon, "Vector3.Zero")
AssertVector3Equals(0, 0, 1, Vector3.Forward(), epsilon, "Vector3.Forward")
AssertVector3Equals(0, 0, -1, Vector3.Backward(), epsilon, "Vector3.Backward")
AssertVector3Equals(0, 1, 0, Vector3.Up(), epsilon, "Vector3.Up")
AssertVector3Equals(0, -1, 0, Vector3.Down(), epsilon, "Vector3.Down")
AssertVector3Equals(1, 0, 0, Vector3.Right(), epsilon, "Vector3.Right")
AssertVector3Equals(-1, 0, 0, Vector3.Left(), epsilon, "Vector3.Left")

AssertVector3Equals(5, 7, 9, a + b, epsilon, "addition")
AssertVector3Equals(-3, -3, -3, a - b, epsilon, "subtraction")
AssertVector3Equals(-1, -2, -3, -a, epsilon, "unary minus")
AssertVector3Equals(2, 4, 6, a * 2, epsilon, "scalar multiplication")
AssertVector3Equals(4, 10, 18, a * b, epsilon, "component multiplication")
AssertVector3Equals(0.5, 1, 1.5, a / 2, epsilon, "scalar division")

AssertNear(math.sqrt(14), a:Length(), epsilon, "length")
AssertNear(32, a:Dot(b), epsilon, "dot")
AssertVector3Equals(-3, 6, -3, a:Cross(b), epsilon, "cross")
AssertVector3Equals(0, 0, 0, Vector3.Zero():Normalize(), epsilon, "zero normalize")
AssertVector3Equals(2.5, 3.5, 4.5, Vector3.Lerp(a, b, 0.5), epsilon, "lerp")
AssertNear(math.sqrt(27), Vector3.Distance(a, b), epsilon, "distance")

local ok = pcall(function()
	return a / 0
end)

AssertFalse(ok, "division by zero fails")
