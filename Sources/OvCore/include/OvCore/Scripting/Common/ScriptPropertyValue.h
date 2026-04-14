/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include <cstdint>
#include <string>
#include <variant>

#include <OvTools/Utils/PathParser.h>

namespace OvCore::Scripting
{
	/**
	* Represents a reference to an asset (file type + path).
	* Used as a script property so the inspector can display an asset picker.
	*/
	struct AssetRef
	{
		OvTools::Utils::PathParser::EFileType fileType = OvTools::Utils::PathParser::EFileType::UNKNOWN;
		std::string path;
	};

	/**
	* Represents a reference to a scene actor by its GUID (0 = none).
	* Used as a script property so the inspector can display an actor picker.
	*/
	struct ActorRef
	{
		uint64_t guid = 0;
	};

	/**
	* Represents a primitive, asset, or actor reference script property value.
	* This type is backend-agnostic and used by Behaviour to store and expose script fields.
	*/
	using ScriptPropertyValue = std::variant<bool, double, std::string, AssetRef, ActorRef>;
}
