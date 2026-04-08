/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include <filesystem>
#include <memory>
#include <vector>

#include <OvCore/Scripting/Common/TScriptEngine.h>

namespace OvCore::ECS::Components
{
	class Behaviour;
}

namespace sol
{
	class state;
}

namespace OvCore::Scripting
{
	/**
	* Lua script engine context
	*/
	struct LuaScriptEngineContext
	{
		std::unique_ptr<sol::state> luaState;
		std::filesystem::path scriptRootFolder;
		std::filesystem::path engineResourcesFolder;
		std::filesystem::path luarcFolder;
		std::vector<std::reference_wrapper<OvCore::ECS::Components::Behaviour>> behaviours;
		uint32_t errorCount;
	};

	using LuaScriptEngineBase = TScriptEngine<EScriptingLanguage::LUA, LuaScriptEngineContext>;

	/**
	* Lua script engine implementation
	*/
	class LuaScriptEngine : public LuaScriptEngineBase
	{
	public:
		/**
		* Constructor of the lua script engine
		* @param p_scriptsFolder
		* @param p_engineResourcesFolder
		* @param p_luarcFolder Folder where the .luarc.json will be written (defaults to p_scriptsFolder)
		*/
		LuaScriptEngine(
			const std::filesystem::path& p_scriptsFolder,
			const std::filesystem::path& p_engineResourcesFolder,
			const std::filesystem::path& p_luarcFolder = {}
		);

		/**
		* Destructor of the Lua script engine
		*/
		virtual ~LuaScriptEngine();

		/**
		* Create the Lua state
		*/
		void CreateContext();

		/**
		* Destroy the lua state
		*/
		void DestroyContext();
	};
}
