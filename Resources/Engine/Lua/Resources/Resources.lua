---@meta

--- Some global resources functions
---@class Resources
Resources = {}

--- Loads (If not already loaded) and returns the Model identified by the given path. Returns nil on failure
---@param path string
---@return userdata|nil
function Resources.GetModel(path) end

--- Loads (If not already loaded) and returns the Shader identified by the given path. Returns nil on failure
---@param path string
---@return userdata|nil
function Resources.GetShader(path) end

--- Loads (If not already loaded) and returns the Texture identified by the given path. Returns nil on failure
---@param path string
---@return userdata|nil
function Resources.GetTexture(path) end

--- Loads (If not already loaded) and returns the Material identified by the given path. Returns nil on failure
---@param path string
---@return userdata|nil
function Resources.GetMaterial(path) end

--- Loads (If not already loaded) and returns the Sound identified by the given path. Returns nil on failure
---@param path string
---@return userdata|nil
function Resources.GetSound(path) end
