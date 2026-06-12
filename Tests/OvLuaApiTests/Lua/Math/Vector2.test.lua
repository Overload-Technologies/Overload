local epsilon = 0.0001

local defaultVector = Vector2.new()
AssertVector2Equals(0, 0, defaultVector, epsilon, "default constructor")

local a = Vector2.new(3, 4)
local b = Vector2.new(1, 2)

AssertVector2Equals(3, 4, a, epsilon, "constructor")
AssertVector2Equals(1, 1, Vector2.One(), epsilon, "Vector2.One")
AssertVector2Equals(0, 0, Vector2.Zero(), epsilon, "Vector2.Zero")

AssertVector2Equals(4, 6, a + b, epsilon, "addition")
AssertVector2Equals(2, 2, a - b, epsilon, "subtraction")
AssertVector2Equals(-3, -4, -a, epsilon, "unary minus")
AssertVector2Equals(6, 8, a * 2, epsilon, "scalar multiplication")
AssertVector2Equals(1.5, 2, a / 2, epsilon, "scalar division")

AssertNear(5, a:Length(), epsilon, "length")
AssertNear(11, a:Dot(b), epsilon, "dot")
AssertVector2Equals(0.6, 0.8, a:Normalize(), epsilon, "normalize")
AssertVector2Equals(0, 0, Vector2.Zero():Normalize(), epsilon, "zero normalize")
AssertVector2Equals(2, 3, Vector2.Lerp(b, a, 0.5), epsilon, "lerp")

local ok = pcall(function()
	return a / 0
end)

AssertFalse(ok, "division by zero fails")
