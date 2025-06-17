+++
title = 'Getting Started'
weight = 1
+++

In this page, we will see how to get Overload, create a new project, and create our very first scene.

# 0. Table of contents

- [0. Table of contents](#0-table-of-contents)
- [1. Glossary](#1-glossary)
- [2. Getting Overload](#2-getting-overload)
    - [2.1. From Sources](#21-from-sources)
    - [2.2. From a Release build](#22-from-a-release-build)
- [3. Project Hub](#3-project-hub)
- [4. Editor Layout](#4-editor-layout)
    - [4.1. Scene View](#41-scene-view)
    - [4.2. Game View](#42-game-view)
    - [4.3. Toolbar](#43-toolbar)
    - [4.4. Asset Browser](#44-asset-browser)
    - [4.5. Hierarchy](#45-hierarchy)
    - [4.6. Inspector](#46-inspector)
    - [4.7. Console](#47-console)
    - [4.8. Profiler](#48-profiler)
    - [4.9. Hardware Info](#49-hardware-info)
    - [4.10. Asset View](#410-asset-view)
    - [4.11. Help](#411-help)
    - [4.12. Project Settings](#412-project-settings)
    - [4.13. Material Editor](#413-material-editor)
    - [4.14. Asset Metadata Editor](#414-asset-metadata-editor)
- [5. Creating our very first scene!](#5-creating-our-very-first-scene)
    - [5.1. Creating an empty scene](#51-creating-an-empty-scene)
    - [5.2. Creating actors](#52-creating-actors)
    - [5.3. Creating a material](#53-creating-a-material)
    - [5.4. Applying a material](#54-applying-a-material)
    - [5.5. Scripting](#55-scripting)
    - [5.6. Playing your game!](#56-playing-your-game)

# 1. Glossary
Here are some terms that you'll encounter while using or reading about Overload.

|Term|Definition|
|----|----------|
|Window|As the Overload editor is fully modulable, it is possible to undock or move around parts of the editor. We will call these parts windows|
|Menu Bar|Top bar of your main window, containing menus such as `File`, `Build`, `Window`...|
|Actor|Any entity with components in Overload is called an actor. Actors are part of a scene|
|Scene|Collection of actors. It can be serialized to XML format to be loaded another time by the editor or the game|
|Mesh|Set of vertices and indices. You can't manipulate meshes directly from the editor|
|Model|Collection of meshes. Each model can be composed of multiple meshes. They directly represent a model file (fbx, obj...). They can be spawned into a scene|
|Material|AAsset that associate a shader with a set of settings, defining the visual look of any object using it|
|Asset Metadata|Set of settings that will influence the way an asset should be loaded|
|Gizmo|Set of 3D arrows that are draggable, used to manipulate an actor in the Scene View|

# 2. Getting Overload
### 2.1. From Sources
Compiling Overload sources works best with Visual Studio 2022.
You can find instructions on how to compile it yourself here: https://github.com/Overload-Technologies/Overload?tab=readme-ov-file#building-overload-from-sources.

### 2.2. From a Release build
You can download a release by checking the [releases list](https://github.com/adriengivry/Overload/releases).

# 3. Project Hub
The first window you'll see when launching Overload is the Project Hub. Basically, the Project Hub is a place where you can create new projects or open a new project. Projects that has already been opened will get displayed in a list.
For the guide next steps, we will assume that you have created a new project.

![image](https://user-images.githubusercontent.com/33324216/84580417-ca63c780-ada4-11ea-9665-3b7ce208e3aa.png)

# 4. Editor Layout
Let's now take a look at the **editor**!

When launching Overload for the first time, you should have an editor layout similar to this one:

![image](https://user-images.githubusercontent.com/33324216/84580498-7d342580-ada5-11ea-9d28-8c6f786dfa89.png)
Actually, only a few editor windows will be opened when you'll launch the editor. You can open additional windows by clicking the `Window` menu in the top bar (AKA menu bar).

Let's dive into those windows:
### 4.1. Scene View
The Scene View can be considered as the main view of Overload editor. It will give you a real-time feedback of what is happening in your current scene. This window is necessary to create your scene (Placing and moving objects).

You can freely move in this view using these controls:
| Action | Input |
| ------ | ----- |
| Rotate the camera | Right Mouse Button |
| Zoom in/out | Mouse Wheel |
| Move camera Forward | Right Mouse Button **+** W |
| Move camera Backward | Right Mouse Button **+** S |
| Move camera Left | Right Mouse Button **+** A |
| Move camera Right | Right Mouse Button **+** D |
| Move camera Up | Right Mouse Button **+** E |
| Move camera Down | Right Mouse Button **+** Q |
| Orbit around selection | ALT + Middle Mouse Button |
| Move to selection | F |
| Move to selection (From Top) | Up Arrow |
| Move to selection (From Bottom) | Down Arrow |
| Move to selection (From Left) | Left Arrow |
| Move to selection (From Right) | Right Arrow |
| Move to selection (From Front) | Page Up |
| Move to selection (From Back) | Page Down |
| Delete selection | Del |
| Switch to translation gizmo | W |
| Switch to rotation gizmo | E |
| Switch to scaling Gizmo | R |

### 4.2. Game View
The Game View, like the Scene View, reflects what is happening in your scene, but only from a fixed camera, which correspond to your game main camera. The editor will automatically focus the Game View when playing the scene

### 4.3. Toolbar
A simple window providing some controls onto your scene. It allows you to play, pause, resume, step, and refresh your lua behaviours.

### 4.4. Asset Browser
You'll find there your project file hierarchy. Any file in your project should be placed into `Assets` or `Scripts`. You can create/move/delete files there. Plenty actions are available for each different file type (Accessible by right-clicking onto a file). A lot of editor fields and windows can take an asset as input (Texture, Scene, Shader...), the Asset Browser allows you to drag and drop these assets.

### 4.5. Hierarchy
The Hierarchy window shows the current scene hierarchy with relations between actors. You can select/delete/move/create actors in this view.

### 4.6. Inspector
In the Inspector, you'll be able to view and edit the currently selected actor. Adding, tweaking and removing components, changing actor settings (Name, tag, activation state...).

### 4.7. Console
As a way to communicate with you, the user, Overload is using logs. These logs (Information, Warnings, Errors), will be found into the console.

### 4.8. Profiler
The profiler will help you identifying bottlenecks in your games. It will show you timings of key modules of the engine, such as the physics system, the scripting system or the renderer.

### 4.9. Hardware Info
This window will show you some information about your hardware (CPU, GPU and RAM usage).

### 4.10. Asset View
The Asset View is useful to preview assets. You can drag and drop textures, models and materials to this view to preview them.

### 4.11. Help
Contains information about editor controls.

### 4.12. Project Settings
This window contains every settings related to the current project you are working on. You can setup various things there, such as Graphics API settings, physics settings, startup scene...

### 4.13. Material Editor
The Material Editor is a window in which you'll be able to edit your materials. From the attached shader to the parameters values, the visual appearance of your game is determined by how you configure your materials.

### 4.14. Asset Metadata Editor
This window let you edit the metadata attached to an asset. In order to edit the metadata of an asset, you should right click onto this asset, and select `Edit metadata` (This option won't be shown if the selected asset cannot carry metadata).

# 5. Creating our very first scene!
Now that the editor has no more secret for you, let's use what we learnt and create our very first game scene!

### 5.1. Creating an empty scene
As you can see, when launching the editor, a default scene is shown. At any time, you can get back to this empty scene by clicking onto `File -> New Scene`

![image](https://user-images.githubusercontent.com/33324216/84580498-7d342580-ada5-11ea-9d28-8c6f786dfa89.png)

This scene is composed of 3 actors : 2 lights (Ambient and directional), and a camera.

Having a camera in a scene is essential for the game to show something onto the screen. Without any camera, your game will just show a black screen. If you want to test the "no camera effect", you can click onto the "Main Camera" (From the hierarchy or the Scene View), and then click onto the Game View window. By clicking the "Active" checkbox in the Inspector window, you should see the Game View going from blue (Default camera background color), to black (No camera is rendering the scene). Leave the "Main Camera" active for next steps.

We will start to build our scene from this base scene. You can go to the menu bar: `File -> Save Scene` and select a location within your project "Assets" folder.
Once the scene has been saved, you should see a log in the console saying that your scene has successfully been saved.

### 5.2. Creating actors
This scene looks pretty empty, let's try to populate it with some actors!

In order to add an actor to the scene, go to the menu bar and click onto: `Actors -> Primitives -> Cube`.

![image](https://user-images.githubusercontent.com/33324216/84583155-8fbd5780-adc3-11ea-9222-f96fff826c12.png)

You should now see a cube in your scene.

![image](https://user-images.githubusercontent.com/33324216/84583124-1de50e00-adc3-11ea-9647-9941074aa070.png)

You can now give a unique name to your new actor by editing "Name" in the Inspector window.

![image](https://user-images.githubusercontent.com/33324216/84583142-70bec580-adc3-11ea-9550-e0de52d839bb.png)

Let's now try to duplicate this actor. Right click onto your actor in the Hierarchy and select `Duplicate`.

![image](https://user-images.githubusercontent.com/33324216/84583204-41f51f00-adc4-11ea-9b25-839e675bd60c.png)

The duplicated actor should get automatically selected and ready to edit. Try to move it in the Scene View using the translation gizmo.

![image](https://user-images.githubusercontent.com/33324216/84583245-74068100-adc4-11ea-9336-8ae5b4fc270b.png)


### 5.3. Creating a material
Well, having two cubes is fine, but it would be better if we could differentiate them visually. Let's add a color to our first cube!

For that, you'll first need to create a material. In order to achieve this, right click onto your `Assets` folder in the Asset Browser, and click onto `Create -> Material -> Standard`. A text field should appear, this is where you can name your material. Enter a name for your new material and hit enter.

![image](https://user-images.githubusercontent.com/33324216/84583337-72898880-adc5-11ea-8033-4b277de6ddd2.png)

What we just did here is that we've create a material using the `Standard` shader which is an engine shader. With Overload, you can create your very own shaders and create `Empty` materials that you'll configure for your custom shader using the Material Editor. But for now, we will stick with the `Standard` material preset.

The `Assets` folder should automatically expand itself and the Asset Preview window should now be opened.

![image](https://user-images.githubusercontent.com/33324216/84583362-00657380-adc6-11ea-8b2e-58b74755ac7a.png)

As you can see in the Material Editor window, we are using the Shader `:Shaders\Standard.glsl`. The `:` character prefix tells us that the asset is located under the `Engine` folder. You can freely navigate this folder in the Asset Browser but you shouldn't modify its content.

Let's modify our material. To do so, expand the `Shader Settings` header. We won't need to tweak any `Material Settings` here as they are more "advanced stuff".

The `Shader Settings` directly mirror the parameters of a shader. When you'll create your own shaders, you'll be able to see in this section every parameters of your custom shaders.

Let's scroll down to the "Diffuse" parameter and switch its display mode to "RGBA".

![image](https://user-images.githubusercontent.com/33324216/84583500-fb54f400-adc6-11ea-932d-bfe895a8ec99.png)

The diffuse color of an object is basically it's main color. You can now click onto the white square and choose a color for this material.

![image](https://user-images.githubusercontent.com/33324216/84583517-28a1a200-adc7-11ea-9ab2-e06291ad32df.png)

You should be able to see your changes in real-time in the Asset View window.

Leave the color selection view by clicking anywhere outside of it, scroll to the top of the Material Editor and click onto "Save to file". This will save your changes to your asset. This is a very important step in order to keep your material asset up-to-date. If you forget this step, your material file will remain unchanged, and the color change won't get reloaded the next time you'll start the editor.

### 5.4. Applying a material
Great, it's now time to apply this new material to one of your cubes.

Select the cube onto which you want to apply your new material. Make the Inspector window visible and expand the Material Renderer header.

![image](https://user-images.githubusercontent.com/33324216/84583619-520efd80-adc8-11ea-8d60-e2ff63d37a19.png)

In this section you should see "lambert1", which is the name of the material slot stored into the model file attached to this cube model. As you can see, it is currently using the `:Material\Default.ovmat` which is an engine material.

In order to apply your material to this actor, you'll have to drag and drop your new material from the asset browser to the "lambert1" field.

![image](https://user-images.githubusercontent.com/33324216/84583679-217b9380-adc9-11ea-82a9-ddbf64ad66d6.png)

Once done, you should be able to see the material you chose in the Material Editor onto your cube.

![image](https://user-images.githubusercontent.com/33324216/84583686-35bf9080-adc9-11ea-9012-83e84b453d0f.png)

### 5.5. Scripting
The last thing this starter guide will explain is the scripting system. A game is nothing without gameplay logic, the scripting system is an interface between you, the game developper, and the engine core.

So, for this last part, we will make our colored cube moving up and down!

The engine is using Lua as a scripting language. Lua itself is just a way for your to communicate with the engine.

Let's start by creating our script. Right click onto the `Scripts` folder in the Asset Browser window, click onto "New script...", enter a name and click enter.

![image](https://user-images.githubusercontent.com/33324216/84583873-09594380-adcc-11ea-9bb5-d84db9af5204.png)

Currently, there is a restriction in the engine that prevent us from having a complex script folder tree, thus, all your scripts should directly be placed under the `Scripts` folder.

Now that you script is created, you can try to open it.

![image](https://user-images.githubusercontent.com/33324216/84583894-766cd900-adcc-11ea-888a-7d6fd6c679c4.png)

If you have never set a default editor for .lua files, you'll be prompt to choose an editor to open your script. I personally use Visual Studio Code but any editor (Even notepad 😄) will work here.

After opening your script, you should have something similar to:

```lua
local MoveUpDown =
{
}

function MoveUpDown:OnStart()
end

function MoveUpDown:OnUpdate(deltaTime)
end

return MoveUpDown
```

The `local MoveUpDown = {...}` should contains any variable that your script wants to share between functions.

The `MoveUpDown:OnStart()` function is called once the scene your script (AKA behaviour) is in is loaded, and the `MoveUpDown:OnUpdate(deltaTime)` is called every frame. There are multiple engine functions that you can use in your scripts to create complex gameplay logic, we won't explain them here. Please, check our lua scripting reference for more information.

I won't go into detail, but this script is basically updating the owner (Actor onto which this script is attached) Y position to follow the sinus function.

```lua
local MoveUpDown =
{
    elapsed = 0
}

function MoveUpDown:OnStart()
end

function MoveUpDown:OnUpdate(deltaTime)
    self.elapsed = self.elapsed + deltaTime
    self.owner:GetTransform():SetPosition(Vector3.new(0, math.sin(self.elapsed), 0))

end

return MoveUpDown
```

Now that our script is done, we can attach it to an actor in our scene. Go back to Overload, select the actor you want this behaviour to apply on, and drag and drop your script onto the field in front of the "Add Script" button and press the button.

![image](https://user-images.githubusercontent.com/33324216/84584197-3eb36080-adcf-11ea-9a2b-7d7e9003fed3.png)

If you scroll to the bottom of the inspector, you should see your script in the component list

![image](https://user-images.githubusercontent.com/33324216/84584227-905beb00-adcf-11ea-9ab2-2cd19b17ed4c.png)

If an error has been found in your script, and error message will be logged into the console.

### 5.6. Playing your game!
Now that your scene is complete, save it (CTRL + S or `File -> Save Scene`). You can now hit the play button and admire your very first game made with Overload!

![GamePlaying](https://user-images.githubusercontent.com/33324216/84584271-2a239800-add0-11ea-8f63-d29a07326741.gif)




