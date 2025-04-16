/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include <algorithm>
#include <array>
#include <format>
#include <fstream>
#include <optional>
#include <span>
#include <sstream>
#include <unordered_set>

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
	bool __LOG_ERRORS = false;
	bool __LOG_SUCCESS = false;

	struct ShaderLoadResult
	{
		const std::string source;
	};

	struct ShaderParseResult
	{
		const std::string vertexShader;
		const std::string fragmentShader;
		const OvRendering::Resources::Shader::FeatureSet features;
	};

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

	std::string FeautreSetToString(const OvRendering::Resources::Shader::FeatureSet& features)
	{
		std::string result = "<features: { ";

		bool first = true;
		for (const auto& feature : features)
		{
			if (!first)
			{
				result += ", ";
			}
			result += feature;
			first = false;
		}

		result += " }>";
		return result;
	}

	std::string EnableFeaturesInShaderCode(const std::string& shaderCode, const OvRendering::Resources::Shader::FeatureSet& features)
	{
		std::string modifiedShaderCode = shaderCode;

		// Find the version directive
		size_t versionPos = modifiedShaderCode.find("#version");

		if (versionPos != std::string::npos)
		{
			// Find the end of the version line
			size_t endOfLine = modifiedShaderCode.find("\n", versionPos);

			if (endOfLine != std::string::npos)
			{
				// Position to insert defines is after the newline character
				size_t insertPos = endOfLine + 1;

				// Prepare the defines string
				std::string definesStr;
				for (const auto& feature : features)
				{
					definesStr += "#define " + feature + "\n";
				}

				// Insert defines after the version line
				modifiedShaderCode.insert(insertPos, definesStr);
			}
		}
		else
		{
			// If no version directive found, add defines at the beginning
			std::string definesStr;
			for (const auto& feature : features)
			{
				definesStr += "#define " + feature + "\n";
			}
			modifiedShaderCode = definesStr + modifiedShaderCode;
		}

		return modifiedShaderCode;
	}

	std::unique_ptr<OvRendering::HAL::ShaderProgram> CreateProgram(
		std::span<const ShaderStageDesc> p_stages,
		const OvRendering::Resources::Shader::FeatureSet& p_features = {}
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
			const auto source = EnableFeaturesInShaderCode(stageInput.source, p_features);
			processedStage.stage.Upload(source);
			const auto compilationResult = processedStage.stage.Compile();
			if (!compilationResult.success)
			{
				std::string shaderTypeStr = OvRendering::Utils::GetShaderTypeName(stageInput.type);

				for (char& c : shaderTypeStr)
				{
					c = std::toupper(c);
				}

				if (__LOG_ERRORS)
				{
					OVLOG_ERROR(std::format(
						"[{} COMPILE] \"{}\" {}: {}",
						shaderTypeStr,
						__FILE_TRACE,
						FeautreSetToString(p_features),
						compilationResult.message)
					);
				}

				++errorCount;
			}
			else
			{
				if (__LOG_SUCCESS)
				{
					OVLOG_INFO(std::format(
						"[{} COMPILE] \"{}\" {}: Success!",
						OvRendering::Utils::GetShaderTypeName(stageInput.type),
						__FILE_TRACE,
						FeautreSetToString(p_features))
					);
				}
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
				if (__LOG_SUCCESS)
				{
					OVLOG_INFO(std::format(
						"[LINK] \"{}\": Success!",
						__FILE_TRACE
					));
				}
				return program;
			}
			else if (__LOG_ERRORS)
			{
				OVLOG_ERROR(std::format(
					"[LINK] \"{}\": Failed: {}",
					__FILE_TRACE,
					linkResult.message
				));
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

		auto program = CreateProgram(shaders, {});
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

	ShaderLoadResult LoadShader(const std::string& p_filePath, OvRendering::Resources::Loaders::ShaderLoader::FilePathParserCallback p_pathParser)
	{
		std::ifstream file(p_filePath);

		if (!file.is_open())
		{
			OVLOG_ERROR(std::format("Error: Could not open shader file: \"{}\"", p_filePath));
			return {};
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
					const auto result = LoadShader(realIncludeFilePath, p_pathParser);
					buffer << result.source << std::endl;
				}
				else
				{
					OVLOG_ERROR(std::format("Error: Invalid #include directive in file: \"{}\"", p_filePath));
				}
			}
			else {
				// If the line does not contain #include, just append it to the buffer
				buffer << line << std::endl;
			}
		}

		return {
			buffer.str()
		};
	}

	ShaderParseResult ParseShader(const std::string& p_filePath, OvRendering::Resources::Loaders::ShaderLoader::FilePathParserCallback p_pathParser)
	{
		const auto shaderLoadResult = LoadShader(p_filePath, p_pathParser);

		std::istringstream stream(shaderLoadResult.source);  // Add this line to create a stringstream from shaderCode
		std::string line;
		std::unordered_map<OvRendering::Settings::EShaderType, std::stringstream> ss;
		OvRendering::Resources::Shader::FeatureSet features;

		auto type = OvRendering::Settings::EShaderType::NONE;

		while (std::getline(stream, line))
		{
			if (line.starts_with("#feature"))
			{
				const std::string featureName = line.substr(line.find(' ') + 1);
				features.insert(featureName);
			}
			else if (line.find("#shader") != std::string::npos)
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
			ss[OvRendering::Settings::EShaderType::FRAGMENT].str(),
			features
		};
	}

	OvRendering::Resources::Shader::ProgramVariants CreatePrograms(const ShaderParseResult& p_parseResult)
	{
		const auto variantCount = (size_t{ 1UL } << p_parseResult.features.size());

		OvRendering::Resources::Shader::ProgramVariants variants;

		// We create a shader program (AKA shader variant) for each combination of features.
		// The number of combinations is 2^n, where n is the number of features.
		for (size_t i = 0; i < variantCount; ++i)
		{
			OvRendering::Resources::Shader::FeatureSet featureSet;
			for (size_t j = 0; j < p_parseResult.features.size(); ++j)
			{
				if (i & (size_t{ 1UL } << j))
				{
					featureSet.insert(*std::next(p_parseResult.features.begin(), j));
				}
			}

			const auto stages = std::to_array<ShaderStageDesc>({
				{ p_parseResult.vertexShader, OvRendering::Settings::EShaderType::VERTEX },
				{ p_parseResult.fragmentShader, OvRendering::Settings::EShaderType::FRAGMENT }
			});

			auto program = CreateProgram(
				stages,
				featureSet
			);

			if (program)
			{
				variants.emplace(featureSet, std::move(program));
			}
		}

		if (variants.empty())
		{
			if (__LOG_ERRORS)
			{
				OVLOG_ERROR(std::format(
					"[COMPILE] \"{}\" Failed! Previous shader version keept",
					__FILE_TRACE
				));
			}

			auto defaultProgram = CreateDefaultProgram();
			variants.emplace(OvRendering::Resources::Shader::FeatureSet{}, std::move(defaultProgram));
		}
		else if (__LOG_SUCCESS)
		{
			OVLOG_INFO(std::format("[COMPILE] \"{}\" Compiled ({} variants)", __FILE_TRACE, variantCount));
		}

		return variants;
	}
}

