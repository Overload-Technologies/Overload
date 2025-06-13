+++
title = 'Debug (table)'
+++

### Description
Global table containing debug-related functions

### Functions
|Name|Input|Output|Description|
|-|-|-|-|
|Log|`string`:&nbsp;message<br>||Log a message to the console with `Default` severity|
|LogInfo|`string`:&nbsp;message<br>||Log a message to the console with `Info` severity|
|LogWarning|`string`:&nbsp;message<br>||Log a message to the console with `Warning` severity|
|LogError|`string`:&nbsp;message<br>||Log a message to the console with `Error` severity|

### Examples
```lua
function MyScript:OnStart()
    Debug.Log("Hello World!")
end

```