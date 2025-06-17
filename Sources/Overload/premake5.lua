-- Function to read the version number from VERSION.txt
local function readVersion()
	local versionFile = io.open("../../VERSION.txt", "r")
	if versionFile then
		local version = versionFile:read("*l")
		versionFile:close()
		return version
	else
		error("Could not open VERSION.txt")
	end
end

local version = readVersion()

workspace "Overload"
	configurations { "Debug", "Release" }
	platforms { "x64" }
	startproject "OvEditor"
	defines {
		"LUA_SCRIPTING",
		"GRAPHICS_API_OPENGL",
		"OVERLOAD_VERSION=\"" .. version .. "\"",
		"TRACY_ENABLE",
		"TRACY_ON_DEMAND",
		"TRACY_MEMORY_ENABLE"
	}

	-- Disable "Hot Reload": Doesn't work with Tracy.
	editandcontinue "Off"

outputdir = "%{wks.location}/../../Bin/"
objoutdir = "%{wks.location}/../../Bin-Int/"
dependdir = "%{wks.location}/../../Dependencies/"
builddir = "%{wks.location}/../../Build/"
resdir = "%{wks.location}/../../Resources/"

group "Dependencies"
	include "../../Dependencies/ImGui"
	include "../../Dependencies/tinyxml2"
	include "../../Dependencies/tracy"
	include "../../Dependencies/lua"
	include "../../Dependencies/glad"
	include "../../Dependencies/soloud"
	include "../../Dependencies/assimp"
	include "../../Dependencies/glfw"
	include "../../Dependencies/bullet3"
group ""

group "Dependencies/Prebuilt"
	include "../../Dependencies/stb_image"
	include "../../Dependencies/sol"
group ""

group "Overload SDK"
	include "OvAudio"
	include "OvCore"
	include "OvDebug"
	include "OvMaths"
	include "OvPhysics"
	include "OvRendering"
	include "OvTools"
	include "OvUI"
	include "OvWindowing"
group ""

group "Overload Apps"
	include "OvEditor"
	include "OvGame"
group ""

include "../../Resources"
include "../../Documentation"
