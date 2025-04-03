project "tinyxml2"
	kind "StaticLib"
	language "C++"
	cppdialect "C++20"
	files { "**.h", "**.cpp", "**.lua" }
	targetdir (outputdir .. "%{cfg.buildcfg}/%{prj.name}")
	objdir (objoutdir .. "%{cfg.buildcfg}/%{prj.name}")
	characterset ("MBCS")

	filter { "configurations:Debug" }
		defines { "DEBUG" }
		symbols "On"

	filter { "configurations:Release" }
		defines { "NDEBUG" }
		optimize "On"