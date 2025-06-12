![overload-banner-2](https://github.com/user-attachments/assets/feef5188-582c-4812-bb80-b893026811ee)

<p align="center">
  <a href="https://github.com/Overload-Technologies/Overload/releases">Releases</a> |
  <a href="#screenshots">Screenshots</a> |
  <a href="#features">Features</a> |
  <a href="https://github.com/Overload-Technologies/Overload/wiki">Wiki</a> |
  <a href="https://github.com/Overload-Technologies/Overload/blob/develop/CONTRIBUTING.md">Contributing</a>
<br/>
<br/>
<br/>
<a href="https://github.com/Overload-Technologies/Overload/releases"><img alt="platforms" src="https://img.shields.io/badge/platforms-Windows-blue?style=flat-square"/></a>
<a href="https://github.com/Overload-Technologies/Overload/releases"><img alt="release" src="https://img.shields.io/github/v/release/adriengivry/overload?style=flat-square"/></a>
<a href="https://github.com/Overload-Technologies/Overload/tree/develop/Sources/Overload"><img alt="size" src="https://img.shields.io/github/repo-size/adriengivry/overload?style=flat-square"/></a>
<br/>
<a href="https://github.com/Overload-Technologies/Overload/issues"><img alt="issues" src="https://img.shields.io/github/issues-raw/adriengivry/overload.svg?color=yellow&style=flat-square"/></a>
<a href="https://github.com/Overload-Technologies/Overload/pulls"><img alt="pulls" src="https://img.shields.io/github/issues-pr-raw/adriengivry/overload?color=yellow&style=flat-square"/></a>
<br/>
<a href="https://github.com/Overload-Technologies/Overload/blob/develop/LICENSE"><img alt="license" src="https://img.shields.io/github/license/adriengivry/overload?color=green&style=flat-square"/></a>
<a href="https://github.com/Overload-Technologies/Overload/releases"><img alt="downloads" src="https://img.shields.io/github/downloads/adriengivry/overload/total?color=green&style=flat-square"></a>
<br/>
<br/>
<br/>
<a href="https://discord.gg/wqe775s"><img src="https://img.shields.io/discord/622075717659656195.svg?label=&logo=discord&logoColor=ffffff&color=7389D8&labelColor=6A7EC2" height=30></img></a>
</p>

# What's Overload?
Overload is a free, open-source 3D game engine made in C++ with Lua as its scripting language.

Originally created in 2019 by [Benjamin VIRANIN](https://github.com/BenjaminViranin), [Max BRUN](https://github.com/maxbrundev), and [Adrien GIVRY](https://github.com/adriengivry) as a graduation project, it has since evolved into an community-driven initiative supported by dozens of contributors.

Overload pillars are:
- 🐣 **Ease of use** – Inspired by other commercial engines, so you feel right at home.
- 🧊 **Simplicity** – Minimalistic by design, avoiding unnecessary complexity.
- 💭 **Pragmatic Design** – Features are carefully considered and deeply integrated.
- 📄 **Documentation** – Scripting API and documented source-code.
- 🤝 **Community-Driven** – Welcoming contributions and feedback from all developers.
- ⚙️ **Modern C++20** – Leveraging the power and safety of up-to-date C++ features.

# Features
- Lua scripting
- Game editor
- Physically-Based Rendering (PBR)
- Custom shaders support
- Windows game building
- Profiling tools
- Material editor
- Spatial audio
- Rigidbody physics
- And many more to come...

Check out our [issues](https://github.com/Overload-Technologies/Overload/issues) and [pull requests](https://github.com/Overload-Technologies/Overload/pulls) to learn more about what's coming next!

# Quick Start (TL;DR)
*Assuming you are on Windows, have Visual Studio 2022 installed, and have about 5 minutes to spare!*
1. Clone Overload
2. Inside of the repository folder, run `OpenInVisualStudio.bat`
3. Build the project, voilà!

- **⭐ Bonus:** get one of Overload's [sample projects](https://github.com/Overload-Technologies/Overload/wiki/Sample-Projects)!
- **✨ Extra Bonus:** check-out the [documentation](https://github.com/Overload-Technologies/Overload/wiki).

More in-depth guide on getting started available [here](#getting-started).

In a rush? [Get the latest release](https://github.com/Overload-Technologies/Overload/releases)!

# Architecture
Overload is divided into 11 modules: 9 libraries (SDK), and 2 executables (Applications).

## Overload SDK
The Overload SDK is the core of the engine. It is a set of libraries used by our applications: `OvGame` and `OvEditor`.
We designed theses libraries with reusability in mind. They are highly modular and easy to extract from a game engine context.
- `OvDebug`: Logging and assertions.
- `OvTools`: Serialization, file system, platform, events, clock, and more.
- `OvMaths`: Vectors, matrices, quaternions, transforms.
- `OvAudio`: Audio engine, built around [SoLoud](https://github.com/jarikomppa/soloud).
- `OvPhysics`: Physics engine, built around [Bullet3](https://github.com/bulletphysics/bullet3).
- `OvRendering`: Fully agnostic rendering engine (HAL), with OpenGL implementation using [GLAD](https://github.com/Dav1dde/glad).
- `OvWindowing`: Handles inputs and windows using [GLFW](https://github.com/glfw/glfw).
- `OvUI`: Widget-based UI, leveraging [ImGui](https://github.com/ocornut/imgui) under the hood.
- `OvCore`: Component-based scene system, scripting, and resource management.

## Overload Applications
Overload applications use the Overload SDK to operate.
- `OvGame`: A data-driven executable for any game built with Overload.
- `OvEditor`: An editor for building your game.

![editor](https://github.com/user-attachments/assets/3e16c52f-1607-4c7b-a34b-c98348acdf70)

## Dependencies
Overload depends on a few third-party libraries:
- [GLAD](https://github.com/Dav1dde/glad) (OpengGL Graphics API)
- [GLFW](https://github.com/glfw/glfw) (Windowing and inputs)
- [Assimp](https://github.com/assimp/assimp) (3D model loader)
- [Bullet3](https://github.com/bulletphysics/bullet3) (Physics)
- [SoLoud](https://github.com/jarikomppa/soloud) (Audio)
- [Tinyxml2](https://github.com/leethomason/tinyxml2) (XML serializer)
- [Sol3](https://github.com/ThePhD/sol2) (Lua binding)
- [ImGui](https://github.com/ocornut/imgui) (GUI)
- [Premake5](https://github.com/premake/premake-core) (Project generation)

# Getting started
## Running Overload from a Release Build
Get started with Overload in no time by downloading one of our [release builds](https://github.com/Overload-Technologies/Overload/releases). While this is the fastest way to get started, you might miss out on some cool features we're cooking up!

After downloading the archive, unzip it and run the `Overload.exe` executable file.

## Building Overload from Sources

### Quick Start (For Visual Studio 2022)
To start working with Overload quickly, clone the repository and run the `OpenInVisualStudio.bat` script. Project files will be automatically generated, and Visual Studio will open with the generated solution (`Overload/Sources/Overload.sln`).

```powershell
# These 2 lines will clone Overload, generate project files, and open the Visual Studio solution.
git clone https://github.com/Overload-Technologies/Overload
.\Overload\OpenInVisualStudio.bat
```

### Generating Project Files (For Any IDE)
*Note: This step is performed automatically when using `OpenInVisualStudio.bat`*

Overload uses Premake5 to generate project files. To generate these files, execute the `GenerateProjects.bat` located in the `Scripts/` folder.

By default, `GenerateProjects.bat` will generate project files for Visual Studio 2022.

If you'd like to use another IDE, you'll need to run `GenerateProjects.bat` from the command line:

```powershell
.\Scripts\GenerateProjects.bat <ide_of_your_choice>
```

*Please refer to [Premake5's documentation](https://premake.github.io/docs/Using-Premake) to find supported IDEs.*

> ⚠️ Some Premake5-supported IDEs might still not work with Overload.

### Building From the Command Line (MSVC Only)
*Note: Before building, make sure that you generated the Visual Studio solution.*

If you'd like to build Overload directly from the command line (without opening Visual Studio), you can use the `BuildAll.bat` script located in `Scripts/MSVC/`. By default, `BuildAll.bat` will build the project in `Debug` mode, but you can choose the configuration you want by providing an argument:
```powershell
.\Scripts\MSVC\BuildAll.bat Release
```

## Tutorials & Scripting API
Learn how to create your own games using Overload by visiting our [wiki](https://github.com/Overload-Technologies/Overload/wiki).

# Contributing
Overload is open to contributions of all kinds. Feel free to open issues (feature requests or bug reports) or submit pull requests.

If you'd like to contribute, please refer to our [contribution guildelines](https://github.com/Overload-Technologies/Overload/blob/develop/CONTRIBUTING.md).

# Minimum Requirements
| | |
|-|-|
| **RAM** | 1GB |
| **OS**  | Windows 7 |
| **GPU** | Graphics card supporting OpenGL 4.5 |
| **CPU** | x64 |

# Screenshots
![editor](https://github.com/user-attachments/assets/b1ab6300-774a-4733-a810-4cece269aef2)
![shading](https://github.com/user-attachments/assets/414c72e0-cea5-4710-8d5e-c8d03f56c00f)
![reflections](https://github.com/user-attachments/assets/2a54a9c4-1884-4271-ac77-1da57aa43a9b)
![pbr](https://github.com/user-attachments/assets/712d9b6e-4bf7-4cf7-ad23-01aeb1de9713)
![custom-shaders](https://github.com/user-attachments/assets/8cfed591-a565-45d2-aa4c-8e3926bdeea6)
![intel-sponza](https://github.com/user-attachments/assets/325fb0b5-54fc-4110-9011-5a51edcebf72)
![intel-sponza-2](https://github.com/user-attachments/assets/87acd196-09aa-4399-af71-f7ac6d1ee34f)
![intel-sponza-3](https://github.com/user-attachments/assets/28a8c112-d723-4e02-b790-a0b023dbc5c6)
