/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include <algorithm>
#include <cstdint>
#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <optional>
#include <string>
#include <vector>

#include <sol/sol.hpp>

#include <OvCore/Scripting/Lua/LuaBindings.h>

namespace
{
	constexpr auto kDefaultTestRoot = "Tests/OvLuaApiTests/Lua";
	constexpr auto kLuaTestSuffix = ".test.lua";
	constexpr auto kSupportFileName = "Support.lua";

	std::filesystem::path GetTestRoot(int p_argumentCount, char** p_arguments)
	{
		if (p_argumentCount > 1)
		{
			return p_arguments[1];
		}

		return kDefaultTestRoot;
	}

	bool IsLuaTestFile(const std::filesystem::path& p_path)
	{
		const std::string filename = p_path.filename().string();
		return filename.ends_with(kLuaTestSuffix);
	}

	std::vector<std::filesystem::path> CollectTestFiles(const std::filesystem::path& p_testRoot)
	{
		std::vector<std::filesystem::path> testFiles;

		for (const auto& entry : std::filesystem::recursive_directory_iterator(p_testRoot))
		{
			if (!entry.is_regular_file())
			{
				continue;
			}

			if (!IsLuaTestFile(entry.path()))
			{
				continue;
			}

			testFiles.push_back(entry.path());
		}

		std::sort(testFiles.begin(), testFiles.end(), [](const auto& p_left, const auto& p_right)
		{
			return p_left.generic_string() < p_right.generic_string();
		});

		return testFiles;
	}

	std::string GetDisplayPath(const std::filesystem::path& p_testRoot, const std::filesystem::path& p_path)
	{
		return std::filesystem::relative(p_path, p_testRoot).generic_string();
	}

	bool ExecuteLuaFile(
		sol::state& p_luaState,
		const std::filesystem::path& p_scriptPath,
		const std::string& p_displayPath,
		const std::string& p_phase
	)
	{
		const auto result = p_luaState.safe_script_file(p_scriptPath.string(), &sol::script_pass_on_error);

		if (result.valid())
		{
			return true;
		}

		const sol::error error = result;
		std::cerr
			<< '\n'
			<< "[ERROR] Lua API test execution failed" << '\n'
			<< "File: " << p_displayPath << '\n'
			<< "Phase: " << p_phase << '\n'
			<< "Details:" << '\n'
			<< error.what() << '\n';
		return false;
	}

	int HandleLuaException(lua_State* p_luaState, sol::optional<const std::exception&>, sol::string_view p_message)
	{
		lua_pushlstring(p_luaState, p_message.data(), p_message.size());
		return 1;
	}

	bool RunTestFile(
		const std::filesystem::path& p_testRoot,
		const std::filesystem::path& p_testFile,
		const std::optional<std::filesystem::path>& p_supportFile
	)
	{
		sol::state luaState;
		luaState.set_exception_handler(HandleLuaException);
		OvCore::Scripting::Lua::BindLuaApi(luaState);

		if (p_supportFile.has_value())
		{
			if (!ExecuteLuaFile(luaState, p_supportFile.value(), kSupportFileName, "loading assertions"))
			{
				return false;
			}
		}

		return ExecuteLuaFile(luaState, p_testFile, GetDisplayPath(p_testRoot, p_testFile), "running test");
	}
}

int main(int p_argumentCount, char** p_arguments)
{
	const std::filesystem::path testRoot = GetTestRoot(p_argumentCount, p_arguments);

	if (!std::filesystem::exists(testRoot) || !std::filesystem::is_directory(testRoot))
	{
		std::cerr << "Lua API test root not found: " << testRoot.generic_string() << '\n';
		return EXIT_FAILURE;
	}

	const std::vector<std::filesystem::path> testFiles = CollectTestFiles(testRoot);

	if (testFiles.empty())
	{
		std::cerr << "No Lua API tests found under: " << testRoot.generic_string() << '\n';
		return EXIT_FAILURE;
	}

	const std::filesystem::path supportFile = testRoot / kSupportFileName;
	const std::optional<std::filesystem::path> supportPath =
		std::filesystem::is_regular_file(supportFile) ?
		std::optional<std::filesystem::path>(supportFile) :
		std::nullopt;

	uint32_t failureCount = 0;

	for (const auto& testFile : testFiles)
	{
		const std::string displayPath = GetDisplayPath(testRoot, testFile);

		if (RunTestFile(testRoot, testFile, supportPath))
		{
			std::cout << "[PASS] " << displayPath << '\n';
			continue;
		}

		std::cerr << "[FAIL] " << displayPath << '\n';
		++failureCount;
	}

	if (failureCount > 0)
	{
		std::cerr << failureCount << " Lua API test(s) failed" << '\n';
		return EXIT_FAILURE;
	}

	std::cout << testFiles.size() << " Lua API test(s) passed" << '\n';
	return EXIT_SUCCESS;
}
