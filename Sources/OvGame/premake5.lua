project "OvGame"
	language "C++"
	cppdialect "C++20"
	targetdir (outputdir .. "%{cfg.buildcfg}/%{prj.name}")
	objdir (objoutdir .. "%{cfg.buildcfg}/%{prj.name}")
	debugdir (outputdir .. "%{cfg.buildcfg}/%{prj.name}")
	fatalwarnings { "All" }
	
	files {
		"**.h",
		"**.inl",
		"**.cpp",
		"**.lua",
		"**.ini",
	}

	includedirs {
		-- Dependencies
		dependdir .. "glad/include",
		dependdir .. "ImGui/include",
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
		"assimp",
		"bullet3",
		"glad",
		"glfw",
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
		"OvWindowing"
    }

	filter { "configurations:Debug" }
		defines { "DEBUG", "_DEBUG" }
		symbols "On"
		kind "ConsoleApp"

	filter { "configurations:Release" }
		defines { "NDEBUG" }
		optimize "Speed"
		kind "WindowedApp"

	filter { "system:windows" }
		-- forces post-build commands to trigger even if nothing changed
		fastuptodate "Off"

		files {
			"**.rc",
		}

		links {
			-- Precompiled Libraries
			"dbghelp.lib",
			"opengl32.lib",
		}

		postbuildcommands {
			"for /f \"delims=|\" %%i in ('dir /B /S \"%{dependdir}\\*.dll\"') do xcopy /Q /Y \"%%i\" \"%{outputdir}%{cfg.buildcfg}\\%{prj.name}\"",
			"xcopy \"%{resdir}Engine\\*\" \"%{outputdir}%{cfg.buildcfg}\\%{prj.name}\\Data\\Engine\" /y /i /r /e /q",
			"EXIT /B 0"
		}

	filter { "system:linux" }
		links {
			"dl",
			"pthread",
			"GL",
			"X11",
		}

		-- Force inclusion of all symbols from these libraries
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
			"-Wl,--allow-multiple-definition",  -- Tracy and Bullet3 have some duplicate symbols
		}

		postbuildcommands {
			"mkdir -p %{outputdir}%{cfg.buildcfg}/%{prj.name}/Data",
			"cp -r %{resdir}Engine %{outputdir}%{cfg.buildcfg}/%{prj.name}/Data/Engine",
			"true"
		}

