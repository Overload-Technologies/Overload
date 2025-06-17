+++
title = 'Creating Scripts'
weight = 3
+++

### Creating a script
Every scripts in Overload are behaviours, meaning that when creating a script, your custom usertype will get interpreted by the engine as a behaviour that can be attached to any Actor.

In order to create a script, right click onto the `Scripts` folder in the Asset Browser window, click onto "New script...", enter a name and click enter.

![image](https://user-images.githubusercontent.com/33324216/84583873-09594380-adcc-11ea-9bb5-d84db9af5204.png)


### Implementing your own behaviour
In order to create gameplay interactions in your scripts, you'll need to implement some of these functions:

|Name|Input|Output|Description|
|-|-|-|-|
|OnAwake|`Usertype`:&nbsp;instance<br>||Called when the scene start right before OnStart|
|OnStart|`Usertype`:&nbsp;instance<br>||Called when the scene start right after OnAwake|
|OnEnable|`Usertype`:&nbsp;instance<br>||Called when the behaviour gets enabled (owner SetActive set to true)|
|OnDisable|`Usertype`:&nbsp;instance<br>||Called when the behaviour gets disabled (owner SetActive set to false)|
|OnDestroy|`Usertype`:&nbsp;instance<br>||Called when the behaviour gets destroyed|
|OnUpdate|`Usertype`:&nbsp;instance<br>`number`:&nbsp;deltaTime<br>||Called every frame|
|OnFixedUpdate|`Usertype`:&nbsp;instance<br>`number`:&nbsp;fixedDeltaTime<br>||Called every physics frame|
|OnLateUpdate|`Usertype`:&nbsp;instance<br>`number`:&nbsp;deltaTime<br>||Called every frame after OnUpdate|
|OnCollisionEnter|`Usertype`:&nbsp;instance<br>`PhysicalObject`:&nbsp;collideWith<br>||Called when the owner of this behaviour enter in collision with another physical object|
|OnCollisionStay|`Usertype`:&nbsp;instance<br>`PhysicalObject`:&nbsp;collideWith<br>||Called when the owner of this behaviour is in collision with another physical object|
|OnCollisionExit|`Usertype`:&nbsp;instance<br>`PhysicalObject`:&nbsp;collideWith<br>||Called when the owner of this behaviour exit from collision with another physical object|
|OnTriggerEnter|`Usertype`:&nbsp;instance<br>`PhysicalObject`:&nbsp;triggeredBy<br>||Called when the owner of this behaviour enter in trigger with another physical object|
|OnTriggerStay|`Usertype`:&nbsp;instance<br>`PhysicalObject`:&nbsp;triggeredBy<br>||Called when the owner of this behaviour is in trigger with another physical object|
|OnTriggerExit|`Usertype`:&nbsp;instance<br>`PhysicalObject`:&nbsp;triggeredBy<br>||Called when the owner of this behaviour exit from trigger with another physical object|

### Example
```lua
-- Holds data that are shared between functions of this usertype
local MoveUpDown =
{
    elapsed = 0
}

-- Called when the scene starts
function MoveUpDown:OnStart()
end

-- Called every frame (The passed deltaTime holds the time elapsed between the current and previous frame in seconds)
function MoveUpDown:OnUpdate(deltaTime)
    -- Here, elapsed is incremented to sum the elapsed time since start
    self.elapsed = self.elapsed + deltaTime

    -- Stores the transform component instance into a variable
    transform = self.owner:GetTransform()

    -- Invoke SetPosition function with `:` to send the transform instance as first parameter to this function
    -- `transform:SetPosition(...)` is equivalent to `transform.SetPosition(transform, ...)`
    transform:SetPosition(Vector3.new(0, math.sin(self.elapsed), 0))

end

-- Returns the usertype so the engine has a reference to it
return MoveUpDown
```