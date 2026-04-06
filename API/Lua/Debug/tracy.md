### Description
Lua script profiling using Tracy.

### Variables
_This usertype has no variables_

### Constructors
_This usertype has no constructors_

### Functions
|Name|Input|Output|Description|
|-|-|-|-|
|ZoneBegin|||Starts a profiling zone|
|ZoneBeginN|`string`:&nbsp;name<br>||Starts a profiling zone with a custom name|
|ZoneText|`string`:&nbsp;text<br>||Sets a text for the current zone|
|Message|`string`:&nbsp;text<br>||Sends a message|
|ZoneName|`string`:&nbsp;text<br>||Sets a zone name on a per-call basis|
|ZoneEnd|||Ends the current profiling zone|

### Operators
_This usertype has no operators_

### Example
```lua
local Test = {}

function Test:OnUpdate(deltaTime)
    tracy.ZoneBegin()

    for i = 1, 1000000 do
        local a = math.random(1, 100)
        local b = math.random(1, 100)
        local c = a + b
    end

    tracy.ZoneEnd()
end

return Test
```