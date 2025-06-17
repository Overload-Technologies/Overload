-- Function to check if Hugo is available
function isHugoAvailable()
    local handle = io.popen("hugo version 2>nul || hugo version 2>/dev/null")
    if handle then
        local result = handle:read("*a")
        handle:close()
        return result and result ~= ""
    end
    return false
end

project "Documentation"
	kind "Makefile"
    targetdir (outputdir .. "%{cfg.buildcfg}/%{prj.name}")
	objdir (objoutdir .. "%{cfg.buildcfg}/%{prj.name}")

	files { 
		"premake5.lua",
		"hugo.toml",
		"hugo-local.toml",
		"content/**.md"
	}

    if isHugoAvailable() then
        buildcommands {
            "hugo --gc --minify --quiet --cleanDestinationDir --destination=\"" .. outputdir .. "%{cfg.buildcfg}/%{prj.name}" .. "\" --config hugo-local.toml",
            "@echo Documentation generated"
        }
    else
        print("Hugo not found - skipping build commands for Documentation project")
        buildcommands {
            "@echo Hugo not available - documentation build skipped"
        }
    end
