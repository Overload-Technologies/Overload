/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include <string>
#include <variant>

namespace OvCore::Scripting
{
	/**
	* Represents a primitive script property value (boolean, number, or string).
	* This type is backend-agnostic and used by Behaviour to store and expose script fields.
	*/
	using ScriptPropertyValue = std::variant<bool, double, std::string>;
}
