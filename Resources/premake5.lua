project "Resources"
	kind "Utility"

	filter "action:vs*"
		kind "SharedItems"
	filter {}

	files { "Editor/**", "Engine/**", "**.lua" }
