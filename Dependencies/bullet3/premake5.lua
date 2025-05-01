project "bullet3"
	kind "StaticLib"
	language "C++"
	cppdialect "C++20"
	targetdir (outputdir .. "%{cfg.buildcfg}/%{prj.name}")
	objdir (objoutdir .. "%{cfg.buildcfg}/%{prj.name}")
	-- warnings "Off"

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
		"BT_USE_DOUBLE_PRECISION",
		"B3_USE_CLEW"
	}

	filter { "configurations:Debug" }
		defines { "DEBUG" }
		symbols "On"

	filter { "configurations:Release" }
		defines { "NDEBUG" }
		optimize "On"
