project "tracy"
	kind "StaticLib"
	language "C++"
	cppdialect "C++20"
	files { "**.h", "**.hpp", "**.cpp", "**.lua" }
	targetdir (outputdir .. "%{cfg.buildcfg}/%{prj.name}")
	objdir (objoutdir .. "%{cfg.buildcfg}/%{prj.name}")
	characterset ("MBCS")
	defines { "_CRT_SECURE_NO_WARNINGS" }
	disablewarnings { "4996" } -- Ignore deprecated functions warnings
	linkoptions { "/ignore:4006" } -- Ignore "symbol already defined" warnings
	
	filter { "configurations:Debug" }
		defines { "DEBUG" }
		symbols "On"

	filter { "configurations:Release" }
		defines { "NDEBUG" }
		optimize "On"
