/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include <cstdint>
#include <charconv>
#include <optional>
#include <string>
#include <string_view>

#include <OvTools/Utils/PathParser.h>

namespace OvRendering::Resources::Parsers
{
	struct EmbeddedAssetPath
	{
		std::string modelPath;
		std::string assetName;
	};

	inline std::optional<EmbeddedAssetPath> ParseEmbeddedAssetPath(const std::string& p_path)
	{
		const auto separatorPos = p_path.rfind(':');
		if (separatorPos == std::string::npos || separatorPos == 0 || separatorPos + 1 >= p_path.size())
		{
			return std::nullopt;
		}

		EmbeddedAssetPath result{
			.modelPath = p_path.substr(0, separatorPos),
			.assetName = p_path.substr(separatorPos + 1)
		};

		if (result.modelPath.empty() || result.assetName.empty())
		{
			return std::nullopt;
		}

		if (OvTools::Utils::PathParser::GetFileType(result.modelPath) != OvTools::Utils::PathParser::EFileType::MODEL)
		{
			return std::nullopt;
		}

		return result;
	}

	inline std::string MakeEmbeddedMaterialPath(const std::string& p_modelPath, uint32_t p_materialIndex)
	{
		return p_modelPath + ":embedded_material_" + std::to_string(p_materialIndex) + ".ovmat";
	}

	inline std::string MakeEmbeddedTexturePath(const std::string& p_modelPath, uint32_t p_textureIndex, const std::string& p_extension)
	{
		return p_modelPath + ":embedded_texture_" + std::to_string(p_textureIndex) + "." + p_extension;
	}

	inline std::optional<uint32_t> ParseEmbeddedMaterialIndex(const std::string& p_assetName)
	{
		constexpr std::string_view kPrefix = "embedded_material_";
		constexpr std::string_view kSuffix = ".ovmat";

		if (!p_assetName.starts_with(kPrefix) || !p_assetName.ends_with(kSuffix))
		{
			return std::nullopt;
		}

		const auto indexPart = std::string_view{ p_assetName }.substr(
			kPrefix.size(),
			p_assetName.size() - (kPrefix.size() + kSuffix.size())
		);

		uint32_t index = 0;
		const auto parseResult = std::from_chars(indexPart.data(), indexPart.data() + indexPart.size(), index);
		if (parseResult.ec != std::errc{} || parseResult.ptr != indexPart.data() + indexPart.size())
		{
			return std::nullopt;
		}

		return index;
	}

	inline std::optional<uint32_t> ParseEmbeddedTextureIndex(const std::string& p_assetName)
	{
		constexpr std::string_view kPrefix = "embedded_texture_";

		if (!p_assetName.starts_with(kPrefix))
		{
			return std::nullopt;
		}

		const auto dotPos = p_assetName.find('.', kPrefix.size());
		if (dotPos == std::string::npos)
		{
			return std::nullopt;
		}

		const auto indexPart = std::string_view{ p_assetName }.substr(kPrefix.size(), dotPos - kPrefix.size());
		if (indexPart.empty())
		{
			return std::nullopt;
		}

		uint32_t index = 0;
		const auto parseResult = std::from_chars(indexPart.data(), indexPart.data() + indexPart.size(), index);
		if (parseResult.ec != std::errc{} || parseResult.ptr != indexPart.data() + indexPart.size())
		{
			return std::nullopt;
		}

		return index;
	}
}
