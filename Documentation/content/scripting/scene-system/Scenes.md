+++
title = 'Scenes (table)'
+++

### Description
Global table containing scene-related functions

### Functions
|Name|Input|Output|Description|
|-|-|-|-|
|GetCurrentScene||`Scene`|Returns the currently active scene|
|Load|`string`:&nbsp;path<br>|`Scene`|Loads the scene identified by the given path and returns it|

### Examples
```lua
function MyScript::OnStart()
    local scene = Scenes.GetCurrentScene()
    Scenes.Load("MyScene.ovscene")
end
```