# Lua API tests

`OvLuaApiTests` starts a fresh Lua state for each `*.test.lua` file, initializes it through the same `OvCore::Scripting::Lua::BindLuaApi` entry point as the engine, then executes assertions from `Lua/Support.lua`.

Add focused tests under `Tests/OvLuaApiTests/Lua/<area>/<name>.test.lua`. Tests should be independent because global state is reset before each file.
