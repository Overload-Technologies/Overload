project "stb_image"
	kind "Utility"

	filter "action:vs*"
		kind "SharedItems"
	filter {}

	files {
		"**.h",
		"**.lua"
	}
