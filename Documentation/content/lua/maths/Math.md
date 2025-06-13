+++
title = 'Math (table)'
+++

### Description
Global table containing math-related functions

### Functions
|Name|Input|Output|Description|
|-|-|-|-|
|RandomInt|`number`:&nbsp;min<br>`number`:&nbsp;max<br>|`number`|Generate a random number between two given integers (Closed interval)|
|RandomFloat|`number`:&nbsp;min<br>`number`:&nbsp;max<br>|`number`|Generate a random number between two given floats (Closed interval)|
|CheckPercentage|`number`:&nbsp;percentage<br>|`boolean`|Verify if the percentage is satisfied|

### Examples
```lua
function MyScript::OnStart()
    local randomInt = Math.RandomInt(0, 100)
    local randomFloat = Math.RandomFloat(0.0, 1.0) 
end
```
