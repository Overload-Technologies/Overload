### Description
A ModelRenderer is necessary in combination with a MaterialRenderer to render a model in the world

- C++ related class: [`OvCore::ECS::Components::CModelRenderer`](/Sources/OvCore/include/OvCore/ECS/Components/CModelRenderer.h)
- Inherits from lua usertype: [`Component`](Component.md)

### Variables
_This usertype has no variables_

### Constructors
|Name|Input|Output|Description|
|-|-|-|-|
|new||ModelRenderer||

### Functions
|Name|Input|Output|Description|
|-|-|-|-|
|GetModel|`ModelRenderer`:&nbsp;instance<br>|`Model`|Returns the current model|
|SetModel|`ModelRenderer`:&nbsp;instance<br>`Model`:&nbsp;model<br>||Defines the model to use|
|GetFrustumBehaviour|`ModelRenderer`:&nbsp;instance<br>|[`FrustumBehaviour`](FrustumBehaviour)|Returns the current bounding mode|
|SetFrustumBehaviour|`ModelRenderer`:&nbsp;instance<br>[`FrustumBehaviour`](FrustumBehaviour):&nbsp;behaviour<br>||Sets a bounding mode|

### Operators
_This usertype has no operators_
