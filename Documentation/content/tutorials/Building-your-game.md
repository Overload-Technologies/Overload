+++
title = 'Building Your Game'
weight = 2
+++

# 0. Table of contents
- [0. Table of contents](#0-table-of-contents)
- [1. Creating a scene](#1-creating-a-scene)
- [2. Setuping your project](#2-setuping-your-project)
- [3. Building](#3-building)
- [4. Profiling](#4-profiling)
- [5. Releasing](#5-releasing)

# 1. Creating a scene
The first step to build your game is to create a scene. If you haven't followed the [Getting Started](https://github.com/adriengivry/Overload/wiki/Getting-Started) guide, we encourage you to do so.

For next steps, we will use the scene created in the [Getting Started](https://github.com/adriengivry/Overload/wiki/Getting-Started) guide.

# 2. Setuping your project
Before trying to build your project, you should configure it to your needs. For this, let's open the `Project Settings` window. 

![image](https://user-images.githubusercontent.com/33324216/84598984-a9e74c00-ae3c-11ea-9f08-ac61dd4191ed.png)

As you can see, you can configure several aspect of your project, from physics, window to rendering settings.

Every project made with Overload is composed of an `.ovproject` file. This file is a simple INI file (`key=value`), containing all the settings of your project. Modifying and saving your settings in the Project Settings will modify your `.ovproject` file content.

As it is possible to setup your project by editing the `.ovproject` file, it is recommended to configure it via the Project Settings window.

For this guide, we will only describe the most important settings:
| Setting | Description |
| ------- | ----------- |
|Scene Management: **Start Scene**|Defines the scene that should get loaded when launching your game|
|Windowing: **Fullscreen**|Set this to true if you want your game to startup in fullscreen mode|
|Windowing: **Resolution**|Defines the size of the game window. When fullscreen is enabled, this setting will defines the render resolution|
|Build: **Development build**|Enabling this option will launch your game in Debug mode (Less-optimized but debuggable with a built-in profiler). If you've compiled Overload from sources, make sure `OvGame` has been compiled in `Debug` and `Release` modes to launch your game in all scenarios (Development build or Release build)|

In our case, Overload has been compiled from sources in `Debug` and `Release` mode, thus, "Development build" option can be toggled on or off. If you are using an Overload release, you should be able to use both options.

Let's toggle the "Development build" option on for now (It should be on by default).

The other setting we will change here is the "Start Scene". We will simply have to drag and drop our saved scene from the Asset Browser to the "Start Scene" field.

![image](https://user-images.githubusercontent.com/33324216/84599472-52e37600-ae40-11ea-959b-1053aa405331.png)

Click apply, and voilà, you should be good to go for your first build.

# 3. Building
Let's now heads to the menu bar, and click onto "Build".
There are multiple options here.

|Option|Description|
|------|-----------|
|Build Game|Build your game at the specified location on disk|
|Build Game and run|Build your game at the specified location on disk and launch it|
|Temporary build|Build your game in a temporary folder (`%appdata%\OverloadTech\OvEditor\TempBuild\`) and launch it. This option is the best way to try your game quickly|

We will use the "Temporary build" option as it is the quickest way to try our game.

If the build failed (Watch the console for logs), make sure:
- You are using an Overload release OR you have compiled the right version of `OvGame` (Debug mode for this guide)
- You haven't modified any other setting
- The "Start Scene" is correctly setup

If the build worked, heads to the next section!

# 4. Profiling
If you've followed the guide, you should have your game in a standalone executable, in debug mode. You should see something similar to:

![GameBuild](https://user-images.githubusercontent.com/33324216/84599679-d5b90080-ae41-11ea-9ffb-b6a33fe1cd4c.gif)

As you can see, building a game in development mode will automatically displays the profiler. The profiler is a useful tool to identify bottlenecks in your game. In my case, `[Post-Update]` is taking most of frame time, which is ok because the game is setup to use vertical synchronisation (vsync) which limits your game framerate to your monitor refresh rate. You can toggle the overlay by pressing `F12`.

# 5. Releasing
Let's now try a release build for our game. Simply uncheck the "Development build" option and click "Apply". As mentionned before, if you are using Overload from sources, makes sure `OvGame` is compiled in `Release` mode for this "Non-Development build".

![image](https://user-images.githubusercontent.com/33324216/84599779-9c34c500-ae42-11ea-9f96-a0de4b6fc66d.png)

Click again onto "Temporary Build" from the Build menu, and notice how smooth your game is!

![GameBuildRelease](https://user-images.githubusercontent.com/33324216/84599855-1e24ee00-ae43-11ea-989c-d70cf699bfed.gif)

There is no more profiler attached to your game and the executable is running in optimized mode. You still can press `F12` to display the FPS counter.



