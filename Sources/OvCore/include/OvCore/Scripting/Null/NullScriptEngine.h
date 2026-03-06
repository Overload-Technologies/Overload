/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include <OvCore/Scripting/Common/TScriptEngine.h>

namespace OvCore::Scripting
{
	struct NullScriptEngineContext {};
	using NullScriptEngineBase = TScriptEngine<EScriptingLanguage::NONE, NullScriptEngineContext>;
	using NullScriptEngine = NullScriptEngineBase;
}
