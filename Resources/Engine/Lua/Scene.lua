---@meta

---@class Scene
Scene = {}

---@param name string
---@return Actor|nil
function Scene:FindActorByName(name) end

---@param tag string
---@return Actor|nil
function Scene:FindActorByTag(tag) end

---@param name string
---@return Actor[]
function Scene:FindActorsByName(name) end

---@param tag string
---@return Actor[]
function Scene:FindActorsByTag(tag) end

---@overload fun(self: Scene): Actor
---@overload fun(self: Scene, name: string, tag: string): Actor
---@return Actor
function Scene:CreateActor(...) end
