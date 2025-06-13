+++
title = 'Component'
+++

### Description
A component is a set of data and behaviours (Entity-Component without systems) that is interpreted by the engine (Or the user)

- C++ related class: [`OvCore::ECS::Components::AComponent`](https://github.com/Overload-Technologies/Overload/tree/main/Sources/Overload/OvCore/include/OvCore/ECS/Components/AComponent.h)
- Inherited by lua usertypes:
  - [`Transform`](../Transform)
  - [`Camera`](../Camera)
  - [`MaterialRenderer`](../MaterialRenderer)
  - [`ModelRenderer`](../ModelRenderer)
  - [`Light`](../Light)
  - [`PhysicalObject`](../PhysicalObject)
  - [`AudioListener`](../AudioListener)
  - [`AudioSource`](../AudioSource)
  - [`PostProcessStack`](../PostProcessStack)
  - [`ReflectionProbe`](../ReflectionProbe)

### Variables
_This usertype has no variables_

### Constructors
_This usertype has no constructors_

### Functions
|Name|Input|Output|Description|
|-|-|-|-|
|GetOwner|`Component`:&nbsp;instance<br>|`Actor`|Returns the actor that owns this component|

### Operators
_This usertype has no operators_
