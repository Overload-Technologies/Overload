+++
title = 'Creating Shaders'
weight = 4
+++

# 1. Overload GLSL Flavor
Overload utilizes a custom GLSL flavor as its shading language, which includes the following features:
- `#include` statements
- `#pass` declaration
- `#feature` declaration
- Fragment and vertex section specification using `#shader vertex` and `#shader fragment`

## 1.1. File Extensions
Overload shaders can have the following file extensions:
- **.ovfx**: A compilable shader file that contains (or includes) both a vertex and a fragment shader.
- **.ovfxh**: A partial shader file (non-compilable) that can be included within another shader file.

## 1.2. Include Statements
Overload shaders can be included using the `#include` statement.

To include engine shaders, prefix the file name with a colon (`:`):
```cpp
#include ":Shaders/Vertex/Basic.ovfxh"
```

If the colon is omitted, the shader loader will search within the user project's assets:
```cpp
#include "Shaders/MyCustomShader.ovfxh"
```

## 1.3. Fragment/Vertex Sections
A complete shader (`.ovfx`) must include both vertex and fragment sections, which are specified using the `#shader vertex` and `#shader fragment` directives.

```glsl
#shader vertex
#version 430 core

// Vertex shader code

#shader fragment
#version 430 core

// Fragment shader code
```

# 2. Complete Shader Example
```glsl
// Unlit.ovfx

#shader vertex
#version 430 core

#include ":Shaders/Common/Buffers/EngineUBO.ovfxh"
#include ":Shaders/Common/Utils.ovfxh"

layout (location = 0) in vec3 geo_Pos;
layout (location = 1) in vec2 geo_TexCoords;

out VS_OUT
{
    vec3 FragPos;
    vec2 TexCoords;
} vs_out;

void main()
{
    vs_out.FragPos = vec3(ubo_Model * vec4(geo_Pos, 1.0));
    vs_out.TexCoords = geo_TexCoords;

    gl_Position = ubo_Projection * ubo_View * vec4(vs_out.FragPos, 1.0);
}

#shader fragment
#version 430 core

#include ":Shaders/Common/Buffers/EngineUBO.ovfxh"
#include ":Shaders/Common/Utils.ovfxh"

in VS_OUT
{
    vec3 FragPos;
    vec2 TexCoords;
} fs_in;

uniform vec4 u_Diffuse = vec4(1.0);
uniform sampler2D u_DiffuseMap;
uniform vec2 u_TextureTiling = vec2(1.0);
uniform vec2 u_TextureOffset = vec2(0.0);

out vec4 FRAGMENT_COLOR;

void main()
{
    vec2 texCoords = TileAndOffsetTexCoords(fs_in.TexCoords, u_TextureTiling, u_TextureOffset);
    FRAGMENT_COLOR = texture(u_DiffuseMap, texCoords) * u_Diffuse;
}
```

# 3. Shader Passes & Features (Variant System)

Shader passes & features allow you to create compile-time branching in your shaders, generating different variants of the same shader based on the pass and feature combinations. This approach is more efficient than runtime branching for certain operations.

## 3.1. Declaring Shader Passes & Features

To declare a shader pass, use the `#pass` directive, and for features, use the `#feature` directive. It's recommended to place these declarations at the top of your shader file (`.ovfx`) for better readability and organization.

```glsl
#pass SHADOW_PASS
#feature MY_COOL_FEATURE
#feature FOO

#shader vertex
// Vertex shader code goes here...

#shader fragment
// Fragment shader code goes here...
```

## 3.2. Understanding Shader Variants

Each feature added to your shader doubles the number of variants generated. The relationship between feature count and variant count is exponential:

| Feature Count | Variant Count |
|---------------|---------------|
| 0             | 1             |
| 1             | 2             |
| 2             | 4             |
| 3             | 8             |
| 4             | 16            |

Passes, on another hand, are exclusive, meaning that only one pass can be selected at a time, so their impact on variant count is linear:

| Feature Count | Pass Count | Variant Count |
|---------------|------------|---------------|
| 4             | 1          | 16            |
| 4             | 2          | 32            |
| 4             | 3          | 48            |
| 4             | 4          | 64            |

A shader with no `#pass PASS_NAME` directive will always have one pass (default).

**Performance Considerations:**
- Lower feature count = Faster compilation time
- Using runtime branches instead of features may impact runtime performance
- Consider carefully whether a branch should be a feature or a runtime condition

## 3.3. Using Passes & Features in Shader Code

You can utilize passes & features in your code with standard GLSL preprocessor directives:
- `#ifdef` - Check if pass/feature is enabled
- `#ifndef` - Check if pass/feature is disabled
- `#if`, `#else`, `#elif` - Conditional branching
- `#endif` - End conditional block

> Note: Branching for passes and features is strictly similar!

Example:
```glsl
#shader fragment
void main()
{
#ifdef SHADOW_PASS
    // This code only compiles for the shadow pass
    gl_FragColor = calculateShadowValue();
#else
    // This code compiles for any other pass
    gl_FragColor = calculateStandardShading();
#endif
}
```

## 3.4. Built-in Passes

The following built-in passes can be used to override Overload's default rendering behavior:

| Pass Name | Description | Required Inputs |
|--------------|-------------|----------------|
| `SHADOW_PASS` | Override the default shadow pass implementation. Useful for creating "holes" in shadows based on (for instance) the alpha of a texture, by discarding fragments below a certain threshold (e.g. window). This allows light to pass through semi-transparent surfaces. | `mat4 _LightSpaceMatrix` |
| `OUTLINE_PASS` | **(Editor Only)** Override the outline rendering (stencil + forward pass). Particularly useful when your vertex shader modifies vertex positions, ensuring the outline matches the transformed geometry. | `vec4 _OutlineColor` |
| `PICKING_PASS` | **(Editor Only)** Override the picking pass implementation. Important when using vertex transformations, allowing objects to be selected at their visible location rather than their original position. | `vec4 _PickingColor` |

When a pass provides an input, you can retrieve the value by declaring the input in your shader code, e.g.:
```glsl
#pass SHADOW_PASS

#shader vertex

// ...

#ifdef SHADOW_PASS
uniform mat4 _LightSpaceMatrix;
#endif

// ...

void main()
{
    gl_Position = _LightSpaceMatrix * ubo_Model * vec4(geo_Pos, 1.0);
}

// Rest of the code...
```
**⚠️ Not declaring a required input might result in error if the render pass tries to send the input value to the underlying shader program, so make sure you declare the inputs required by the pass you are overriding!**

# 4. Visual Studio Code Syntax Highlighting
To add syntax highlighting for `.ovfx` and `.ovfxh` files in Visual Studio Code:
1. Open the Command Palette (CTRL + Shift + P).
2. Search for and select "Preferences: Open User Settings (JSON)".
3. Edit or create a `files.associations` entry in the settings file:
```json
"files.associations": {
    "*.ovfx": "glsl",
    "*.ovfxh": "glsl"
},
```