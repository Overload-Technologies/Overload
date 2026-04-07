---@meta

--- Some global maths functions
---@class Math
Math = {}

--- Generate a random number between two given integers (Closed interval)
---@param min integer
---@param max integer
---@return integer
function Math.RandomInt(min, max) end

--- Generate a random number between two given floats (Closed interval)
---@param min number
---@param max number
---@return number
function Math.RandomFloat(min, max) end

--- Verify if the percentage is satisfied
---@param percentage number
---@return boolean
function Math.CheckPercentage(percentage) end

--- Calculates the linear interpolation between two values using the given alpha
---@param a number
---@param b number
---@param t number
---@return number
function Math.Lerp(a, b, t) end
