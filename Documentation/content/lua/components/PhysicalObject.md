+++
title = 'PhysicalObject'
+++

### Description
Base class for any entity that is physical

- C++ related class: [`OvCore::ECS::Components::CPhysicalObject`](https://github.com/Overload-Technologies/Overload/tree/main/Sources/Overload/OvCore/include/OvCore/ECS/Components/CPhysicalObject.h)
- Inherits from lua usertype: [`Component`](../Component)
- Inherited by lua usertypes:
    - [`PhysicalBox`](PhysicalBox)
    - [`PhysicalSphere`](PhysicalSphere)
    - [`PhysicalCapsule`](PhysicalCapsule)

### Variables
_This usertype has no variables_

### Constructors
|Name|Input|Output|Description|
|-|-|-|-|
|new||`PhysicalObject`||

### Functions
|Name|Input|Output|Description|
|-|-|-|-|
|GetMass|`PhysicalObject`:&nbsp;instance<br>||Returns the mass of the physical object|
|SetMass|`PhysicalObject`:&nbsp;instance<br>`number`:&nbsp;mass<br>||Defines a new mass for the physical object|
|GetFriction|`PhysicalObject`:&nbsp;instance<br>|`number`|Returns the friction of the physical object|
|SetFriction|`PhysicalObject`:&nbsp;instance<br>`number`:&nbsp;friction<br>||Defines a new friction for the physical object|
|GetBounciness|`PhysicalObject`:&nbsp;instance<br>|`number`|Returns the bounciness of the physical object|
|SetBounciness|`PhysicalObject`:&nbsp;instance<br>`number`:&nbsp;bounciness<br>||Defines a new bounciness for the physical object|
|SetLinearVelocity|`PhysicalObject`:&nbsp;instance<br>`Vector3`:&nbsp;linearVelocity<br>||Defines a new linear velocity for the physical object|
|SetAngularVelocity|`PhysicalObject`:&nbsp;instance<br>`Vector3`:&nbsp;angularVelocity<br>||Defines a new angular velocity for the physical object|
|GetLinearVelocity|`PhysicalObject`:&nbsp;instance<br>|`Vector3`|Returns the linear velocity of the physical object|
|GetAngularVelocity|`PhysicalObject`:&nbsp;instance<br>|`Vector3`|Returns the angular of the physical object|
|SetLinearFactor|`PhysicalObject`:&nbsp;instance<br>`Vector3`:&nbsp;linearFactor<br>||Defines a new linear factor for the physical object|
|SetAngularFactor|`PhysicalObject`:&nbsp;instance<br>`Vector3`:&nbsp;angularFactor<br>||Defines a new angular factor for the physical object|
|GetLinearFactor|`PhysicalObject`:&nbsp;instance<br>|`Vector3`|Returns the linear factor of the physical object|
|GetAngularFactor|`PhysicalObject`:&nbsp;instance<br>|`Vector3`|Returns the angular of the physical object|
|IsTrigger|`PhysicalObject`:&nbsp;instance<br>|`boolean`|Returns true if the physical object is a trigger|
|SetTrigger|`PhysicalObject`:&nbsp;instance<br>`boolean`:&nbsp;trigger<br>||Defines if the physical object should be a trigger or not|
|AddForce|`PhysicalObject`:&nbsp;instance<br>`Vector3`:&nbsp;force<br>||Add a force to the physical object|
|AddImpulse|`PhysicalObject`:&nbsp;instance<br>`Vector3`:&nbsp;impulse<br>||Add an impulse to the physical object|
|ClearForces|`PhysicalObject`:&nbsp;instance<br>||Clear forces|
|SetCollisionDetectionMode|`PhysicalObject`:&nbsp;instance<br>`CollisionDetectionMode`:&nbsp;mode<br>||Defines a new collision detection mode for the physical object|
|GetCollisionMode|`PhysicalObject`:&nbsp;instance<br>|`CollisionDetectionMode`|Returns the collision detection mode of the physical object|
|IsKinematic|`PhysicalObject`:&nbsp;instance<br>|`boolean`|Returns true if the physical object is kinematic|
|SetKinematic|`PhysicalObject`:&nbsp;instance<br>`boolean`:&nbsp;kinematic<br>||Defines if the physical object should be kinematic or not|

### Operators
_This usertype has no operators_
