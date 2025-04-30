project "OvEditor"
	language "C++"
	cppdialect "C++20"
	targetdir (outputdir .. "%{cfg.buildcfg}/%{prj.name}")
	objdir (objoutdir .. "%{cfg.buildcfg}/%{prj.name}")
	debugdir "%{wks.location}/../../Build/%{cfg.buildcfg}"
	
	files {
		"**.h",
		"**.inl",
		"**.cpp",
		"**.lua",
		"**.ini",
		"**.rc",
	}

	includedirs {
		-- Dependencies
		dependdir .. "bullet3/include",
		dependdir .. "glad/include",
		dependdir .. "ImGui/include",
		dependdir .. "lua/include",
		dependdir .. "sol/include",
		dependdir .. "tinyxml2/include",
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

	libdirs {
		dependdir .. "bullet3/lib/%{cfg.buildcfg}",
		dependdir .. "lua/lib"
	}

	links {
		-- Precompiled Libraries
		"dbghelp.lib",
		"opengl32.lib",
		"Bullet3Collision.lib",
		"Bullet3Common.lib",
		"Bullet3Dynamics.lib",
		"Bullet3Geometry.lib",
		"BulletCollision.lib",
		"BulletDynamics.lib",
		"BulletSoftBody.lib",
		"LinearMath.lib",

		-- Dependencies
		"assimp",
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

		"rmdir /s /q \"%{builddir}%{cfg.buildcfg}\\Data\"",

		"xcopy \"%{resdir}Engine\\*\" \"%{builddir}%{cfg.buildcfg}\\Data\\Engine\" /y /i /r /e /q",
		"xcopy \"%{resdir}Editor\\*\" \"%{builddir}%{cfg.buildcfg}\\Data\\Editor\" /y /i /r /e /q",
		"xcopy \"%{prj.location}\\Layout.ini\" \"%{builddir}%{cfg.buildcfg}\\Config\\\" /y /i",

		"xcopy \"%{wks.location}\\..\\..\\Tools\\tracy-profiler.exe\" \"%{builddir}%{cfg.buildcfg}\\Tools\\\" /y /i",

		"xcopy /Y /I /Q /D \"%{outputdir}%{cfg.buildcfg}\\%{prj.name}\\*.exe\" \"%{builddir}%{cfg.buildcfg}\\\"",
		"xcopy /Y /I /Q /D \"%{outputdir}%{cfg.buildcfg}\\%{prj.name}\\*.dll\" \"%{builddir}%{cfg.buildcfg}\\\"",

		"xcopy \"%{outputdir}Debug\\OvGame\\*.exe\" \"%{builddir}%{cfg.buildcfg}\\Builder\\Development\" /y /i /c",
		"xcopy \"%{outputdir}Debug\\OvGame\\*.dll\" \"%{builddir}%{cfg.buildcfg}\\Builder\\Development\" /y /i /c",
		"xcopy \"%{outputdir}Release\\OvGame\\*.exe\" \"%{builddir}%{cfg.buildcfg}\\Builder\\Shipping\" /y /i /c",
		"xcopy \"%{outputdir}Release\\OvGame\\*.dll\" \"%{builddir}%{cfg.buildcfg}\\Builder\\Shipping\" /y /i /c",

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
		characterset ("MBCS")
		-- forces post-build commands to trigger even if nothing changed
		fastuptodate "Off"
