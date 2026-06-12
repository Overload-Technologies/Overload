project "OvLuaApiTests"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++20"
	targetdir (outputdir .. "%{cfg.buildcfg}/%{prj.name}")
	objdir (objoutdir .. "%{cfg.buildcfg}/%{prj.name}")
	debugdir "%{wks.location}"
	fatalwarnings { "All" }

	files {
		"**.h",
		"**.inl",
		"**.cpp",
		"**.lua"
	}

	includedirs {
		-- Dependencies
		dependdir .. "assimp/include",
		dependdir .. "glad/include",
		dependdir .. "ImGui/include",
		dependdir .. "lua/include",
		dependdir .. "sol/include",
		dependdir .. "tinyxml2/include",
		dependdir .. "tracy",

		-- Overload SDK
		"%{wks.location}/Sources/OvAudio/include",
		"%{wks.location}/Sources/OvCore/include",
		"%{wks.location}/Sources/OvDebug/include",
		"%{wks.location}/Sources/OvMaths/include",
		"%{wks.location}/Sources/OvPhysics/include",
		"%{wks.location}/Sources/OvRendering/include",
		"%{wks.location}/Sources/OvTools/include",
		"%{wks.location}/Sources/OvUI/include",
		"%{wks.location}/Sources/OvWindowing/include",

		-- Current project
		"include"
	}

	links {
		-- Dependencies
		"bullet3",
		"freetype",
		"glad",
		"ImGui",
		"lua",
		"soloud",
		"tinyxml2",
		"tracy",

		-- Overload SDK
		"OvAudio",
		"OvCore",
		"OvDebug",
		"OvMaths",
		"OvPhysics",
		"OvRendering",
		"OvTools",
		"OvUI",
		"OvWindowing",

		-- Dependencies that others depend on
		"assimp",
		"glfw"
	}

	filter { "configurations:Debug" }
		defines { "DEBUG", "_DEBUG" }
		symbols "On"

	filter { "configurations:Release or configurations:Publish" }
		defines { "NDEBUG" }
		optimize "Speed"

	filter { "system:windows" }
		links {
			"dbghelp.lib",
			"opengl32.lib",
		}

	filter { "system:linux" }
		links {
			"dl",
			"pthread",
			"GL",
			"X11",
		}

		linkoptions {
			"-Wl,--whole-archive",
			outputdir .. "%{cfg.buildcfg}/ImGui/libImGui.a",
			outputdir .. "%{cfg.buildcfg}/bullet3/libbullet3.a",
			outputdir .. "%{cfg.buildcfg}/lua/liblua.a",
			outputdir .. "%{cfg.buildcfg}/soloud/libsoloud.a",
			outputdir .. "%{cfg.buildcfg}/OvAudio/libOvAudio.a",
			outputdir .. "%{cfg.buildcfg}/assimp/libassimp.a",
			outputdir .. "%{cfg.buildcfg}/tinyxml2/libtinyxml2.a",
			outputdir .. "%{cfg.buildcfg}/glad/libglad.a",
			"-Wl,--no-whole-archive",
			"-Wl,--allow-multiple-definition",
		}
