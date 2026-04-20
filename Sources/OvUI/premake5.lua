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
		"%{wks.location}/Sources/OvDebug/include",
		"%{wks.location}/Sources/OvWindowing/include",

		-- Current Project
		"include"
	}

	filter { "configurations:Debug" }
		defines { "DEBUG", "_DEBUG" }
		symbols "On"

	filter { "configurations:Release or configurations:Publish" }
		defines { "NDEBUG" }
		optimize "On"
