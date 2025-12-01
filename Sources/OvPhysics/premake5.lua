project "OvPhysics"
	kind "StaticLib"
	language "C++"
	cppdialect "C++20"
	targetdir (outputdir .. "%{cfg.buildcfg}/%{prj.name}")
	objdir (objoutdir .. "%{cfg.buildcfg}/%{prj.name}")
	fatalwarnings { "All" }

	files {
		"**.h",
		"**.inl",
		"**.cpp",
		"**.lua",
		"**.ini"
	}

	includedirs { 
		-- Dependencies
		dependdir .. "bullet3/",
		dependdir .. "bullet3/bullet",

		-- Overload SDK
		"%{wks.location}/Sources/OvDebug/include",
		"%{wks.location}/Sources/OvMaths/include",
		"%{wks.location}/Sources/OvTools/include",

		-- Current Project
		"include"
	}

	filter { "configurations:Debug" }
		defines { "DEBUG" }
		symbols "On"

	filter { "configurations:Release" }
		defines { "NDEBUG" }
		optimize "On"
