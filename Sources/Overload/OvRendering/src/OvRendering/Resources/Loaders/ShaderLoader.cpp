/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include <algorithm>
#include <array>
#include <filesystem>
#include <format>
#include <fstream>
#include <numeric>
#include <optional>
#include <ranges>
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
	namespace Grammar
	{
		constexpr std::string_view kFeatureToken = "#feature";
		constexpr std::string_view kVertexShaderToken = "#shader vertex";
		constexpr std::string_view kFragmentShaderToken = "#shader fragment";
		constexpr std::string_view kIncludeToken = "#include";
		constexpr std::string_view kDefineToken = "#define";
		constexpr std::string_view kVersionToken = "#version";
	}

	struct ShaderInputInfo
	{
		std::string path;
		std::string name;
	};

	OvRendering::Resources::Loaders::ShaderLoader::LoggingSettings __LOGGING_SETTINGS{
		.summary = false,
		.linkingErrors = true,
		.linkingSuccess = false,
		.compilationErrors = true,
		.compilationSuccess = false,
	};

	// Load a shader, including any included files
	struct ShaderLoadResult
	{
		const ShaderInputInfo inputInfo;
		const std::string source;
	};

	// Parse the shader source code and extract the vertex and fragment shaders, as well as features
	struct ShaderParseResult
	{
		const ShaderInputInfo inputInfo;
		const std::string vertexShader;
		const std::string fragmentShader;
		const OvRendering::Resources::Shader::FeatureSet features;
	};

	// Compile the shader and create a program for each combination of features
	struct ShaderAssembleResult
	{
		const ShaderInputInfo inputInfo;
		const uint32_t failures; // How many variants failed to compile
		OvRendering::Resources::Shader::ProgramVariants variants;
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

	std::string Trim(const std::string_view p_str)
	{
		auto view =
			p_str |
			std::views::drop_while(isspace) |
			std::views::reverse |
			std::views::drop_while(isspace) |
			std::views::reverse;

		return std::string{ view.begin(), view.end() };
	}

	std::string FeatureSetToString(const OvRendering::Resources::Shader::FeatureSet& features)
	{
		if (!features.empty())
		{
			return std::format("({})", std::accumulate(
				std::next(features.begin()), features.end(), *features.begin(),
				[](const auto& a, const auto& b) {
					return std::format("{}|{}", a, b);
				}
			));
		}

		return std::string{};
	}

	std::string EnableFeaturesInShaderCode(const std::string& shaderCode, const OvRendering::Resources::Shader::FeatureSet& features)
	{
		if (features.empty())
		{
			return shaderCode;
		}

		// Create the defines string once
		std::string definesStr;
		for (const auto& feature : features)
		{
			definesStr += std::format("{} {}\n", Grammar::kDefineToken, feature);
		}

		// Find insertion point after version directive if it exists
		const size_t versionPos = shaderCode.find(Grammar::kVersionToken);
		if (versionPos == std::string::npos)
		{
			// No version directive, prepend defines
			return definesStr + shaderCode;
		}

		// Find end of version line
		const size_t endOfLine = shaderCode.find('\n', versionPos);
		if (endOfLine == std::string::npos)
		{
			// Version directive is on the last line (unusual case)
			return shaderCode + "\n" + definesStr;
		}

		// Insert defines after version line
		std::string result = shaderCode;
		result.insert(endOfLine + 1, definesStr);
		return result;
	}

	std::unique_ptr<OvRendering::HAL::ShaderProgram> CreateProgram(
		const ShaderInputInfo& p_shaderInputInfo,
		std::span<const ShaderStageDesc> p_stages,
		const OvRendering::Resources::Shader::FeatureSet& p_features,
		bool p_disableLogging = false
	)
	{
		// Process and compile all shader stages
		std::vector<ProcessedShaderStage> processedStages;
		processedStages.reserve(p_stages.size());

		bool compilationFailed = false;

		for (const auto& stageInput : p_stages)
		{
			const auto& processedStage = processedStages.emplace_back(stageInput.type);
			const auto source = EnableFeaturesInShaderCode(stageInput.source, p_features);
			processedStage.stage.Upload(source);

			if (const auto result = processedStage.stage.Compile(); !result.success)
			{
				if (!p_disableLogging && __LOGGING_SETTINGS.compilationErrors)
				{
					OVLOG_ERROR(std::format(
						"[Shader Compile] {}{}: {}",
						std::format("{}/{}",
							p_shaderInputInfo.name,
							OvRendering::Utils::GetShaderTypeName(stageInput.type)
						),
						FeatureSetToString(p_features),
						result.message
					));
				}

				compilationFailed = true;
			}
			else if (!p_disableLogging && __LOGGING_SETTINGS.compilationSuccess)
			{
				OVLOG_INFO(std::format(
					"[Shader Compile] {}{}: Compilation successful.",
					std::format("{}/{}",
						p_shaderInputInfo.name,
						OvRendering::Utils::GetShaderTypeName(stageInput.type)
					),
					FeatureSetToString(p_features)
				));
			}
		}

		if (compilationFailed)
		{
			return nullptr;
		}

		// Link the program
		auto program = std::make_unique<OvRendering::HAL::ShaderProgram>();

		// Attach all stages
		for (const auto& processedStage : processedStages)
		{
			program->Attach(processedStage.stage);
		}

		// Link and detach regardless of result
		const auto linkResult = program->Link();

		// Detach all stages
		for (const auto& processedStage : processedStages)
		{
			program->Detach(processedStage.stage);
		}

		if (linkResult.success)
		{
			if (!p_disableLogging && __LOGGING_SETTINGS.linkingSuccess)
			{
				OVLOG_INFO(std::format(
					"[Shader Linking] {}{}: {}",
					p_shaderInputInfo.name,
					FeatureSetToString(p_features),
					"Linking successful."
				));
			}

			return program;
		}
		else
		{
			if (!p_disableLogging && __LOGGING_SETTINGS.linkingErrors)
			{
				OVLOG_INFO(std::format(
					"[Shader Linking] {}{}: {}",
					p_shaderInputInfo.name,
					FeatureSetToString(p_features),
					linkResult.message
				));
			}

			return nullptr;
		}
	}

	std::unique_ptr<OvRendering::HAL::ShaderProgram> CreateDefaultProgram()
	{
		const std::string vertex = R"(
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

		auto program = CreateProgram(
			ShaderInputInfo{
				.path = "N/A",
				.name = "DefaultProgram"
			},
			shaders,
			{},
			true // Force no logging for default program (we expect it to succeed, otherwise we have a problem)
		);

		OVASSERT(program != nullptr, "Failed to create default shader program");
		return std::move(program);
	}

	std::optional<std::string> ParseIncludeDirective(const std::string_view line)
	{
		const auto start = line.find('"');
		const auto end = line.find('"', start + 1);

		if (start != std::string_view::npos && end != std::string_view::npos && end > start)
		{
			return std::make_optional<std::string>(line.substr(start + 1, end - start - 1));
		}

		return std::nullopt;
	}

	ShaderLoadResult LoadShader(const ShaderInputInfo& p_shaderInputInfo, const std::string& p_filePath, OvRendering::Resources::Loaders::ShaderLoader::FilePathParserCallback p_pathParser)
	{
		std::ifstream file(p_filePath);

		if (!file.is_open())
		{
			OVLOG_ERROR(std::format("[Shader Loading] Could not open shader file: \"{}\"", p_filePath));
			return {};
		}

		std::stringstream buffer;
		std::string line;

		while (std::getline(file, line))
		{
			const std::string trimmedLine = Trim(line);

			if (trimmedLine.starts_with(Grammar::kIncludeToken))
			{
				// If the line contains #include, process the included file
				if (const auto includeFilePath = ParseIncludeDirective(line))
				{
					// Recursively load the included file
					const std::string realIncludeFilePath = p_pathParser ? p_pathParser(includeFilePath.value()) : includeFilePath.value();
					const auto result = LoadShader(p_shaderInputInfo, realIncludeFilePath, p_pathParser);
					buffer << result.source << std::endl;
				}
				else
				{
					OVLOG_ERROR(std::format("[Shader Loading] Invalid #include directive in file: \"{}\"", p_filePath));
				}
			}
			else
			{
				// If the line does not contain #include, just append it to the buffer
				buffer << line << std::endl;
			}
		}

		return {
			.inputInfo = p_shaderInputInfo,
			.source = buffer.str()
		};
	}

	ShaderParseResult ParseShader(const ShaderLoadResult& p_shaderLoadResult)
	{
		using namespace OvRendering::Settings;

		std::istringstream stream(p_shaderLoadResult.source); // Add this line to create a stringstream from shaderCode
		std::string line;
		std::unordered_map<EShaderType, std::stringstream> shaderSources;
		OvRendering::Resources::Shader::FeatureSet features;

		auto currentType = EShaderType::NONE;

		while (std::getline(stream, line))
		{
			const std::string trimmedLine = Trim(line);

			if (trimmedLine.starts_with(Grammar::kFeatureToken))
			{
				std::string featureName;
				featureName.reserve(16); // Reserve some arbitrary space for the feature name

				for (auto& c : trimmedLine |
					std::views::drop(Grammar::kFeatureToken.size()) |
					std::views::drop_while(isspace) |
					std::views::take_while([](char c) { return !isspace(c); }))
				{
					featureName += c;
				}

				features.insert(featureName);
			}
			else if (trimmedLine.starts_with(Grammar::kVertexShaderToken))
			{
				currentType = EShaderType::VERTEX;
			}
			else if (trimmedLine.starts_with(Grammar::kFragmentShaderToken))
			{
				currentType = EShaderType::FRAGMENT;
			}
			else if (currentType != EShaderType::NONE)
			{
				shaderSources[currentType] << line << '\n';
			}
		}

		return ShaderParseResult{
			.inputInfo = p_shaderLoadResult.inputInfo,
			.vertexShader = shaderSources[EShaderType::VERTEX].str(),
			.fragmentShader = shaderSources[EShaderType::FRAGMENT].str(),
			.features = features
		};
	}

	ShaderAssembleResult AssembleShader(const ShaderParseResult& p_parseResult)
	{
		const auto variantCount = (size_t{ 1UL } << p_parseResult.features.size());

		uint32_t failures = 0;
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
				p_parseResult.inputInfo,
				stages,
				featureSet
			);

			if (program)
			{
				variants.emplace(featureSet, std::move(program));
			}
			else
			{
				++failures;
			}
		}

		// If no default program was created, we create a default one
		if (!variants.contains({}))
		{
			variants.emplace(
				OvRendering::Resources::Shader::FeatureSet{},
				std::move(CreateDefaultProgram())
			);
		}

		if (failures > 0)
		{
			if (__LOGGING_SETTINGS.summary)
			{
				OVLOG_ERROR(std::format(
					"[Shader Assembling] {}: {} variant(s) failed to compile.",
					p_parseResult.inputInfo.name,
					failures
				));
			}
		}
		else if (__LOGGING_SETTINGS.summary)
		{
			OVLOG_INFO(std::format(
				"[Shader Assembling] {}: {} variant(s) assembled.",
				p_parseResult.inputInfo.name,
				variantCount
			));
		}

		return ShaderAssembleResult{
			p_parseResult.inputInfo,
			failures,
			std::move(variants)
		};
	}

	ShaderAssembleResult CompileShaderFromFile(
		const std::string& p_filePath,
		OvRendering::Resources::Loaders::ShaderLoader::FilePathParserCallback p_pathParser
	)
	{
		const auto shaderInputInfo = ShaderInputInfo{
			.path = p_filePath,
			.name = std::filesystem::path{ p_filePath }.stem().string()
		};

		const auto shaderLoadResult = LoadShader(shaderInputInfo, p_filePath, p_pathParser);
		const auto shaderParseResult = ParseShader(shaderLoadResult);
		return AssembleShader(shaderParseResult);
	}

	ShaderAssembleResult CompileShaderFromSources(
		const std::string& p_vertexShader,
		const std::string& p_fragmentShader
	)
	{
		const ShaderParseResult shaderParseResult{
			.inputInfo = {
				.path = "N/A",
				.name = "{C++ embedded shader}"
			},
			.vertexShader = p_vertexShader,
			.fragmentShader = p_fragmentShader,
			.features = {} // No support for features in embedded shaders
		};

		return AssembleShader(shaderParseResult);
	}
}

