---@meta

--- Some global inputs functions
---@class Inputs
Inputs = {}

--- Returns true if the key has been pressed during the current frame
---@param key Key
---@return boolean
function Inputs.GetKeyDown(key) end

--- Returns true if the key has been released during the current frame
---@param key Key
---@return boolean
function Inputs.GetKeyUp(key) end

--- Returns true if the key is currently down
---@param key Key
---@return boolean
function Inputs.GetKey(key) end

--- Returns true if the mouse button has been pressed during the current frame
---@param button MouseButton
---@return boolean
function Inputs.GetMouseButtonDown(button) end

--- Returns true if the mouse button has been released during the current frame
---@param button MouseButton
---@return boolean
function Inputs.GetMouseButtonUp(button) end

--- Returns true if the mouse button is currently down
---@param button MouseButton
---@return boolean
function Inputs.GetMouseButton(button) end

--- Returns the current position of the mouse cursor
---@return Vector2
function Inputs.GetMousePos() end

--- Returns the current scroll value
---@return Vector2
function Inputs.GetMouseScroll() end

--- Locks and hides the mouse cursor
function Inputs.LockMouse() end

--- Unlocks and shows the mouse cursor
function Inputs.UnlockMouse() end
