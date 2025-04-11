project "OvCore"
	kind "StaticLib"
	language "C++"
	cppdialect "C++20"
	files { "**.h", "**.inl", "**.cpp", "**.lua" }
	includedirs { 
		dependdir .. "glfw/include", dependdir .. "stb_image/include", dependdir .. "lua/include", dependdir .. "sol/include", dependdir .. "bullet3/include", dependdir .. "glew/include", dependdir .. "irrKlang/include", dependdir .. "ImGui/include", dependdir .. "tinyxml2/include",
		"%{wks.location}/OvAnalytics/include", "%{wks.location}/OvAudio/include", "%{wks.location}/OvDebug/include", "%{wks.location}/OvMaths/include", "%{wks.location}/OvPhysics/include",
		"%{wks.location}/OvRendering/include", "%{wks.location}/OvTools/include", "%{wks.location}/OvUI/include", "%{wks.location}/OvWindowing/include",
		"include"
	}

	targetdir (outputdir .. "%{cfg.buildcfg}/%{prj.name}")
	objdir (objoutdir .. "%{cfg.buildcfg}/%{prj.name}")
	characterset ("MBCS")
	
	buildoptions { "/bigobj" }

	filter { "configurations:Debug" }
		defines { "DEBUG" }
		symbols "On"

	filter { "configurations:Release" }
		defines { "NDEBUG" }
		optimize "On"
