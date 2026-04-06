/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include <algorithm>
#include <assert.h>
#include <cctype>
#include <filesystem>
#include <string_view>
#include <unordered_map>
#include <vector>

#include <OvTools/Utils/CodeEditor.h>
#include <OvTools/Utils/PathParser.h>
#include <OvTools/Utils/SystemCalls.h>

#ifdef _WIN32
#include <Windows.h>
#include <shellapi.h>
#else
#include <cstdlib>
#endif

namespace
{
	constexpr std::string_view kDefaultCodeEditorName = "Visual Studio Code";
	constexpr std::string_view kDefaultCodeEditorArgumentFormat = "\"{path}\"";

	struct RegisteredCodeEditor
	{
		std::string name;
		std::string command;
		std::string argumentFormat;
	};

	using RegisteredCodeEditorMap = std::unordered_map<std::string, RegisteredCodeEditor>;

	std::string NormalizeCodeEditorName(std::string p_name)
	{
		std::transform(
			p_name.begin(),
			p_name.end(),
			p_name.begin(),
			[](const unsigned char p_character)
			{
				return static_cast<char>(std::tolower(p_character));
			}
		);
		return p_name;
	}

	void ReplaceToken(std::string& p_source, const std::string& p_token, const std::string& p_value)
	{
		size_t position = p_source.find(p_token);
		while (position != std::string::npos)
		{
			p_source.replace(position, p_token.size(), p_value);
			position = p_source.find(p_token, position + p_value.size());
		}
	}

	std::string BuildEditorArguments(const std::string& p_argumentFormat, const std::filesystem::path& p_path)
	{
		std::string arguments = p_argumentFormat;

		auto preferredPath = p_path;
		preferredPath.make_preferred();

		const std::string genericPath = preferredPath.string();
		const std::string windowsPath = OvTools::Utils::PathParser::MakeWindowsStyle(genericPath);
		const std::string unixPath = OvTools::Utils::PathParser::MakeNonWindowsStyle(genericPath);

		ReplaceToken(arguments, "{path}", genericPath);
		ReplaceToken(arguments, "{path_windows}", windowsPath);
		ReplaceToken(arguments, "{path_unix}", unixPath);

		return arguments;
	}

	RegisteredCodeEditorMap& GetCodeEditors()
	{
		static RegisteredCodeEditorMap codeEditors = {
			{
				NormalizeCodeEditorName(std::string{ kDefaultCodeEditorName }),
				RegisteredCodeEditor
				{
					std::string{ kDefaultCodeEditorName },
					"code",
					std::string{ kDefaultCodeEditorArgumentFormat }
				}
			},
			{
				NormalizeCodeEditorName("Sublime Text"),
				RegisteredCodeEditor
				{
					"Sublime Text",
					"subl",
					std::string{ kDefaultCodeEditorArgumentFormat }
				}
			},
			{
				NormalizeCodeEditorName("Atom"),
				RegisteredCodeEditor
				{
					"Atom",
					"atom",
					std::string{ kDefaultCodeEditorArgumentFormat }
				}
			}
		};

		return codeEditors;
	}

