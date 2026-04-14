/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <vector>

#include <OvMaths/FVector3.h>
#include <OvMaths/FVector4.h>

namespace OvRendering::Resources
{
	struct EmbeddedTextureData
	{
		enum class ESourceType
		{
			EXTERNAL_FILE,
			EMBEDDED_COMPRESSED,
			EMBEDDED_RAW_RGBA8
		};

		ESourceType sourceType = ESourceType::EXTERNAL_FILE;
		std::string sourcePath;
		std::string extension = "bin";
		std::vector<uint8_t> compressedData;
		std::vector<uint8_t> rawRGBAData;
		uint32_t width = 0;
		uint32_t height = 0;
	};

	struct EmbeddedMaterialData
	{
		std::string name;
		OvMaths::FVector4 albedo{ 1.0f, 1.0f, 1.0f, 1.0f };
		float metallic = 0.0f;
		float roughness = 0.5f;
		OvMaths::FVector3 emissiveColor{ 0.0f, 0.0f, 0.0f };
		float emissiveIntensity = 0.0f;
		std::optional<uint32_t> albedoTexture;
		std::optional<uint32_t> metallicTexture;
		std::optional<uint32_t> roughnessTexture;
		std::optional<uint32_t> ambientOcclusionTexture;
		std::optional<uint32_t> normalTexture;
		std::optional<uint32_t> heightTexture;
		std::optional<uint32_t> emissiveTexture;
		std::optional<uint32_t> opacityTexture;
	};
}

