project "OvTools"
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
		dependdir .. "tracy",

		-- Current Project
		"include"
	}

	filter { "system:windows" }
		characterset ("MBCS")

	filter { "configurations:Debug" }
		defines { "DEBUG", "_DEBUG" }
		symbols "On"

	filter { "configurations:Release or configurations:Publish" }
		defines { "NDEBUG" }
		optimize "On"
