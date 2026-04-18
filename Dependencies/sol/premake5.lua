project "sol"
	kind "Utility"

	filter "action:vs*"
		kind "SharedItems"
	filter {}

	files {
		"**.h",
		"**.hpp",
		"**.lua"
	}
