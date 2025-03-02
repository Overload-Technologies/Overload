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

include "OvEditor"
include "OvGame"

include "../../Resources"
