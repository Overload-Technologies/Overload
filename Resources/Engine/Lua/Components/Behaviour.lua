---@meta

--- Base class for user scripts. Inherit from this to create game logic
---@class Behaviour
---@field owner Actor
Behaviour = {}

--- Called once when the behaviour is first initialized, before OnStart
function Behaviour:OnAwake() end

--- Called once before the first update, after OnAwake
function Behaviour:OnStart() end

--- Called when the behaviour becomes enabled and active
function Behaviour:OnEnable() end

--- Called when the behaviour becomes disabled or inactive
function Behaviour:OnDisable() end

--- Called when the behaviour is about to be destroyed
function Behaviour:OnDestroy() end

--- Called every frame while the behaviour is active
---@param deltaTime number
function Behaviour:OnUpdate(deltaTime) end

--- Called at a fixed timestep, used for physics-related updates
---@param fixedDeltaTime number
function Behaviour:OnFixedUpdate(fixedDeltaTime) end

--- Called every frame after OnUpdate
---@param deltaTime number
function Behaviour:OnLateUpdate(deltaTime) end

--- Called when the owner starts colliding with another physical object
---@param collideWith PhysicalObject
function Behaviour:OnCollisionEnter(collideWith) end

--- Called every frame while the owner is colliding with another physical object
---@param collideWith PhysicalObject
function Behaviour:OnCollisionStay(collideWith) end

--- Called when the owner stops colliding with another physical object
---@param collideWith PhysicalObject
function Behaviour:OnCollisionExit(collideWith) end

--- Called when the owner enters a trigger volume
---@param triggeredBy PhysicalObject
function Behaviour:OnTriggerEnter(triggeredBy) end

--- Called every frame while the owner is inside a trigger volume
---@param triggeredBy PhysicalObject
function Behaviour:OnTriggerStay(triggeredBy) end

--- Called when the owner exits a trigger volume
---@param triggeredBy PhysicalObject
function Behaviour:OnTriggerExit(triggeredBy) end
