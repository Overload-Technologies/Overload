project "OvTools"
	kind "StaticLib"
	language "C++"
	cppdialect "C++20"
	files { "**.h", "**.inl", "**.cpp", "**.lua" }
	includedirs {
		dependdir .. "tracy",
		"include"
	}
	targetdir (outputdir .. "%{cfg.buildcfg}/%{prj.name}")
	objdir (objoutdir .. "%{cfg.buildcfg}/%{prj.name}")
	characterset ("MBCS")

	filter { "configurations:Debug" }
		defines { "DEBUG" }
		symbols "On"

	filter { "configurations:Release" }
		defines { "NDEBUG" }
		optimize "On"
