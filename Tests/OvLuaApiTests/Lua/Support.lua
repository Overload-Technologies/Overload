local kDefaultEpsilon = 0.0001

local function Fail(message)
	error(message, 2)
end

local function GetTypeDescription(value)
	local valueType = type(value)

	if valueType ~= "userdata" and valueType ~= "table" then
		return valueType
	end

	local metatable = getmetatable(value)
	if type(metatable) ~= "table" then
		return valueType
	end

	local callType = metatable and type(metatable.__call) or nil

	if callType then
		return valueType .. " with __call " .. callType
	end

	return valueType
end

local function IsCallable(value)
	local valueType = type(value)

	if valueType == "function" then
		return true
	end

	if valueType ~= "userdata" and valueType ~= "table" then
		return false
	end

	local metatable = getmetatable(value)
	if type(metatable) ~= "table" then
		return false
	end

	local callType = type(metatable.__call)
	return callType == "function" or callType == "userdata"
end

function AssertTrue(condition, message)
	if not condition then
		Fail(message or "expected condition to be true")
	end
end

function AssertFalse(condition, message)
	if condition then
		Fail(message or "expected condition to be false")
	end
end

function AssertEquals(expected, actual, message)
	if actual ~= expected then
		Fail((message or "values differ") .. ": expected " .. tostring(expected) .. ", got " .. tostring(actual))
	end
end

function AssertCallable(value, message)
	if not IsCallable(value) then
		Fail((message or "value") .. ": expected callable, got " .. GetTypeDescription(value))
	end
end

function AssertNear(expected, actual, epsilon, message)
	local tolerance = epsilon or kDefaultEpsilon

	if math.abs(expected - actual) > tolerance then
		Fail((message or "values differ") .. ": expected " .. tostring(expected) .. ", got " .. tostring(actual))
	end
end

function AssertVector2Equals(expectedX, expectedY, actual, epsilon, message)
	local label = message or "Vector2"

	AssertEquals("userdata", type(actual), label .. " type")
	AssertNear(expectedX, actual.x, epsilon, label .. ".x")
	AssertNear(expectedY, actual.y, epsilon, label .. ".y")
end

function AssertVector3Equals(expectedX, expectedY, expectedZ, actual, epsilon, message)
	local label = message or "Vector3"

	AssertEquals("userdata", type(actual), label .. " type")
	AssertNear(expectedX, actual.x, epsilon, label .. ".x")
	AssertNear(expectedY, actual.y, epsilon, label .. ".y")
	AssertNear(expectedZ, actual.z, epsilon, label .. ".z")
end
