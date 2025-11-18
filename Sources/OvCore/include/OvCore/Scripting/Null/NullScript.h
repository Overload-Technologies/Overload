/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include <OvCore/Scripting/Common/TScript.h>

namespace OvCore::Scripting
{
	struct NullScriptContext {};
	using NullScript = TScript<EScriptingLanguage::NONE, NullScriptContext>;
}
