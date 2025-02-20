![Overload Logo](https://user-images.githubusercontent.com/33324216/94352497-cc8c2200-0033-11eb-93e1-1a30386133b9.png)

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
- 🐣 Ease of use
- 📄 Documentation
- 🤝 Community
- ⚙️ Modern C++20 code

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

# Architecture
Overload is divided into 12 modules: 10 libraries (SDK), and 2 executables (Applications).

## Overload SDK
The Overload SDK is the core of the engine. It is a set of libraries used by our applications: `OvGame` and `OvEditor`.
We designed theses libraries with reusability in mind. They are highly modular and easy to extract from a game engine context.
- `OvAnalytics`: Code and hardware profiling
- `OvDebug`: Logging and assertions
- `OvTools`: Serialization, INI files, events, clock
- `OvMaths`: Vectors, matrices, quaternions, transforms
- `OvAudio`: Wraps irrKlang
- `OvPhysics`: Wraps Bullet3
- `OvRendering`: Rendering engine using OpenGL
- `OvWindowing`: GLFW and Windows API wrapper
- `OvUI`: Dear ImGui wrapped into an event-based and component-based UI system
- `OvCore`: Component-based scene system, scripting, and resource management

## Overload Applications
Overload applications use the Overload SDK to operate.
- `OvGame`: A data-driven executable for any game built with Overload
- `OvEditor`: An editor for building your game

![Editor](https://user-images.githubusercontent.com/33324216/94352908-fd228a80-0038-11eb-849a-c076bde4c7c6.PNG)

## Dependencies
Overload depends on a few third-party libraries:
- OpenGL with GLEW (Graphics API)
- GLFW (Windowing and inputs)
- Assimp (3D model loader)
- Bullet3 (Physics)
- irrKlang (Audio)
- Tinyxml2 (XML serializer)
- Sol2 (Lua binding)
- ImGui (GUI)
- Premake5 (Project generation)

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

# Requirements
- **RAM:** 100MB
- **OS:** Windows 7
- **GPU:** Graphics card supporting OpenGL 4.3
- **CPU:** x64

# Screenshots
![PBR Shading](https://user-images.githubusercontent.com/33324216/94352806-96e93800-0037-11eb-8d7f-9c9a318ca2c7.PNG)
![Material Editor](https://user-images.githubusercontent.com/33324216/94352805-96e93800-0037-11eb-883b-fdd8818b93a6.PNG)
![Standard Shader Library](https://user-images.githubusercontent.com/33324216/94352810-9781ce80-0037-11eb-8788-095794711b2c.PNG)
![Custom Shaders](https://user-images.githubusercontent.com/33324216/94352802-9650a180-0037-11eb-8931-c6b2163c0ef1.PNG)
![Realtime Lighting](https://user-images.githubusercontent.com/33324216/94352808-9781ce80-0037-11eb-8b91-3ec0ab06db45.PNG)
![Scene Edition](https://user-images.githubusercontent.com/33324216/94352809-9781ce80-0037-11eb-9adf-d216eb4d963e.PNG)
![Build System](https://user-images.githubusercontent.com/33324216/94352926-5db1c780-0039-11eb-88ef-7ca14a8bc821.PNG)
![Project Hub](https://user-images.githubusercontent.com/33324216/94352807-9781ce80-0037-11eb-911b-7e3d0d00ce41.png)
