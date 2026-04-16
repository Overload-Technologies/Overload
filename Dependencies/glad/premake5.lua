project "glad"
	kind "StaticLib"
	language "C"
	cdialect "C17"
	targetdir (outputdir .. "%{cfg.buildcfg}/%{prj.name}")
	objdir (objoutdir .. "%{cfg.buildcfg}/%{prj.name}")
	warnings "Off"

	files {
		"**.h",
		"**.c",
		"**.lua"
	}

	includedirs {
		"include"
	}

	filter { "configurations:Debug" }
		defines { "DEBUG", "_DEBUG" }
		optimize "On"

	filter { "configurations:Release or configurations:Publish" }
		defines { "NDEBUG" }
		optimize "On"
