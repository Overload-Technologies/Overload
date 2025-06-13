+++
title = 'Physics (table)'
+++

### Description
Global table containing physics-related functions

### Functions
|Name|Input|Output|Description|
|-|-|-|-|
|Raycast|`Vector3`:&nbsp;origin<br>`Vector3`:&nbsp;direction<br>`number`:&nbsp;distance<br>|`RaycastHit`|Casts a ray against all Physical Object in the Scene and returns information on what was hit|

### Examples
```lua
function MyScript::OnUpdate()
    -- Constants
    local distance = 5

    -- Prepare raycast input
    local transform = self.owner:GetTransform()
    local worldPos = transform:GetWorldPosition()
    local forward = transform:GetForward()

    -- Perform raycast
    local hit = Physics.Raycast(worldPos, forward, distance)

    -- Evaluate raycast result
    if hit ~= nil then
        Debug.Log("Object found!")
    else
        Debug.Log("No object found...")
    end
end
```