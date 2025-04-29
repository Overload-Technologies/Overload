project "glfw"
	kind "StaticLib"
	language "C"
	cdialect "C17"
	files { "**.h", "**.c", "**.m", "**.lua" }
	includedirs { "include" }
	targetdir (outputdir .. "%{cfg.buildcfg}/%{prj.name}")
	objdir (objoutdir .. "%{cfg.buildcfg}/%{prj.name}")
	characterset ("MBCS")
	defines { "_GLFW_WIN32" }

	filter { "configurations:Debug" }
		defines { "DEBUG" }
		symbols "On"

	filter { "configurations:Release" }
		defines { "NDEBUG" }
		optimize "On"
