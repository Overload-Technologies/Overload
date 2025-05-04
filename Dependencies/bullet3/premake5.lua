project "bullet3"
	kind "StaticLib"
	language "C++"
	cppdialect "C++20"
	targetdir (outputdir .. "%{cfg.buildcfg}/%{prj.name}")
	objdir (objoutdir .. "%{cfg.buildcfg}/%{prj.name}")
	warnings "Off"
	linkoptions { "/ignore:4006" } -- Ignore "symbol already defined" warnings

	files {
		"**.h",
		"**.cpp",
		"**.c",
		"**.lua"
	}

	includedirs {
		"bullet/"
	}

	defines {
		"B3_USE_CLEW",
		"BT_USE_SSE_IN_API",
	}

	filter { "configurations:Debug" }
		defines { "DEBUG" }
		symbols "On"

	filter { "configurations:Release" }
		defines { "NDEBUG" }
		optimize "On"
