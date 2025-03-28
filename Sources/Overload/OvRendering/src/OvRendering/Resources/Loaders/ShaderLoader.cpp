/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include <algorithm>
#include <array>
#include <fstream>
#include <optional>
#include <span>
#include <sstream>

#include <OvDebug/Logger.h>
#include <OvDebug/Assertion.h>

#include <OvRendering/Resources/Loaders/ShaderLoader.h>
#include <OvRendering/Resources/Shader.h>
#include <OvRendering/HAL/ShaderProgram.h>
#include <OvRendering/HAL/ShaderStage.h>
#include <OvRendering/Utils/ShaderUtil.h>

namespace
{
	std::string __FILE_TRACE;

	struct ShaderStageDesc
	{
		const std::string source;
		const OvRendering::Settings::EShaderType type;
	};

	struct ProcessedShaderStage
	{
		const OvRendering::HAL::ShaderStage stage;
		std::optional<OvRendering::Settings::ShaderCompilationResult> compilationResult;
	};

	std::unique_ptr<OvRendering::HAL::ShaderProgram> CreateProgram(
		std::span<ShaderStageDesc> p_stages,
		bool p_verbose = false
	)
	{
		using namespace OvRendering::HAL;
		using namespace OvRendering::Resources;
		using namespace OvRendering::Settings;

		std::vector<ProcessedShaderStage> processedStages;
		processedStages.reserve(p_stages.size());

		uint32_t errorCount = 0;

		for (auto& stageInput : p_stages)
		{
			const auto& processedStage = processedStages.emplace_back(stageInput.type);
			processedStage.stage.Upload(stageInput.source);
			const auto compilationResult = processedStage.stage.Compile();
			if (!compilationResult.success)
			{
				std::string shaderTypeStr = OvRendering::Utils::GetShaderTypeName(stageInput.type);
				std::transform(shaderTypeStr.begin(), shaderTypeStr.end(), shaderTypeStr.begin(), std::toupper);
				OVLOG_ERROR("[" + shaderTypeStr + " COMPILE] \"" + __FILE_TRACE + "\": " + compilationResult.message);
				++errorCount;
			}
		}

		if (errorCount == 0)
		{
			auto program = std::make_unique<ShaderProgram>();

			for (const auto& processedStage : processedStages)
			{
				program->Attach(processedStage.stage);
			}

			const auto linkResult = program->Link();

			for (const auto& processedStage : processedStages)
			{
				program->Detach(processedStage.stage);
			}

			if (linkResult.success)
			{
				if (p_verbose)
				{
					OVLOG_INFO("[COMPILE] \"" + __FILE_TRACE + "\": Success!");
				}
				return program;
			}
			else
			{
				OVLOG_ERROR("[LINK] \"" + __FILE_TRACE + "\": Failed: " + linkResult.message);
			}
		}

		return nullptr;
	}

	std::unique_ptr<OvRendering::HAL::ShaderProgram> CreateDefaultProgram()
	{
		const std::string vertex =R"(
#version 450 core

layout(location = 0) in vec3 geo_Pos;

void main()
{
	gl_Position = vec4(geo_Pos, 1.0);
}
)";

