---@meta

---@enum CollisionDetectionMode
CollisionDetectionMode = {
    DISCRETE = 0,
    CONTINUOUS = 1,
}

---@class PhysicalObject : Component
PhysicalObject = {}

---@return Actor
function PhysicalObject:GetOwner() end

---@return number
function PhysicalObject:GetMass() end

---@param mass number
function PhysicalObject:SetMass(mass) end

---@return number
function PhysicalObject:GetFriction() end

---@param friction number
function PhysicalObject:SetFriction(friction) end

---@return number
function PhysicalObject:GetBounciness() end

---@param bounciness number
function PhysicalObject:SetBounciness(bounciness) end

---@param velocity Vector3
function PhysicalObject:SetLinearVelocity(velocity) end

---@param velocity Vector3
function PhysicalObject:SetAngularVelocity(velocity) end

---@return Vector3
function PhysicalObject:GetLinearVelocity() end

---@return Vector3
function PhysicalObject:GetAngularVelocity() end

---@param factor Vector3
function PhysicalObject:SetLinearFactor(factor) end

---@param factor Vector3
function PhysicalObject:SetAngularFactor(factor) end

---@return Vector3
function PhysicalObject:GetLinearFactor() end

---@return Vector3
function PhysicalObject:GetAngularFactor() end

---@return boolean
function PhysicalObject:IsTrigger() end

---@param trigger boolean
function PhysicalObject:SetTrigger(trigger) end

---@param force Vector3
function PhysicalObject:AddForce(force) end

---@param impulse Vector3
function PhysicalObject:AddImpulse(impulse) end

function PhysicalObject:ClearForces() end

---@param mode CollisionDetectionMode
function PhysicalObject:SetCollisionDetectionMode(mode) end

---@return CollisionDetectionMode
function PhysicalObject:GetCollisionMode() end

---@param kinematic boolean
function PhysicalObject:SetKinematic(kinematic) end
