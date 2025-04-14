project "soloud"
	kind "StaticLib"
	language "C++"
	cppdialect "C++20"
	files { "**.h", "**.cpp", "**.c", "**.lua" }
	includedirs { "include" }
	targetdir (outputdir .. "%{cfg.buildcfg}/%{prj.name}")
	objdir (objoutdir .. "%{cfg.buildcfg}/%{prj.name}")
	characterset ("MBCS")
	defines {"WITH_MINIAUDIO"}

	filter { "configurations:Debug" }
		defines { "DEBUG" }
		symbols "On"

	filter { "configurations:Release" }
		defines { "NDEBUG" }
		optimize "On"
