### Description
A component responsible for skeletal animation playback and skinning runtime control.

- C++ related class: `OvCore::ECS::Components::CSkinnedMeshRenderer`
- Inherits from lua usertype: [Component](Component.md)

### Variables
_This usertype has no variables_

### Constructors
_This usertype has no constructors_

### Functions
|Name|Input|Output|Description|
|-|-|-|-|
|Play|`SkinnedMeshRenderer`:&nbsp;instance<br>||Starts/resumes animation playback|
|Pause|`SkinnedMeshRenderer`:&nbsp;instance<br>||Pauses animation playback|
|Stop|`SkinnedMeshRenderer`:&nbsp;instance<br>||Stops playback and resets time to start|
|IsPlaying|`SkinnedMeshRenderer`:&nbsp;instance<br>|`boolean`|Returns whether playback is active|
|SetLooping|`SkinnedMeshRenderer`:&nbsp;instance<br>`boolean`:&nbsp;looping<br>||Sets looping mode|
|IsLooping|`SkinnedMeshRenderer`:&nbsp;instance<br>|`boolean`|Returns whether looping is enabled|
|SetPlaybackSpeed|`SkinnedMeshRenderer`:&nbsp;instance<br>`number`:&nbsp;speed<br>||Sets animation playback speed|
|GetPlaybackSpeed|`SkinnedMeshRenderer`:&nbsp;instance<br>|`number`|Returns current playback speed|
|SetTime|`SkinnedMeshRenderer`:&nbsp;instance<br>`number`:&nbsp;timeSeconds<br>||Sets playback time in seconds|
|GetTime|`SkinnedMeshRenderer`:&nbsp;instance<br>|`number`|Returns playback time in seconds|
|GetAnimationCount|`SkinnedMeshRenderer`:&nbsp;instance<br>|`number`|Returns the number of available animation clips|
|GetAnimationName|`SkinnedMeshRenderer`:&nbsp;instance<br>`number`:&nbsp;index<br>|`string`|Returns clip name at index or `nil`|
|SetAnimation|`SkinnedMeshRenderer`:&nbsp;instance<br>`number`:&nbsp;index<br>|`boolean`|Sets active clip by index, returns `true` on success|
|SetAnimation|`SkinnedMeshRenderer`:&nbsp;instance<br>`string`:&nbsp;name<br>|`boolean`|Sets active clip by name, returns `true` on success|
|GetActiveAnimationIndex|`SkinnedMeshRenderer`:&nbsp;instance<br>|`number`|Returns current clip index or `nil`|
|GetActiveAnimationName|`SkinnedMeshRenderer`:&nbsp;instance<br>|`string`|Returns current clip name or `nil`|

### Operators
_This usertype has no operators_