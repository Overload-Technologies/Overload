+++
title = 'AudioSource'
+++

### Description
Represents an audio source. Its position in the world is important if the spatial sound settings is on

- C++ related class: [`OvCore::ECS::Components::CAudioSource`](https://github.com/Overload-Technologies/Overload/tree/main/Sources/Overload/OvCore/include/OvCore/ECS/Components/CAudioSource.h)
- Inherits from lua usertype: [`Component`](../Component)

### Variables
_This usertype has no variables_

### Constructors
|Name|Input|Output|Description|
|-|-|-|-|
|new||`AudioSource`|Default Constructor|

### Functions
|Name|Input|Output|Description|
|-|-|-|-|
|Play|`AudioSource`:&nbsp;instance<br>||Plays the attached sounds|
|Stop|`AudioSource`:&nbsp;instance<br>||Stops playback|
|Pause|`AudioSource`:&nbsp;instance<br>||Suspend playback|
|Resume|`AudioSource`:&nbsp;instance<br>||Resume the playback from where it get suspended|
|GetSound|`AudioSource`:&nbsp;instance<br>|`Usertype`|Returns the sound attached to the audio source|
|GetVolume|`AudioSource`:&nbsp;instance<br>|`number`|Returns the current volume|
|GetPan|`AudioSource`:&nbsp;instance<br>|`number`|Returns the pan|
|IsLooped|`AudioSource`:&nbsp;instance<br>|`boolean`|Returns true if the AudioSource is setup to loop|
|GetPitch|`AudioSource`:&nbsp;instance<br>|`number`|Returns the pitch|
|IsFinished|`AudioSource`:&nbsp;instance<br>|`boolean`|Returns true if the audio source sound has finished|
|IsSpatial|`AudioSource`:&nbsp;instance<br>|`boolean`|Returns true if the audio source is spatialized|
|GetAttenuationThreshold|`AudioSource`:&nbsp;instance<br>|`number`|Returns the audio source attenuation threshold (Minimum distance before applying sound attenuation)|
|SetSound|`AudioSource`:&nbsp;instance<br>`Usertype`:&nbsp;sound<br>||Defines the sound to play on the audio source|
|SetVolume|`AudioSource`:&nbsp;instance<br>`number`:&nbsp;volume<br>||Defines the audio source volume|
|SetPan|`AudioSource`:&nbsp;instance<br>`number`:&nbsp;pan<br>||Defines the audio source pan (Left / Right)|
|SetLooped|`AudioSource`:&nbsp;instance<br>`boolean`:&nbsp;loop<br>||Defines if the audio source should loop|
|SetPitch|`AudioSource`:&nbsp;instance<br>`number`:&nbsp;pitch<br>||Defines the audio source pitch (= frequency or playback speed)|
|SetSpatial|`AudioSource`:&nbsp;instance<br>`boolean`:&nbsp;spatial<br>||Defines if the audio source should be spatialized or not|
|SetAttenuationThreshold|`AudioSource`:&nbsp;instance<br>`number`:&nbsp;threshold<br>||Defines the audio source attenuation threshold (Minimum distance before applying sound attenuation)|

### Operators
_This usertype has no operators_
