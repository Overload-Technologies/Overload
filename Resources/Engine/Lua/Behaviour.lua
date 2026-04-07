---@meta

---@class Behaviour
---@field owner Actor
Behaviour = {}

function Behaviour:OnAwake() end

function Behaviour:OnStart() end

function Behaviour:OnEnable() end

function Behaviour:OnDisable() end

function Behaviour:OnDestroy() end

---@param deltaTime number
function Behaviour:OnUpdate(deltaTime) end

---@param fixedDeltaTime number
function Behaviour:OnFixedUpdate(fixedDeltaTime) end

---@param deltaTime number
function Behaviour:OnLateUpdate(deltaTime) end

---@param collideWith PhysicalObject
function Behaviour:OnCollisionEnter(collideWith) end

---@param collideWith PhysicalObject
function Behaviour:OnCollisionStay(collideWith) end

---@param collideWith PhysicalObject
function Behaviour:OnCollisionExit(collideWith) end

---@param triggeredBy PhysicalObject
function Behaviour:OnTriggerEnter(triggeredBy) end

---@param triggeredBy PhysicalObject
function Behaviour:OnTriggerStay(triggeredBy) end

---@param triggeredBy PhysicalObject
function Behaviour:OnTriggerExit(triggeredBy) end
