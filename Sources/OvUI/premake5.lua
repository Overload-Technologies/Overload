project "OvUI"
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
		dependdir .. "ImGui/include",

		-- Overload SDK
		"%{wks.location}/Sources/OvMaths/include",
		"%{wks.location}/Sources/OvTools/include",

		-- Current Project
		"include"
	}

	filter { "configurations:Debug" }
		defines { "DEBUG", "_DEBUG" }
		symbols "On"

	filter { "configurations:Release" }
		defines { "NDEBUG" }
		optimize "On"
