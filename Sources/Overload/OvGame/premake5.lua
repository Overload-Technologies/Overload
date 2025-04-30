project "OvGame"
	language "C++"
	cppdialect "C++20"
	files { "**.h", "**.inl", "**.cpp", "**.lua", "**.rc" }
	includedirs {
		dependdir .. "glfw/include", dependdir .. "stb_image/include", dependdir .. "lua/include", dependdir .. "sol/include", dependdir .. "bullet3/include", dependdir .. "glad/include", dependdir .. "soloud/include", dependdir .. "ImGui/include", dependdir .. "tinyxml2/include", dependdir .. "tracy",
		"%{wks.location}/OvAnalytics/include", "%{wks.location}/OvAudio/include", "%{wks.location}/OvCore/include",
		"%{wks.location}/OvDebug/include", "%{wks.location}/OvMaths/include", "%{wks.location}/OvPhysics/include",
		"%{wks.location}/OvRendering/include", "%{wks.location}/OvTools/include", "%{wks.location}/OvUI/include", "%{wks.location}/OvWindowing/include",
		"include"
	}

	libdirs { dependdir .. "bullet3/lib/%{cfg.buildcfg}", dependdir .. "lua/lib" }
	links {
		"Bullet3Collision.lib", "Bullet3Common.lib", "Bullet3Dynamics.lib", "Bullet3Geometry.lib", "BulletCollision.lib", "BulletDynamics.lib", "BulletSoftBody.lib", "LinearMath.lib", "opengl32.lib", "dbghelp.lib",
		"ImGui", "tinyxml2", "lua", "glad", "glfw", "assimp", "tracy", "soloud",
		"OvAudio", "OvCore", "OvDebug", "OvMaths", "OvPhysics", "OvRendering", "OvTools", "OvUI", "OvWindowing"
    }

	targetdir (outputdir .. "%{cfg.buildcfg}/%{prj.name}")
	objdir (objoutdir .. "%{cfg.buildcfg}/%{prj.name}")
	characterset ("MBCS")
	debugdir (outputdir .. "%{cfg.buildcfg}/%{prj.name}")

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
