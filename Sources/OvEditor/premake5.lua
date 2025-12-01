project "OvEditor"
	language "C++"
	cppdialect "C++20"
	targetdir (outputdir .. "%{cfg.buildcfg}/%{prj.name}")
	objdir (objoutdir .. "%{cfg.buildcfg}/%{prj.name}")
	debugdir "%{wks.location}/Build/%{cfg.buildcfg}"
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
		-- Dependencies (order matters on Linux!)
		"bullet3",
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

		-- Dependencies that others depend on - must come after
		"assimp",
		"glfw",
    }

	filter { "configurations:Debug" }
		defines { "DEBUG" }
		symbols "On"
		kind "ConsoleApp"

	filter { "configurations:Release" }
		defines { "NDEBUG" }
		optimize "Speed"
		kind "WindowedApp"

	filter { "system:windows" }
		characterset ("MBCS")
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

			"rmdir /s /q \"%{builddir}%{cfg.buildcfg}\\Data\"",

			"xcopy \"%{resdir}Engine\\*\" \"%{builddir}%{cfg.buildcfg}\\Data\\Engine\" /y /i /r /e /q",
			"xcopy \"%{resdir}Editor\\*\" \"%{builddir}%{cfg.buildcfg}\\Data\\Editor\" /y /i /r /e /q",
			"xcopy \"%{prj.location}\\Layout.ini\" \"%{builddir}%{cfg.buildcfg}\\Config\\\" /y /i",

			"xcopy \"%{wks.location}\\Tools\\tracy-profiler.exe\" \"%{builddir}%{cfg.buildcfg}\\Tools\\\" /y /i",

			"xcopy /Y /I /Q /D \"%{outputdir}%{cfg.buildcfg}\\%{prj.name}\\*.exe\" \"%{builddir}%{cfg.buildcfg}\\\"",
			"xcopy /Y /I /Q /D \"%{outputdir}%{cfg.buildcfg}\\%{prj.name}\\*.dll\" \"%{builddir}%{cfg.buildcfg}\\\"",

			"xcopy \"%{outputdir}Debug\\OvGame\\*.exe\" \"%{builddir}%{cfg.buildcfg}\\Builder\\Development\" /y /i /c",
			"xcopy \"%{outputdir}Debug\\OvGame\\*.dll\" \"%{builddir}%{cfg.buildcfg}\\Builder\\Development\" /y /i /c",
			"xcopy \"%{outputdir}Release\\OvGame\\*.exe\" \"%{builddir}%{cfg.buildcfg}\\Builder\\Shipping\" /y /i /c",
			"xcopy \"%{outputdir}Release\\OvGame\\*.dll\" \"%{builddir}%{cfg.buildcfg}\\Builder\\Shipping\" /y /i /c",

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
			"rm -rf %{builddir}%{cfg.buildcfg}/Data",
			"mkdir -p %{builddir}%{cfg.buildcfg}/Data",
			"mkdir -p %{builddir}%{cfg.buildcfg}/Config",
			"mkdir -p %{builddir}%{cfg.buildcfg}/Builder/Development",
			"mkdir -p %{builddir}%{cfg.buildcfg}/Builder/Shipping",

			"cp -r %{resdir}Engine %{builddir}%{cfg.buildcfg}/Data/Engine",
			"cp -r %{resdir}Editor %{builddir}%{cfg.buildcfg}/Data/Editor",
			"cp %{prj.location}/layout.ini %{builddir}%{cfg.buildcfg}/Config/ 2>/dev/null || true",

			"cp %{outputdir}%{cfg.buildcfg}/%{prj.name}/OvEditor %{builddir}%{cfg.buildcfg}/",

			"cp %{outputdir}Debug/OvGame/OvGame %{builddir}%{cfg.buildcfg}/Builder/Development/ 2>/dev/null || true",
			"cp %{outputdir}Release/OvGame/OvGame %{builddir}%{cfg.buildcfg}/Builder/Shipping/ 2>/dev/null || true",

			"true"
		}
