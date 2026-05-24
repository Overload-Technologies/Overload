project "clay"
	kind "StaticLib"
	language "C"
	cdialect "C17"
	targetdir (outputdir .. "%{cfg.buildcfg}/%{prj.name}")
	objdir (objoutdir .. "%{cfg.buildcfg}/%{prj.name}")
	warnings "Off"

	files {
		"include/**.h",
		"src/**.c",
		"**.lua"
	}

	includedirs {
		"include"
	}

	filter { "configurations:Debug" }
		defines { "DEBUG", "_DEBUG" }
		runtime "Debug"
		symbols "On"

	filter { "configurations:Release or configurations:Publish" }
		defines { "NDEBUG" }
		runtime "Release"
		optimize "On"
