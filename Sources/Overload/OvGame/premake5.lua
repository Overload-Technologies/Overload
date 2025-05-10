project "OvGame"
	language "C++"
	cppdialect "C++20"
	targetdir (outputdir .. "%{cfg.buildcfg}/%{prj.name}")
	objdir (objoutdir .. "%{cfg.buildcfg}/%{prj.name}")
	debugdir (outputdir .. "%{cfg.buildcfg}/%{prj.name}")
	
	files {
		"**.h",
		"**.inl",
		"**.cpp",
		"**.lua",
		"**.ini",
		"**.rc"
	}

	includedirs {
		-- Dependencies
		dependdir .. "glad/include",
		dependdir .. "ImGui/include",
		dependdir .. "tracy",

		-- Overload SDK
		"%{wks.location}/OvAudio/include",
		"%{wks.location}/OvCore/include",
		"%{wks.location}/OvDebug/include",
		"%{wks.location}/OvMaths/include",
		"%{wks.location}/OvPhysics/include",
		"%{wks.location}/OvRendering/include",
		"%{wks.location}/OvTools/include",
		"%{wks.location}/OvUI/include",
		"%{wks.location}/OvWindowing/include",

		-- Current project
		"include"
	}

	links {
		-- Precompiled Libraries
		"dbghelp.lib",
		"opengl32.lib",

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

	postbuildcommands {
		"for /f \"delims=|\" %%i in ('dir /B /S \"%{dependdir}\\*.dll\"') do xcopy /Q /Y \"%%i\" \"%{outputdir}%{cfg.buildcfg}\\%{prj.name}\"",
		"xcopy \"%{resdir}Engine\\*\" \"%{outputdir}%{cfg.buildcfg}\\%{prj.name}\\Data\\Engine\" /y /i /r /e /q",
		"EXIT /B 0"
	}

	filter { "configurations:Debug" }
		defines { "DEBUG" }
		symbols "On"
		kind "ConsoleApp"

	filter { "configurations:Release" }
		defines { "NDEBUG" }
		optimize "Speed"
		flags { "LinkTimeOptimization" }
		kind "WindowedApp"

	filter { "system:windows" }
		-- forces post-build commands to trigger even if nothing changed
		fastuptodate "Off"
