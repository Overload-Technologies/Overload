---@meta

--- Set of actors
---@class Scene
Scene = {}

--- Returns the first actor identified by the given name, or nil on failure
---@param name string
---@return Actor|nil
function Scene:FindActorByName(name) end

--- Returns the first actor identified by the given tag, or nil on failure
---@param tag string
---@return Actor|nil
function Scene:FindActorByTag(tag) end

--- Returns actors identified by the given name, or nil on failure
---@param name string
---@return Actor[]
function Scene:FindActorsByName(name) end

--- Returns actors identified by the given tag, or nil on failure
---@param tag string
---@return Actor[]
function Scene:FindActorsByTag(tag) end

--- Creates an actor
---@overload fun(self: Scene): Actor
--- Creates an actor with a name and a tag
---@overload fun(self: Scene, name: string, tag: string): Actor
---@return Actor
function Scene:CreateActor(...) end
