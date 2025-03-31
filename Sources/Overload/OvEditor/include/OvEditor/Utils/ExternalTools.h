/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include <string>
#include <unordered_map>
#include <array>

namespace OvEditor::Utils
{
	struct ExternalTool
	{
		const std::string_view name;
		const std::string_view command;
	};

	constexpr auto ExternalTools = std::to_array<ExternalTool>({
		ExternalTool{ "Visual Studio Code", "code {}" },
		ExternalTool{ "Sublime Text", "subl {}" },
		ExternalTool{ "Atom", "atom {}" },
	});
}
