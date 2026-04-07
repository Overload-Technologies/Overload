---@meta

--- Lua script profiling using Tracy
---@class tracyClass
tracy = {}

--- Starts a profiling zone
function tracy.ZoneBegin() end

--- Starts a profiling zone with a custom name
---@param name string
function tracy.ZoneBeginN(name) end

--- Sets a text for the current zone
---@param text string
function tracy.ZoneText(text) end

--- Sends a message
---@param text string
function tracy.Message(text) end

--- Sets a zone name on a per-call basis
---@param text string
function tracy.ZoneName(text) end

--- Ends the current profiling zone
function tracy.ZoneEnd() end
