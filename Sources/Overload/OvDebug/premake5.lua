project "OvDebug"
	kind "StaticLib"
	language "C++"
	cppdialect "C++20"
	targetdir (outputdir .. "%{cfg.buildcfg}/%{prj.name}")
	objdir (objoutdir .. "%{cfg.buildcfg}/%{prj.name}")

	files {
		"**.h",
		"**.inl",
		"**.cpp",
		"**.lua",
		"**.ini"
	}

	includedirs {
		-- Overload SDK
		"%{wks.location}/OvTools/include",

		-- Current Project
		"include"
	}

	filter { "configurations:Debug" }
		defines { "DEBUG" }
		symbols "On"

	filter { "configurations:Release" }
		defines { "NDEBUG" }
		optimize "On"
