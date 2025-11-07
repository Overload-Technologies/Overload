project "glfw"
	kind "StaticLib"
	language "C"
	cdialect "C17"
	targetdir (outputdir .. "%{cfg.buildcfg}/%{prj.name}")
	objdir (objoutdir .. "%{cfg.buildcfg}/%{prj.name}")
	warnings "Off"

	files {
		"**.h",
		"**.c",
		"**.m",
		"**.lua"
	}

	includedirs {
		"include"
	}

	filter { "system:window" }
		defines { "_GLFW_WIN32" }

	filter { "system:linux" }
		defines { "_GLFW_X11", "_GNU_SOURCE" }
		removefiles {
			"src/win32_*",
			"src/cocoa_*",
			"src/posix_time.h",
			"src/wgl_*",
				"src/nsgl_*"
		}


	filter { "configurations:Debug" }
		defines { "DEBUG" }
		symbols "On"

	filter { "configurations:Release" }
		defines { "NDEBUG" }
		optimize "On"
