+++
title = 'Scene'
+++

### Description
Set of actors

- C++ related class: [`OvCore::SceneSystem::Scene`](https://github.com/Overload-Technologies/Overload/tree/main/Sources/Overload/OvCore/include/OvCore/SceneSystem/Scene.h)

### Variables
_This usertype has no variables_

### Constructors
_This usertype has no constructors_

### Functions
|Name|Input|Output|Description|
|-|-|-|-|
|FindActorByName|`Scene`:&nbsp;instance<br>`string`:&nbsp;name<br>|`Actor`|Returns the first actor identified by the given name, or nil on failure|
|FindActorByTag|`Scene`:&nbsp;instance<br>`string`:&nbsp;tag<br>|`Actor`|Returns the first actor identified by the given tag, or nil on failure|
|FindActorsByName|`Scene`:&nbsp;instance<br>`string`:&nbsp;name<br>|`Actor[]`|Returns actors identified by the given name, or nil on failure|
|FindActorsByTag|`Scene`:&nbsp;instance<br>`string`:&nbsp;tag<br>|`Actor[]`|Returns actors identified by the given ID, or nil on failure|
|CreateActor|`Scene`:&nbsp;instance<br>|`Actor`|Creates an actor|
|CreateActor|`Scene`:&nbsp;instance<br>`string`:&nbsp;name<br>`string`:&nbsp;tag<br>|`Actor`|Creates an actor with a name and a tag|

### Operators
_This usertype has no operators_
