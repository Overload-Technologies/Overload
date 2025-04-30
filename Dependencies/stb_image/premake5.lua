project "stb_image"
	kind "SharedItems"
	files { "**.h", "**.lua" }
	objdir (objoutdir .. "%{cfg.buildcfg}/%{prj.name}")
	characterset ("MBCS")