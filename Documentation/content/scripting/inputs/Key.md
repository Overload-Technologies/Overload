+++
title = 'Key (enum)'
+++

### Description
Enumeration of keyboard keys.

- C++ related class: [`OvWindowing::Inputs::EKey`](https://github.com/Overload-Technologies/Overload/tree/main/Sources/Overload/OvWindowing/include/OvWindowing/Inputs/EKey.h)

### Values
|Name|Type|Description|
|-|-|-|
|UNKNOWN|`number`||
|SPACE|`number`||
|APOSTROPHE|`number`||
|COMMA|`number`||
|MINUS|`number`||
|PERIOD|`number`||
|SLASH|`number`||
|ALPHA_0|`number`||
|ALPHA_1|`number`||
|ALPHA_2|`number`||
|ALPHA_3|`number`||
|ALPHA_4|`number`||
|ALPHA_5|`number`||
|ALPHA_6|`number`||
|ALPHA_7|`number`||
|ALPHA_8|`number`||
|ALPHA_9|`number`||
|SEMICOLON|`number`||
|EQUAL|`number`||	
|A|`number`||
|B|`number`||
|C|`number`||
|D|`number`||
|E|`number`||
|F|`number`||
|G|`number`||
|H|`number`||
|I|`number`||
|J|`number`||
|K|`number`||
|L|`number`||
|M|`number`||
|N|`number`||
|O|`number`||
|P|`number`||
|Q|`number`||
|R|`number`||
|S|`number`||
|T|`number`||
|U|`number`||
|V|`number`||
|W|`number`||
|X|`number`||
|Y|`number`||
|Z|`number`||
|LEFT_BRACKET|`number`||
|BACKSLASH|`number`||
|RIGHT_BRACKET|`number`||
|GRAVE_ACCENT|`number`||
|WORLD_1|`number`||
|WORLD_2|`number`||
|ESCAPE|`number`||
|ENTER|`number`||
|TAB|`number`||
|BACKSPACE|`number`||
|INSERT|`number`||
|DELETE|`number`||
|RIGHT|`number`||
|LEFT|`number`||
|DOWN|`number`||
|UP|`number`||
|PAGE_UP|`number`||	
|PAGE_DOWN|`number`||
|HOME|`number`||
|END|`number`||	
|CAPS_LOCK|`number`||
|SCROLL_LOCK|`number`||
|NUM_LOCK|`number`||
|PRINT_SCREEN|`number`||
|PAUSE|`number`||
|F1|`number`||
|F2|`number`||
|F3|`number`||
|F4|`number`||
|F5|`number`||
|F6|`number`||
|F7|`number`||
|F8|`number`||
|F9|`number`||
|F10|`number`||
|F11|`number`||
|F12|`number`||
|F13|`number`||
|F14|`number`||
|F15|`number`||
|F16|`number`||
|F17|`number`||
|F18|`number`||
|F19|`number`||
|F20|`number`||
|F21|`number`||
|F22|`number`||
|F23|`number`||
|F24|`number`||
|F25|`number`||
|KP_0|`number`||
|KP_1|`number`||
|KP_2|`number`||
|KP_3|`number`||
|KP_4|`number`||
|KP_5|`number`||
|KP_6|`number`||
|KP_7|`number`||	
|KP_8|`number`||
|KP_9|`number`||
|KP_DECIMAL|`number`||
|KP_DIVIDE|`number`||
|KP_MULTIPLY|`number`||
|KP_SUBTRACT|`number`||
|KP_ADD|`number`||
|KP_ENTER|`number`||
|KP_EQUAL|`number`||
|LEFT_SHIFT|`number`||
|LEFT_CONTROL|`number`||
|LEFT_ALT|`number`||
|LEFT_SUPER|`number`||
|RIGHT_SHIFT|`number`||
|RIGHT_CONTROL|`number`||
|RIGHT_ALT|`number`||
|RIGHT_SUPER|`number`||
|MENU|`number`||

### Examples
```lua
function MyScript::OnUpdate()
    local spaceKey = Key.SPACE
    local upArrowKey = Key.UP

    if Inputs.GetKeyDown(spaceKey):
        Debug.Log("Space key pressed!")
    end

    if Inputs.GetKeyDown(upArrowKey):
        Debug.Log("Up arrow key pressed!")
    end
end
```