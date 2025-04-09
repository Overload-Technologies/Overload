project "sol"
	kind "SharedItems"
	files { "**.hpp", "**.h", "**.lua" }
	objdir (objoutdir .. "%{cfg.buildcfg}/%{prj.name}")
	characterset ("MBCS")