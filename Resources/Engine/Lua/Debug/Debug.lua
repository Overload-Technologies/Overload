---@meta

--- Some global debug functions
---@class Debug
Debug = {}

--- Log a message to the console with Default severity
---@param message string
function Debug.Log(message) end

--- Log a message to the console with Info severity
---@param message string
function Debug.LogInfo(message) end

--- Log a message to the console with Warning severity
---@param message string
function Debug.LogWarning(message) end

--- Log a message to the console with Error severity
---@param message string
function Debug.LogError(message) end
