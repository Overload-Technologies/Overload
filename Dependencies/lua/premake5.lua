project "lua"
	kind "StaticLib"
	language "C"
	cdialect "C17"
	files { "**.h", "**.c", "**.lua" }
	includedirs { "include" }
	targetdir (outputdir .. "%{cfg.buildcfg}/%{prj.name}")
	objdir (objoutdir .. "%{cfg.buildcfg}/%{prj.name}")

	filter { "configurations:Debug" }
		defines { "DEBUG" }
		optimize "On"
		symbols "Off"
		runtime "Debug" -- Ensures /MDd is used

	filter { "configurations:Release" }
		defines { "NDEBUG" }
		optimize "Speed"