namespace OvRendering::Resources::Loaders
{
	ShaderLoader::LoggingSettings ShaderLoader::GetLoggingSettings()
	{
		return __LOGGING_SETTINGS;
	}

	void ShaderLoader::SetLoggingSettings(LoggingSettings p_settings)
	{
		__LOGGING_SETTINGS = p_settings;
	}

	Shader* ShaderLoader::Create(const std::string& p_filePath, FilePathParserCallback p_pathParser)
	{
		auto result = CompileShaderFromFile(p_filePath, p_pathParser);
		return new Shader(p_filePath, std::move(result.variants));
	}

	Shader* ShaderLoader::CreateFromSource(const std::string& p_vertexShader, const std::string& p_fragmentShader)
	{
		auto result = CompileShaderFromSources(p_vertexShader, p_fragmentShader);
		return new Shader({}, std::move(result.variants));
	}

	void ShaderLoader::Recompile(Shader& p_shader, const std::string& p_filePath, FilePathParserCallback p_pathParser)
	{
		auto result = CompileShaderFromFile(p_filePath, p_pathParser);

		if (result.failures == 0)
		{
			p_shader.SetPrograms(std::move(result.variants));
		}
		else
		{
			OVLOG_ERROR(std::format(
				"[Shader Reload] {}: reloading failed, previous shader programs kept.",
				result.inputInfo.name
			));
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