		const std::string fragment = R"(
#version 450 core

out vec4 FragColor;

void main()
{
	FragColor = vec4(1.0, 0.0, 1.0, 1.0);
}
)";

		auto shaders = std::array<ShaderStageDesc, 2>{
			ShaderStageDesc{vertex, OvRendering::Settings::EShaderType::VERTEX},
			ShaderStageDesc{fragment, OvRendering::Settings::EShaderType::FRAGMENT}
		};

		auto program = CreateProgram(shaders);
		OVASSERT(program != nullptr, "Failed to create default shader program");
		return std::move(program);
	}

	bool ParseIncludeDirective(const std::string& line, std::string& includeFilePath)
	{
		// Find the position of the opening and closing quotes
		size_t start = line.find("\"");
		size_t end = line.find("\"", start + 1);

		// Check if both quotes are found
		if (start != std::string::npos && end != std::string::npos && end > start)
		{
			// Extract the included file path
			includeFilePath = line.substr(start + 1, end - start - 1);
			return true;
		}
		else
		{
			return false;
		}
	}

	std::string LoadShader(const std::string& p_filePath, OvRendering::Resources::Loaders::ShaderLoader::FilePathParserCallback p_pathParser)
	{
		std::ifstream file(p_filePath);

		if (!file.is_open())
		{
			OVLOG_ERROR("Error: Could not open shader file - " + p_filePath);
			return "";
		}

		std::stringstream buffer;
		std::string line;

		while (std::getline(file, line))
		{
			if (line.find("#include") != std::string::npos)
			{
				// If the line contains #include, process the included file
				std::string includeFilePath;
				if (ParseIncludeDirective(line, includeFilePath))
				{
					// Recursively load the included file
					const std::string realIncludeFilePath = p_pathParser ? p_pathParser(includeFilePath) : includeFilePath;
					std::string includedShader = LoadShader(realIncludeFilePath, p_pathParser);
					buffer << includedShader << std::endl;
				}
				else
				{
					OVLOG_ERROR("Error: Invalid #include directive in file - " + p_filePath);
				}
			}
			else {
				// If the line does not contain #include, just append it to the buffer
				buffer << line << std::endl;
			}
		}

		return buffer.str();
	}

	std::pair<std::string, std::string> ParseShader(const std::string& p_filePath, OvRendering::Resources::Loaders::ShaderLoader::FilePathParserCallback p_pathParser)
	{
		const std::string shaderCode = LoadShader(p_filePath, p_pathParser);

		std::istringstream stream(shaderCode);  // Add this line to create a stringstream from shaderCode
		std::string line;
		std::unordered_map<OvRendering::Settings::EShaderType, std::stringstream> ss;

		auto type = OvRendering::Settings::EShaderType::NONE;

		while (std::getline(stream, line))
		{
			if (line.find("#shader") != std::string::npos)
			{
				if (line.find("vertex") != std::string::npos)
					type = OvRendering::Settings::EShaderType::VERTEX;
				else if (line.find("fragment") != std::string::npos)
					type = OvRendering::Settings::EShaderType::FRAGMENT;
			}
			else if (type != OvRendering::Settings::EShaderType::NONE)
			{
				ss[type] << line << '\n';
			}
		}

		return
		{
			ss[OvRendering::Settings::EShaderType::VERTEX].str(),
			ss[OvRendering::Settings::EShaderType::FRAGMENT].str()
		};
	}
}

namespace OvRendering::Resources::Loaders
{
	Shader* ShaderLoader::Create(const std::string& p_filePath, FilePathParserCallback p_pathParser)
	{
		__FILE_TRACE = p_filePath;

		auto [vertex, fragment] = ParseShader(p_filePath, p_pathParser);

		auto shaders = std::array<ShaderStageDesc, 2>{
			ShaderStageDesc{vertex, Settings::EShaderType::VERTEX},
			ShaderStageDesc{fragment, Settings::EShaderType::FRAGMENT}
		};

		if (auto program = CreateProgram(shaders))
		{
			return new Shader(p_filePath, std::move(program));
		}

		return new Shader(p_filePath, std::move(CreateDefaultProgram()));
	}

	Shader* ShaderLoader::CreateFromSource(const std::string& p_vertexShader, const std::string& p_fragmentShader)
	{
		__FILE_TRACE = "{C++ embedded shader}";

		auto shaders = std::array<ShaderStageDesc, 2>{
			ShaderStageDesc{p_vertexShader, Settings::EShaderType::VERTEX},
			ShaderStageDesc{p_fragmentShader, Settings::EShaderType::FRAGMENT}
		};

		if (auto program = CreateProgram(shaders))
		{
			return new Shader("", std::move(program));
		}

		return new Shader("", std::move(CreateDefaultProgram()));
	}

	void ShaderLoader::Recompile(Shader& p_shader, const std::string& p_filePath, FilePathParserCallback p_pathParser)
	{
		__FILE_TRACE = p_filePath;

		auto [vertex, fragment] = ParseShader(p_filePath, p_pathParser);

		auto shaders = std::array<ShaderStageDesc, 2>{
			ShaderStageDesc{vertex, Settings::EShaderType::VERTEX},
			ShaderStageDesc{fragment, Settings::EShaderType::FRAGMENT}
		};

		if (auto program = CreateProgram(shaders, true)) // verbose = true
		{
			p_shader.SetProgram(std::move(program));
		}
		else
		{
			OVLOG_ERROR("[COMPILE] \"" + __FILE_TRACE + "\": Failed! Previous shader version keept");
		}
	}

	bool ShaderLoader::Destroy(Shader*& p_shader)
	{
		if (p_shader)
		{
			delete p_shader;
			p_shader = nullptr;
			return true;
		}

		return false;
	}
}
