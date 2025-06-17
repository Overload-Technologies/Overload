+++
title = 'Resources (table)'
+++

### Description
Global table containing resource-related functions

### Functions
|Name|Input|Output|Description|
|-|-|-|-|
|GetModel|`string`:&nbsp;path<br>|`Usertype`|Loads (If not already loaded) and returns the Model identified by the given path. Returns nil on failure|
|GetShader|`string`:&nbsp;path<br>|`Usertype`|Loads (If not already loaded) and returns the Shader identified by the given path. Returns nil on failure|
|GetTexture|`string`:&nbsp;path<br>|`Usertype`|Loads (If not already loaded) and returns the Texture identified by the given path. Returns nil on failure|
|GetMaterial|`string`:&nbsp;path<br>|`Usertype`|Loads (If not already loaded) and returns the Material identified by the given path. Returns nil on failure|
|GetSound|`string`:&nbsp;path<br>|`Usertype`|Loads (If not already loaded) and returns the Sound identified by the given path. Returns nil on failure|

### Examples
```lua
function MyScript::OnStart()
    -- Find a material in the engine folder (using ':')
    local defaultMaterial = Resources.GetMaterial(":Materials\\Default.ovmat")

    -- Find a shader in the project asset folder
    local myCustomShader = Resources.GetShader("Shaders\\MyCustomShader.ovfx")
end
```