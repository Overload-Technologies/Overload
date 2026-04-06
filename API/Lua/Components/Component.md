### Description
A component is a set of data and behaviours (Entity-Component without systems) that is interpreted by the engine (Or the user)

- C++ related class: [`OvCore::ECS::Components::AComponent`](/Sources/OvCore/include/OvCore/ECS/Components/AComponent.h)
- Inherited by lua usertypes:
  - [`Transform`](Transform.md)
  - [`Camera`](Camera.md)
  - [`MaterialRenderer`](MaterialRenderer.md)
  - [`ModelRenderer`](ModelRenderer.md)
  - [`Light`](Light.md)
  - [`PhysicalObject`](PhysicalObject.md)
  - [`AudioListener`](AudioListener.md)
  - [`AudioSource`](AudioSource.md)
  - [`PostProcessStack`](PostProcessStack.md)
  - [`ReflectionProbe`](ReflectionProbe.md)
  - [`SkinnedMeshRenderer`](SkinnedMeshRenderer.md)

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
