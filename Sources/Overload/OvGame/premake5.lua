project "OvGame"
	language "C++"
	cppdialect "C++20"
	files { "**.h", "**.inl","**.cpp", "**.lua", "**.rc" }
	includedirs { "include", dependdir .. "glfw/include", dependdir .. "stb_image/include", dependdir .. "lua/include", dependdir .. "bullet3/include", dependdir .. "glew/include", dependdir .. "irrklang/include",  dependdir .. "ImGui", dependdir .. "tinyxml2",
	"%{wks.location}/OvAnalytics/include", "%{wks.location}/OvAudio/include", "%{wks.location}/OvCore/include",
	"%{wks.location}/OvDebug/include", "%{wks.location}/OvMaths/include", "%{wks.location}/OvPhysics/include",
	"%{wks.location}/OvRendering/include", "%{wks.location}/OvTools/include", "%{wks.location}/OvUI/include", "%{wks.location}/OvWindowing/include" }

	libdirs { dependdir .. "glfw/lib", dependdir .. "bullet3/lib/%{cfg.buildcfg}", dependdir .. "lua/lib", dependdir .. "glew/lib", dependdir .. "irrklang/lib", dependdir .. "assimp/lib" }
	links { "assimp-vc143-mt", "Bullet3Collision", "Bullet3Common", "Bullet3Dynamics", "Bullet3Geometry", "BulletCollision", "BulletDynamics", "BulletSoftBody", "LinearMath", "glew32", "glfw3dll", "irrKlang", "liblua53", 
	"opengl32", "OvAnalytics", "OvAudio", "OvCore", "OvDebug", "OvMaths", "OvPhysics", "OvRendering", "OvTools", "OvUI", "OvWindowing", "ImGui", "tinyxml2" }

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
		optimize "On"
		kind "WindowedApp"

	filter { "system:windows" }
		-- forces post-build commands to trigger even if nothing changed
		fastuptodate "Off"