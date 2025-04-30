project "sol"
	kind "SharedItems"
	files { "**.h", "**.hpp", "**.lua" }
	objdir (objoutdir .. "%{cfg.buildcfg}/%{prj.name}")
	characterset ("MBCS")