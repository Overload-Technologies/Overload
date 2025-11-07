project "OvCore"
	kind "StaticLib"
	language "C++"
	cppdialect "C++20"
	targetdir (outputdir .. "%{cfg.buildcfg}/%{prj.name}")
	objdir (objoutdir .. "%{cfg.buildcfg}/%{prj.name}")

	-- If MSVC, set big obj flag
	filter { "toolset:msc" }
		buildoptions { "/bigobj" }

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
		"%{wks.location}/OvAudio/include",
		"%{wks.location}/OvDebug/include",
		"%{wks.location}/OvMaths/include",
		"%{wks.location}/OvPhysics/include",
		"%{wks.location}/OvRendering/include",
		"%{wks.location}/OvTools/include",
		"%{wks.location}/OvUI/include",
		"%{wks.location}/OvWindowing/include",

		-- Current project
		"include",
	}
	

	filter { "configurations:Debug" }
		defines { "DEBUG" }
		symbols "On"

	filter { "configurations:Release" }
		defines { "NDEBUG" }
		optimize "On"