	std::string& GetDefaultCodeEditorName()
	{
		static std::string defaultCodeEditorName = NormalizeCodeEditorName(std::string{ kDefaultCodeEditorName });
		return defaultCodeEditorName;
	}

#ifdef _WIN32
	bool TryShellOpen(const std::string& p_file, const std::string& p_parameters = "")
	{
		SHELLEXECUTEINFOA executeInfo{};
		executeInfo.cbSize = sizeof(executeInfo);
		executeInfo.fMask = SEE_MASK_NOCLOSEPROCESS | SEE_MASK_FLAG_NO_UI;
		executeInfo.lpVerb = "open";
		executeInfo.lpFile = p_file.c_str();
		executeInfo.lpParameters = p_parameters.empty() ? nullptr : p_parameters.c_str();
		executeInfo.nShow = SW_SHOWNORMAL;

		if (!ShellExecuteExA(&executeInfo))
		{
			return false;
		}

		if (reinterpret_cast<INT_PTR>(executeInfo.hInstApp) <= 32)
		{
			if (executeInfo.hProcess != nullptr)
			{
				CloseHandle(executeInfo.hProcess);
			}

			return false;
		}

		if (executeInfo.hProcess == nullptr)
		{
			return true;
		}

		const DWORD waitResult = WaitForSingleObject(executeInfo.hProcess, 2000);
		bool launchSucceeded = true;

		if (waitResult == WAIT_OBJECT_0)
		{
			DWORD exitCode = 0;
			if (!GetExitCodeProcess(executeInfo.hProcess, &exitCode) || exitCode != 0)
			{
				launchSucceeded = false;
			}
		}
		else if (waitResult == WAIT_FAILED)
		{
			launchSucceeded = false;
		}

		CloseHandle(executeInfo.hProcess);
		return launchSucceeded;
	}
#else
	bool IsCommandAvailable(const std::string& p_command)
	{
		const std::string checkCommand = "command -v \"" + p_command + "\" >/dev/null 2>&1";
		return std::system(checkCommand.c_str()) == 0;
	}

	bool TryOpenProgram(const std::string& p_command, const std::string& p_arguments = "")
	{
		if (!IsCommandAvailable(p_command))
		{
			return false;
		}

		std::string command = p_arguments.empty() ? p_command : p_command + " " + p_arguments;
		command += " &";
		return std::system(command.c_str()) == 0;
	}
#endif
}

void OvTools::Utils::CodeEditor::Register
(
	const std::string& p_name,
	const std::string& p_command,
	const std::string& p_argumentFormat
)
{
	assert(!p_name.empty() && "Code editor name should not be empty");
	assert(!p_command.empty() && "Code editor command should not be empty");

	GetCodeEditors()[NormalizeCodeEditorName(p_name)] = RegisteredCodeEditor{ p_name, p_command, p_argumentFormat };
}

bool OvTools::Utils::CodeEditor::Unregister(const std::string& p_name)
{
	auto& codeEditors = GetCodeEditors();
	const auto editorIterator = codeEditors.find(NormalizeCodeEditorName(p_name));
	if (editorIterator == codeEditors.end())
	{
		return false;
	}

	codeEditors.erase(editorIterator);
	return true;
}

bool OvTools::Utils::CodeEditor::Open(const std::string& p_name, const std::string& p_path)
{
	auto& codeEditors = GetCodeEditors();
	const auto editorIterator = codeEditors.find(NormalizeCodeEditorName(p_name));
	if (editorIterator == codeEditors.end())
	{
		return false;
	}

	const std::filesystem::path absolutePath = std::filesystem::absolute(p_path);
	const std::string arguments = BuildEditorArguments(editorIterator->second.argumentFormat, absolutePath);

#ifdef _WIN32
	return TryShellOpen(editorIterator->second.command, arguments);
#else
	return TryOpenProgram(editorIterator->second.command, arguments);
#endif
}

std::vector<std::string> OvTools::Utils::CodeEditor::GetRegistered()
{
	std::vector<std::string> registeredEditors;
	registeredEditors.reserve(GetCodeEditors().size());

	for (const auto& editorPair : GetCodeEditors())
	{
		registeredEditors.push_back(editorPair.second.name);
	}

	std::sort(registeredEditors.begin(), registeredEditors.end());
	return registeredEditors;
}

bool OvTools::Utils::CodeEditor::SetDefault(const std::string& p_name)
{
	const std::string normalizedName = NormalizeCodeEditorName(p_name);
	const bool editorExists = GetCodeEditors().find(normalizedName) != GetCodeEditors().end();

	if (editorExists)
	{
		GetDefaultCodeEditorName() = normalizedName;
	}

	return editorExists;
}

bool OvTools::Utils::CodeEditor::OpenDefault(const std::string& p_path)
{
	const bool openedInEditor = Open(GetDefaultCodeEditorName(), p_path);
	if (!openedInEditor)
	{
		OvTools::Utils::SystemCalls::ShowInExplorer(p_path);
	}

	return openedInEditor;
}

