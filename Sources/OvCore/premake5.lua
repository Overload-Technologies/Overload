project "OvCore"
	kind "StaticLib"
	language "C++"
	cppdialect "C++20"
	targetdir (outputdir .. "%{cfg.buildcfg}/%{prj.name}")
	objdir (objoutdir .. "%{cfg.buildcfg}/%{prj.name}")
	fatalwarnings { "All" }

	-- If MSVC, set big obj flag
	filter { "toolset:msc" }
		buildoptions { "/bigobj" }
	filter {}

	files {
		"**.h",
		"**.inl",
		"**.cpp",
		"**.lua",
		"**.ini"
	}

	includedirs {
		-- Dependencies
		dependdir .. "glad/include",
		dependdir .. "ImGui/include",
		dependdir .. "lua/include",
		dependdir .. "sol/include",
		dependdir .. "tinyxml2/include",
		dependdir .. "tracy",

		-- Overload SDK
		"%{wks.location}/Sources/OvAudio/include",
		"%{wks.location}/Sources/OvDebug/include",
		"%{wks.location}/Sources/OvMaths/include",
		"%{wks.location}/Sources/OvPhysics/include",
		"%{wks.location}/Sources/OvRendering/include",
		"%{wks.location}/Sources/OvTools/include",
		"%{wks.location}/Sources/OvUI/include",
		"%{wks.location}/Sources/OvWindowing/include",

		-- Current project
		"include",
	}
	

	filter { "configurations:Debug" }
		defines { "DEBUG", "_DEBUG" }
		symbols "On"

	filter { "configurations:Release" }
		defines { "NDEBUG" }
		optimize "On"
