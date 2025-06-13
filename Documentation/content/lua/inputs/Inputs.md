+++
title = 'Inputs (table)'
+++

### Description
Global table containing input-related functions

### Functions
|Name|Input|Output|Description|
|-|-|-|-|
|GetKeyDown|`Key`:&nbsp;key<br>|`boolean`|Returns true if the key has been pressed during the current frame|
|GetKeyUp|`Key`:&nbsp;key<br>|`boolean`|Returns true if the key has been released during the current frame|
|GetKey|`Key`:&nbsp;key<br>|`boolean`|Returns true if the key is currently down|
|GetMouseButtonDown|`MouseButton`:&nbsp;mouseButton<br>|`boolean`|Returns true if the mouse button has been pressed during the current frame|
|GetMouseButtonUp|`MouseButton`:&nbsp;mouseButton<br>|`boolean`|Returns true if the mouse button has been released during the current frame|
|GetMouseButton|`MouseButton`:&nbsp;mouseButton<br>|`boolean`|Returns true if the mouse button is currently down|
|GetMousePos||`Vector2`|Returns the current position of the mouse cursor|
|GetMouseScroll||`Vector2`|Returns the current scroll value|
|LockMouse|||Locks and hides the mouse cursor|
|UnlockMouse|||Unlocks and shows the mouse cursor|

### Operators
```lua
function MyScript::OnUpdate()
    -- Check if a key has been pressed this frame
    if Inputs.GetKeyDown(Key.SPACE):
        Debug.Log("Space pressed this frame!")
    end

    local movementDirection = Vector2.new(0, 0)

    -- Check if W (move up) is being pressed
    if Inputs.GetKey(Key.W):
        movementDirection.y = movementDirection.y + 1
    end

    -- Check if S (move down) is being pressed
    if Inputs.GetKey(Key.S):
        movementDirection.y = movementDirection.y - 1
    end

    -- Check if D (move right) is being pressed
    if Inputs.GetKey(Key.D):
        movementDirection.x = movementDirection.x + 1
    end

    -- Check if A (move left) is being pressed
    if Inputs.GetKey(Key.A):
        movementDirection.x = movementDirection.x - 1
    end

    Debug.Log("Movement Direction: " .. movementDirection)
end
```