project "OvUI"
	kind "StaticLib"
	language "C++"
	cppdialect "C++20"
	files { "**.h", "**.inl", "**.cpp", "**.lua" }
	includedirs { "include", dependdir .. "glfw/include", dependdir .. "glew/include", dependdir .. "ImGui", "%{wks.location}/OvMaths/include", "%{wks.location}/OvTools/include", "%{wks.location}/../ImGui/" }
	targetdir (outputdir .. "%{cfg.buildcfg}/%{prj.name}")
	objdir (objoutdir .. "%{cfg.buildcfg}/%{prj.name}")
	characterset ("MBCS")

	filter { "configurations:Debug" }
		defines { "DEBUG" }
		symbols "On"

	filter { "configurations:Release" }
		defines { "NDEBUG" }
		optimize "On"