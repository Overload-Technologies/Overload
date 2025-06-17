+++
title = 'Actor'
+++

### Description
The Actor is the main class of the ECS, it corresponds to the entity and is
composed of componenents and behaviours (scripts)

- C++ related class: [`OvCore::ECS::Actor`](https://github.com/Overload-Technologies/Overload/tree/main/Sources/Overload/OvCore/include/OvCore/ECS/Actor.h)

### Variables
_This usertype has no variables_

### Constructors
_This usertype has no constructors_

### Functions
|Name|Input|Output|Description|
|-|-|-|-|
|GetName|`Actor`:&nbsp;instance<br>|`string`|Returns the name of the actor|
|SetName|`Actor`:&nbsp;instance<br>`string`:&nbsp;name||Defines a name for the actor|
|GetTag|`Actor`:&nbsp;instance<br>|`string`|Returns the tag of the actor|
|GetChildren|`Actor`:&nbsp;instance<br>|`Actor[]`|Returns children of this actor|
|SetTag|`Actor`:&nbsp;instance<br>|`string`|Defines a tag for the actor|
|GetID|`Actor`:&nbsp;instance<br>|`number`|Returns the ID of this actor|
|GetParent|`Actor`:&nbsp;instance<br>|`Actor`|Returns the parent (or nil) of this actor|
|SetParent|`Actor`:&nbsp;instance<br>`Actor`:&nbsp;newParent<br>||Defines a new parent to this actor|
|DetachFromParent|`Actor`:&nbsp;instance<br>||Detaches the actor from its parent|
|IsDescendantOf|`Actor`:&nbsp;instance<br>`Actor`:&nbsp;otherActor<br>|`boolean`|Returns true if the otherActor is the topmost parent of the instance in the hierarchy
|Destroy|`Actor`:&nbsp;instance<br>||Removes the actor from the scene|
|IsSelfActive|`Actor`:&nbsp;instance<br>|`boolean`|Returns true if the actor is active, ignoring his parent (if any) active state|
|IsActive|`Actor`:&nbsp;instance<br>|`boolean`|Returns true if the actor is active|
|SetActive|`Actor`:&nbsp;instance<br>`boolean`:&nbsp;active<br>||Defines if the actor is active|
|GetTransform|`Actor`:&nbsp;instance<br>|`Transform`|Returns the Transform attached to this actor|
|GetPhysicalObject|`Actor`:&nbsp;instance<br>|`PhysicalObject`|Returns the PhysicalObject attached to this actor (If any)|
|GetPhysicalBox|`Actor`:&nbsp;instance<br>|`PhysicalBox`|Returns the PhysicalBox attached to this actor (If any)|
|GetPhysicalSphere|`Actor`:&nbsp;instance<br>|`PhysicalSphere`|Returns the PhysicalSphere attached to this actor (If any)|
|GetPhysicalCapsule|`Actor`:&nbsp;instance<br>|`PhysicalCapsule`|Returns the PhysicalCapsule attached to this actor (If any)|
|GetCamera|`Actor`:&nbsp;instance<br>|`Camera`|Returns the Camera attached to this actor (If any)|
|GetLight|`Actor`:&nbsp;instance<br>|`Light`|Returns the Light attached to this actor (If any)|
|GetPointLight|`Actor`:&nbsp;instance<br>|`PointLight`|Returns the PointLight attached to this actor (If any)|
|GetSpotLight|`Actor`:&nbsp;instance<br>|`SpotLight`|Returns the SpotLight attached to this actor (If any)|
|GetDirectionalLight|`Actor`:&nbsp;instance<br>|`DirectionalLight`|Returns the DirectionalLight attached to this actor (If any)|
|GetAmbientBoxLight|`Actor`:&nbsp;instance<br>|`AmbientBoxLight`|Returns the AmbientBoxLight attached to this actor (If any)|
|GetAmbientSphereLight|`Actor`:&nbsp;instance<br>|`AmbientSphereLight`|Returns the AmbientSphereLight attached to this actor (If any)|
|GetModelRenderer|`Actor`:&nbsp;instance<br>|`ModelRenderer`|Returns the ModelRenderer attached to this actor (If any)|
|GetMaterialRenderer|`Actor`:&nbsp;instance<br>|`MaterialRenderer`|Returns the MaterialRenderer attached to this actor (If any)|
|GetAudioSource|`Actor`:&nbsp;instance<br>|`AudioSource`|Returns the AudioSource attached to this actor (If any)|
|GetAudioListener|`Actor`:&nbsp;instance<br>|`AudioListener`|Returns the AudioListener attached to this actor (If any)|
|GetPostProcessStack|`Actor`:&nbsp;instance<br>|`PostProcessStack`|Returns the PostProcessStack attached to this actor (If any)|
|GetReflectionProbe|`Actor`:&nbsp;instance<br>|`ReflectionProbe`|Returns the ReflectionProbe attached to this actor (If any)|
|GetBehaviour|`Actor`:&nbsp;instance<br>`string`:&nbsp;behaviour<br>|`Usertype`|Returns the Behaviour of the given type attached to this actor (If any)|
|AddTransform|`Actor`:&nbsp;instance<br>|`Transform`|Adds a Transform component to the actor and returns it|
|AddModelRenderer|`Actor`:&nbsp;instance<br>|`ModelRenderer`|Adds a ModelRenderer component to the actor and returns it|
|AddPhysicalBox|`Actor`:&nbsp;instance<br>|`PhysicalBox`|Adds a PhysicalBox component to the actor and returns it|
|AddPhysicalSphere|`Actor`:&nbsp;instance<br>|`PhysicalSphere`|Adds a PhysicalSphere component to the actor and returns it|
|AddPhysicalCapsule|`Actor`:&nbsp;instance<br>|`PhysicalCapsule`|Adds a PhysicalCapsule component to the actor and returns it|
|AddCamera|`Actor`:&nbsp;instance<br>|`Camera`|Adds a Camera component to the actor and returns it|
|AddPointLight|`Actor`:&nbsp;instance<br>|`PointLight`|Adds a PointLight component to the actor and returns it|
|AddSpotLight|`Actor`:&nbsp;instance<br>|`SpotLight`|Adds a SpotLight component to the actor and returns it|
|AddDirectionalLight|`Actor`:&nbsp;instance<br>|`DirectionalLight`|Adds an DirectionalLight component to the actor and returns it|
|AddAmbientBoxLight|`Actor`:&nbsp;instance<br>|`AmbientBoxLight`|Adds an AmbientBoxLight component to the actor and returns it|
|AddAmbientSphereLight|`Actor`:&nbsp;instance<br>|`AmbientSphereLight`|Adds an AmbientSphereLight component to the actor and returns it|
|AddMaterialRenderer|`Actor`:&nbsp;instance<br>|`MaterialRenderer`|Adds a MaterialRenderer component to the actor and returns it|
|AddAudioSource|`Actor`:&nbsp;instance<br>|`AudioSource`|Adds an AudioSource component to the actor and returns it|
|AddAudioListener|`Actor`:&nbsp;instance<br>|`AudioListener`|Adds an AudioListener component to the actor and returns it|
|AddPostProcessStack|`Actor`:&nbsp;instance<br>|`PostProcessStack`|Adds a PostProcessStack component to the actor and returns it|
|AddReflectionProbe|`Actor`:&nbsp;instance<br>|`ReflectionProbe`|Adds an ReflectionProbe component to the actor and returns it|
|AddBehaviour|`Actor`:&nbsp;instance<br>|`Usertype`|Adds a behaviour of given type to the actor and returns it|
|RemoveModelRenderer|`Actor`:&nbsp;instance<br>|`boolean`|Removes the ModelRenderer component from the actor (Returns true of success)|
|RemovePhysicalBox|`Actor`:&nbsp;instance<br>|`boolean`|Removes the PhysicalBox component from the actor (Returns true of success)|
|RemovePhysicalSphere|`Actor`:&nbsp;instance<br>|`boolean`|Removes the PhysicalSphere component from the actor (Returns true of success)|
|RemovePhysicalCapsule|`Actor`:&nbsp;instance<br>|`boolean`|Removes the PhysicalCapsule component from the actor (Returns true of success)|
|RemoveCamera|`Actor`:&nbsp;instance<br>|`boolean`|Removes the Camera component from the actor (Returns true of success)|
|RemovePointLight|`Actor`:&nbsp;instance<br>|`boolean`|Removes the PointLight component from the actor (Returns true of success)|
|RemoveSpotLight|`Actor`:&nbsp;instance<br>|`boolean`|Removes the SpotLight component from the actor (Returns true of success)|
|RemoveDirectionalLight|`Actor`:&nbsp;instance<br>|`boolean`|Removes the DirectionalLight component from the actor (Returns true of success)|
|RemoveAmbientBoxLight|`Actor`:&nbsp;instance<br>|`boolean`|Removes the AmbientBoxLight component from the actor (Returns true of success)|
|RemoveAmbientSphereLight|`Actor`:&nbsp;instance<br>|`boolean`|Removes the AmbientSphereLight component from the actor (Returns true of success)|
|RemoveMaterialRenderer|`Actor`:&nbsp;instance<br>|`boolean`|Removes the MaterialRenderer component from the actor (Returns true of success)|
|RemoveAudioSource|`Actor`:&nbsp;instance<br>|`boolean`|Removes the AudioSource component from the actor (Returns true of success)|
|RemoveAudioListener|`Actor`:&nbsp;instance<br>|`boolean`|Removes the AudioListener component from the actor (Returns true of success)|
|RemovePostProcessStack|`Actor`:&nbsp;instance<br>|`PostProcessStack`|Removes the PostProcessStack component from the actor (Returns true of success)|
|RemoveReflectionProbe|`Actor`:&nbsp;instance<br>|`ReflectionProbe`|Removes the ReflectionProbe component from the actor (Returns true of success)|
|RemoveBehaviour|`Actor`:&nbsp;instance<br>`string`:&nbsp;behaviour<br>|`boolean`|Removes the behaviour of given type (Returns true on success)|

### Operators
_This usertype has no operators_
