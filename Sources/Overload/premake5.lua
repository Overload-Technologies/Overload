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
	defines { "LUA_SCRIPTING", "GRAPHICS_API_OPENGL", "OVERLOAD_VERSION=\"" .. version .. "\"" }

outputdir = "%{wks.location}/../../Bin/"
objoutdir = "%{wks.location}/../../Bin-Int/"
dependdir = "%{wks.location}/../../Dependencies/"
builddir = "%{wks.location}/../../Build/"
resdir = "%{wks.location}/../../Resources/"

group "Dependencies"
	include "../../Dependencies/ImGui"
	include "../../Dependencies/tinyxml2"
	include "../../Dependencies/lua"
	include "../../Dependencies/soloud"
group ""

group "Dependencies/Prebuilt"
	include "../../Dependencies/assimp"
	include "../../Dependencies/bullet3"
	include "../../Dependencies/glew"
	include "../../Dependencies/glfw"
	include "../../Dependencies/stb_image"
	include "../../Dependencies/sol"
group ""

group "Overload SDK"
	include "OvAnalytics"
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