namespace OvRendering::Resources::Loaders
{
	void ShaderLoader::SetLoggingSettings(bool p_logErrors, bool p_logSuccess)
	{
		__LOG_ERRORS = p_logErrors;
		__LOG_SUCCESS = p_logSuccess;
	}

	Shader* ShaderLoader::Create(const std::string& p_filePath, FilePathParserCallback p_pathParser)
	{
		__FILE_TRACE = p_filePath;

		const auto shaderParseResult = ParseShader(p_filePath, p_pathParser);

		return new Shader(
			p_filePath,
			shaderParseResult.features,
			std::move(CreatePrograms(shaderParseResult))
		);
	}

	Shader* ShaderLoader::CreateFromSource(const std::string& p_vertexShader, const std::string& p_fragmentShader)
	{
		__FILE_TRACE = "{C++ embedded shader}";

		auto shaders = std::array<ShaderStageDesc, 2>{
			ShaderStageDesc{p_vertexShader, Settings::EShaderType::VERTEX},
			ShaderStageDesc{p_fragmentShader, Settings::EShaderType::FRAGMENT}
		};

		const ShaderParseResult shaderParseResult{
			.vertexShader = p_vertexShader,
			.fragmentShader = p_fragmentShader,
			.features = {}
		};

		return new Shader(
			"",
			shaderParseResult.features,
			std::move(CreatePrograms(shaderParseResult))
		);
	}

	void ShaderLoader::Recompile(Shader& p_shader, const std::string& p_filePath, FilePathParserCallback p_pathParser)
	{
		__FILE_TRACE = p_filePath;

		const auto shaderParseResult = ParseShader(p_filePath, p_pathParser);

		p_shader.SetPrograms(
			shaderParseResult.features,
			std::move(CreatePrograms(shaderParseResult))
		);
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
