/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include <algorithm>
#include <array>
#include <cctype>
#include <cmath>
#include <cstring>
#include <cstdint>
#include <filesystem>
#include <optional>
#include <span>
#include <string>
#include <string_view>
#include <unordered_map>
#include <utility>
#include <vector>

#include <assimp/Importer.hpp>
#include <assimp/material.h>
#include <assimp/matrix4x4.h>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include <OvDebug/Logger.h>
#include <OvRendering/Resources/Parsers/AssimpParser.h>
#include <OvTools/Utils/PathParser.h>

namespace
{
	constexpr std::array kAlbedoTextureTypes{
		aiTextureType_BASE_COLOR,
		aiTextureType_DIFFUSE
	};

	constexpr std::array kNormalTextureTypes{
		aiTextureType_NORMAL_CAMERA,
		aiTextureType_NORMALS
	};

	constexpr std::array kMetallicTextureTypes{
		aiTextureType_METALNESS
	};

	constexpr std::array kRoughnessTextureTypes{
		aiTextureType_DIFFUSE_ROUGHNESS,
		aiTextureType_SHININESS
	};

	constexpr std::array kAmbientOcclusionTextureTypes{
		aiTextureType_AMBIENT_OCCLUSION,
		aiTextureType_LIGHTMAP
	};

	constexpr std::array kEmissiveTextureTypes{
		aiTextureType_EMISSION_COLOR,
		aiTextureType_EMISSIVE
	};

	constexpr std::array kHeightTextureTypes{
		aiTextureType_DISPLACEMENT,
		aiTextureType_HEIGHT
	};

	constexpr std::array kOpacityTextureTypes{
		aiTextureType_OPACITY
	};

	OvRendering::Resources::Parsers::EModelParserFlags FixFlags(OvRendering::Resources::Parsers::EModelParserFlags p_flags)
	{
		using enum OvRendering::Resources::Parsers::EModelParserFlags;

		if (static_cast<bool>(p_flags & GEN_NORMALS) && static_cast<bool>(p_flags & GEN_SMOOTH_NORMALS))
		{
			p_flags &= ~GEN_NORMALS;
			OVLOG_WARNING("AssimpParser: GEN_NORMALS and GEN_SMOOTH_NORMALS are mutually exclusive. GEN_NORMALS will be ignored.");
		}

		if (static_cast<bool>(p_flags & OPTIMIZE_GRAPH) && static_cast<bool>(p_flags & PRE_TRANSFORM_VERTICES))
		{
			p_flags &= ~OPTIMIZE_GRAPH;
			OVLOG_WARNING("AssimpParser: OPTIMIZE_GRAPH and PRE_TRANSFORM_VERTICES are mutually exclusive. OPTIMIZE_GRAPH will be ignored.");
		}

		return p_flags;
	}

	OvMaths::FMatrix4 ToMatrix4(const aiMatrix4x4& p_matrix)
	{
		return {
			p_matrix.a1, p_matrix.a2, p_matrix.a3, p_matrix.a4,
			p_matrix.b1, p_matrix.b2, p_matrix.b3, p_matrix.b4,
			p_matrix.c1, p_matrix.c2, p_matrix.c3, p_matrix.c4,
			p_matrix.d1, p_matrix.d2, p_matrix.d3, p_matrix.d4
		};
	}

	float ToPerceptualColor(const float p_linear)
	{
		constexpr float gamma = 2.2f;
		return std::pow(std::clamp(p_linear, 0.0f, 1.0f), 1.0f / gamma);
	}

	bool AddBoneData(OvRendering::Geometry::SkinnedVertex& p_vertex, uint32_t p_boneIndex, float p_weight)
	{
		if (!std::isfinite(p_weight) || p_weight <= 0.0f)
		{
			return false;
		}

		// aiProcess_LimitBoneWeights guarantees at most kMaxBonesPerVertex influences per vertex.
		for (uint8_t i = 0; i < OvRendering::Animation::kMaxBonesPerVertex; ++i)
		{
			if (p_vertex.boneWeights[i] <= 0.0f)
			{
				p_vertex.boneIDs[i] = p_boneIndex;
				p_vertex.boneWeights[i] = p_weight;
				return true;
			}
		}

		return false;
	}

	void AddSkeletonNodeRecursive(OvRendering::Animation::Skeleton& p_skeleton, aiNode* p_node, int32_t p_parentIndex)
	{
		const auto currentIndex = static_cast<uint32_t>(p_skeleton.nodes.size());

		aiVector3D aiPosition, aiScale;
		aiQuaternion aiRotation;
		p_node->mTransformation.Decompose(aiScale, aiRotation, aiPosition);

		p_skeleton.nodes.push_back({
			.name = p_node->mName.C_Str(),
			.parentIndex = p_parentIndex,
			.boneIndex = -1,
			.localBindTransform = ToMatrix4(p_node->mTransformation),
			.bindPosition = { aiPosition.x, aiPosition.y, aiPosition.z },
			.bindRotation = { aiRotation.x, aiRotation.y, aiRotation.z, aiRotation.w },
			.bindScale = { aiScale.x, aiScale.y, aiScale.z }
		});

		p_skeleton.nodeByName.emplace(p_node->mName.C_Str(), currentIndex);

		for (uint32_t i = 0; i < p_node->mNumChildren; ++i)
		{
			AddSkeletonNodeRecursive(p_skeleton, p_node->mChildren[i], static_cast<int32_t>(currentIndex));
		}
	}

	std::string SanitizeTextureExtension(std::string p_extension)
	{
		if (!p_extension.empty() && p_extension.front() == '.')
		{
			p_extension.erase(p_extension.begin());
		}

		std::transform(
			p_extension.begin(),
			p_extension.end(),
			p_extension.begin(),
			[](unsigned char p_char)
			{
				return static_cast<char>(std::tolower(p_char));
			}
		);

		std::erase_if(p_extension, [](unsigned char p_char)
		{
			return !std::isalnum(p_char);
		});

		return p_extension.empty() ? "bin" : p_extension;
	}

	std::string GetTextureExtensionFromPath(const std::string& p_path)
	{
		const auto extension = std::filesystem::path{ OvTools::Utils::PathParser::MakeNonWindowsStyle(p_path) }.extension().string();
		return SanitizeTextureExtension(extension);
	}

	std::string GetTextureExtensionFromFormatHint(const aiTexture& p_texture)
	{
		std::string hint;
		hint.reserve(sizeof(p_texture.achFormatHint));

		for (char c : p_texture.achFormatHint)
		{
			if (c == '\0')
			{
				break;
			}

			hint.push_back(c);
		}

		return SanitizeTextureExtension(hint);
	}

	std::string ResolveExternalTexturePath(
		const std::string& p_texturePath,
		const std::filesystem::path& p_modelDirectory
	)
	{
		auto resolvedPath = std::filesystem::path{ OvTools::Utils::PathParser::MakeNonWindowsStyle(p_texturePath) };

		if (resolvedPath.is_relative())
		{
			resolvedPath = p_modelDirectory / resolvedPath;
		}

		std::error_code errorCode;
		resolvedPath = std::filesystem::weakly_canonical(resolvedPath, errorCode);
		if (errorCode)
		{
			resolvedPath = resolvedPath.lexically_normal();
		}

		return resolvedPath.string();
	}

	std::optional<uint32_t> RegisterEmbeddedTexture(
		const aiScene& p_scene,
		const std::string& p_texturePath,
		const std::filesystem::path& p_modelDirectory,
		std::unordered_map<std::string, uint32_t>& p_textureIndexByKey,
		std::vector<OvRendering::Resources::EmbeddedTextureData>& p_embeddedTextures
	)
	{
		const std::string normalizedTexturePath = OvTools::Utils::PathParser::MakeNonWindowsStyle(p_texturePath);
		const aiTexture* embeddedTexture = p_scene.GetEmbeddedTexture(p_texturePath.c_str());
		std::string cacheKey = normalizedTexturePath;

		if (embeddedTexture)
		{
			cacheKey = "embedded:" + normalizedTexturePath;
		}
		else
		{
			cacheKey = ResolveExternalTexturePath(normalizedTexturePath, p_modelDirectory);
		}

		if (const auto found = p_textureIndexByKey.find(cacheKey); found != p_textureIndexByKey.end())
		{
			return found->second;
		}

		OvRendering::Resources::EmbeddedTextureData textureData;

		if (embeddedTexture)
		{
			if (embeddedTexture->mHeight == 0)
			{
				textureData.sourceType = OvRendering::Resources::EmbeddedTextureData::ESourceType::EMBEDDED_COMPRESSED;
				textureData.extension = GetTextureExtensionFromFormatHint(*embeddedTexture);

				const auto byteCount = static_cast<size_t>(embeddedTexture->mWidth);
				textureData.compressedData.resize(byteCount);
				std::memcpy(textureData.compressedData.data(), embeddedTexture->pcData, byteCount);
			}
			else
			{
				textureData.sourceType = OvRendering::Resources::EmbeddedTextureData::ESourceType::EMBEDDED_RAW_RGBA8;
				textureData.extension = "tga";
				textureData.width = embeddedTexture->mWidth;
				textureData.height = embeddedTexture->mHeight;

				const auto texelCount = static_cast<size_t>(textureData.width) * static_cast<size_t>(textureData.height);
				textureData.rawRGBAData.resize(texelCount * 4);

				for (size_t i = 0; i < texelCount; ++i)
				{
					const aiTexel texel = embeddedTexture->pcData[i];
					textureData.rawRGBAData[i * 4 + 0] = texel.r;
					textureData.rawRGBAData[i * 4 + 1] = texel.g;
					textureData.rawRGBAData[i * 4 + 2] = texel.b;
					textureData.rawRGBAData[i * 4 + 3] = texel.a;
				}
			}
		}
		else
		{
			textureData.sourceType = OvRendering::Resources::EmbeddedTextureData::ESourceType::EXTERNAL_FILE;
			textureData.sourcePath = normalizedTexturePath;
			textureData.extension = GetTextureExtensionFromPath(normalizedTexturePath);
		}

		const auto newIndex = static_cast<uint32_t>(p_embeddedTextures.size());
		p_embeddedTextures.push_back(std::move(textureData));
		p_textureIndexByKey.emplace(cacheKey, newIndex);

		return newIndex;
	}

	struct TextureSearchResult
	{
		uint32_t index;
		aiTextureType type;
	};

	std::optional<TextureSearchResult> FindFirstTexture(
		const aiScene& p_scene,
		aiMaterial& p_material,
		std::span<const aiTextureType> p_textureTypes,
		const std::filesystem::path& p_modelDirectory,
		std::unordered_map<std::string, uint32_t>& p_textureIndexByKey,
		std::vector<OvRendering::Resources::EmbeddedTextureData>& p_embeddedTextures
	)
	{
		aiString texturePath;

		for (const auto textureType : p_textureTypes)
		{
			const auto textureCount = p_material.GetTextureCount(textureType);
			for (uint32_t textureIndex = 0; textureIndex < textureCount; ++textureIndex)
			{
				if (p_material.GetTexture(textureType, textureIndex, &texturePath) != AI_SUCCESS)
				{
					continue;
				}

				if (texturePath.length == 0)
				{
					continue;
				}

				if (auto result = RegisterEmbeddedTexture(
					p_scene,
					texturePath.C_Str(),
					p_modelDirectory,
					p_textureIndexByKey,
					p_embeddedTextures
				))
				{
					return TextureSearchResult{
						.index = result.value(),
						.type = textureType
					};
				}
			}
		}

		return std::nullopt;
	}

	std::optional<uint32_t> FindFirstTextureIndex(
		const aiScene& p_scene,
		aiMaterial& p_material,
		std::span<const aiTextureType> p_textureTypes,
		const std::filesystem::path& p_modelDirectory,
		std::unordered_map<std::string, uint32_t>& p_textureIndexByKey,
		std::vector<OvRendering::Resources::EmbeddedTextureData>& p_embeddedTextures
	)
	{
		if (const auto foundTexture = FindFirstTexture(
			p_scene,
			p_material,
			p_textureTypes,
			p_modelDirectory,
			p_textureIndexByKey,
			p_embeddedTextures
		))
		{
			return foundTexture->index;
		}

		return std::nullopt;
	}

	void ProcessMaterials(
		const aiScene* p_scene,
		const std::filesystem::path& p_modelDirectory,
		std::vector<std::string>& p_materials,
		std::vector<OvRendering::Resources::EmbeddedMaterialData>& p_embeddedMaterials,
		std::vector<OvRendering::Resources::EmbeddedTextureData>& p_embeddedTextures,
		const bool p_generateEmbeddedAssets
	)
	{
		if (!p_scene)
		{
			return;
		}

		std::unordered_map<std::string, uint32_t> textureIndexByKey;
		textureIndexByKey.reserve(p_scene->mNumMaterials * 2);

		p_materials.reserve(p_scene->mNumMaterials);
		if (p_generateEmbeddedAssets)
		{
			p_embeddedMaterials.reserve(p_scene->mNumMaterials);
		}

		for (uint32_t i = 0; i < p_scene->mNumMaterials; ++i)
		{
			aiMaterial* material = p_scene->mMaterials[i];
			if (!material)
			{
				const std::string fallbackName = "Material_" + std::to_string(i);
				p_materials.push_back(fallbackName);

				if (p_generateEmbeddedAssets)
				{
					OvRendering::Resources::EmbeddedMaterialData fallbackMaterial;
					fallbackMaterial.name = fallbackName;
					p_embeddedMaterials.push_back(std::move(fallbackMaterial));
				}

				continue;
			}

			aiString materialName;
			aiGetMaterialString(material, AI_MATKEY_NAME, &materialName);

			std::string safeMaterialName = materialName.C_Str();
			if (safeMaterialName.empty())
			{
				safeMaterialName = "Material_" + std::to_string(i);
			}

			p_materials.push_back(safeMaterialName);

			if (!p_generateEmbeddedAssets)
			{
				continue;
			}

			OvRendering::Resources::EmbeddedMaterialData embeddedMaterial;
			embeddedMaterial.name = safeMaterialName;

			embeddedMaterial.albedoTexture = FindFirstTextureIndex(
				*p_scene,
				*material,
				std::span{ kAlbedoTextureTypes },
				p_modelDirectory,
				textureIndexByKey,
				p_embeddedTextures
			);

			const auto normalTexture = FindFirstTexture(
				*p_scene,
				*material,
				std::span{ kNormalTextureTypes },
				p_modelDirectory,
				textureIndexByKey,
				p_embeddedTextures
			);
			embeddedMaterial.normalTexture = normalTexture ? std::optional<uint32_t>{ normalTexture->index } : std::nullopt;

			embeddedMaterial.metallicTexture = FindFirstTextureIndex(
				*p_scene,
				*material,
				std::span{ kMetallicTextureTypes },
				p_modelDirectory,
				textureIndexByKey,
				p_embeddedTextures
			);

			embeddedMaterial.roughnessTexture = FindFirstTextureIndex(
				*p_scene,
				*material,
				std::span{ kRoughnessTextureTypes },
				p_modelDirectory,
				textureIndexByKey,
				p_embeddedTextures
			);

			embeddedMaterial.ambientOcclusionTexture = FindFirstTextureIndex(
				*p_scene,
				*material,
				std::span{ kAmbientOcclusionTextureTypes },
				p_modelDirectory,
				textureIndexByKey,
				p_embeddedTextures
			);

			embeddedMaterial.emissiveTexture = FindFirstTextureIndex(
				*p_scene,
				*material,
				std::span{ kEmissiveTextureTypes },
				p_modelDirectory,
				textureIndexByKey,
				p_embeddedTextures
			);

			const auto heightTexture = FindFirstTexture(
				*p_scene,
				*material,
				std::span{ kHeightTextureTypes },
				p_modelDirectory,
				textureIndexByKey,
				p_embeddedTextures
			);
			embeddedMaterial.heightTexture = heightTexture ? std::optional<uint32_t>{ heightTexture->index } : std::nullopt;

			embeddedMaterial.opacityTexture = FindFirstTextureIndex(
				*p_scene,
				*material,
				std::span{ kOpacityTextureTypes },
				p_modelDirectory,
				textureIndexByKey,
				p_embeddedTextures
			);

			const bool useHeightAsNormal =
				heightTexture.has_value() &&
				heightTexture->type == aiTextureType_HEIGHT &&
				(
					!normalTexture.has_value() ||
					heightTexture->index == normalTexture->index
				);

			if (useHeightAsNormal)
			{
				embeddedMaterial.normalTexture = heightTexture->index;
				embeddedMaterial.heightTexture.reset();
			}

			aiColor4D albedoColor{};
			bool hasBaseColor = false;
#if defined(AI_MATKEY_BASE_COLOR)
			hasBaseColor = material->Get(AI_MATKEY_BASE_COLOR, albedoColor) == AI_SUCCESS;
#endif

			if (hasBaseColor)
			{
				embeddedMaterial.albedo.x = ToPerceptualColor(albedoColor.r);
				embeddedMaterial.albedo.y = ToPerceptualColor(albedoColor.g);
				embeddedMaterial.albedo.z = ToPerceptualColor(albedoColor.b);
				embeddedMaterial.albedo.w = std::clamp(albedoColor.a, 0.0f, 1.0f);
			}
			else if (!embeddedMaterial.albedoTexture.has_value())
			{
				aiColor3D diffuseColor{};
				if (material->Get(AI_MATKEY_COLOR_DIFFUSE, diffuseColor) == AI_SUCCESS)
				{
					embeddedMaterial.albedo.x = ToPerceptualColor(diffuseColor.r);
					embeddedMaterial.albedo.y = ToPerceptualColor(diffuseColor.g);
					embeddedMaterial.albedo.z = ToPerceptualColor(diffuseColor.b);
				}
			}

			float opacity = 1.0f;
			if (material->Get(AI_MATKEY_OPACITY, opacity) == AI_SUCCESS)
			{
				embeddedMaterial.albedo.w = std::clamp(embeddedMaterial.albedo.w * opacity, 0.0f, 1.0f);
			}

#if defined(AI_MATKEY_METALLIC_FACTOR)
			float metallicFactor = 0.0f;
			const bool hasMetallicFactor = material->Get(AI_MATKEY_METALLIC_FACTOR, metallicFactor) == AI_SUCCESS;

			if (hasMetallicFactor)
			{
				embeddedMaterial.metallic = std::clamp(metallicFactor, 0.0f, 1.0f);
			}
			else if (embeddedMaterial.metallicTexture.has_value())
			{
				embeddedMaterial.metallic = 1.0f;
			}
#if defined(AI_MATKEY_REFLECTIVITY)
			else
			{
				float reflectivityFactor = 0.0f;
				if (material->Get(AI_MATKEY_REFLECTIVITY, reflectivityFactor) == AI_SUCCESS && reflectivityFactor > 0.0f)
				{
					// FBX can expose metallic as ReflectionFactor when no metallic data is available.
					embeddedMaterial.metallic = std::clamp(reflectivityFactor, 0.0f, 1.0f);
				}
			}
#endif
#else
			if (embeddedMaterial.metallicTexture.has_value())
			{
				embeddedMaterial.metallic = 1.0f;
			}
#endif

#if defined(AI_MATKEY_ROUGHNESS_FACTOR)
			float roughnessFactor = 0.0f;
			if (material->Get(AI_MATKEY_ROUGHNESS_FACTOR, roughnessFactor) == AI_SUCCESS)
			{
				embeddedMaterial.roughness = std::clamp(roughnessFactor, 0.0f, 1.0f);
			}
			else if (embeddedMaterial.roughnessTexture.has_value())
			{
				embeddedMaterial.roughness = 1.0f;
			}
#else
			if (embeddedMaterial.roughnessTexture.has_value())
			{
				embeddedMaterial.roughness = 1.0f;
			}
#endif

			aiColor3D emissiveColor{};
			if (material->Get(AI_MATKEY_COLOR_EMISSIVE, emissiveColor) == AI_SUCCESS)
			{
				embeddedMaterial.emissiveColor = {
					ToPerceptualColor(emissiveColor.r),
					ToPerceptualColor(emissiveColor.g),
					ToPerceptualColor(emissiveColor.b)
				};
			}

			float emissiveIntensity = 0.0f;
#if defined(AI_MATKEY_EMISSIVE_INTENSITY)
			if (material->Get(AI_MATKEY_EMISSIVE_INTENSITY, emissiveIntensity) == AI_SUCCESS)
			{
				embeddedMaterial.emissiveIntensity = std::max(emissiveIntensity, 0.0f);
			}
			else
#endif
			{
				const bool hasEmissiveColor = embeddedMaterial.emissiveColor.x > 0.0f
					|| embeddedMaterial.emissiveColor.y > 0.0f
					|| embeddedMaterial.emissiveColor.z > 0.0f;

				if (hasEmissiveColor || embeddedMaterial.emissiveTexture.has_value())
				{
					embeddedMaterial.emissiveIntensity = 1.0f;
				}
			}

			p_embeddedMaterials.push_back(std::move(embeddedMaterial));
		}
	}

	void ProcessAnimations(
		const aiScene* p_scene,
		const OvRendering::Animation::Skeleton& p_skeleton,
		std::vector<OvRendering::Animation::SkeletalAnimation>& p_animations
	)
	{
		p_animations.reserve(p_scene->mNumAnimations);

		for (uint32_t animIndex = 0; animIndex < p_scene->mNumAnimations; ++animIndex)
		{
			const auto* animation = p_scene->mAnimations[animIndex];

			OvRendering::Animation::SkeletalAnimation outAnimation{
				.name = animation->mName.length > 0 ? animation->mName.C_Str() : ("Animation_" + std::to_string(animIndex)),
				.duration = static_cast<float>(animation->mDuration),
				.ticksPerSecond = animation->mTicksPerSecond > 0.0 ? static_cast<float>(animation->mTicksPerSecond) : 25.0f
			};

			outAnimation.tracks.reserve(animation->mNumChannels);

			for (uint32_t channelIndex = 0; channelIndex < animation->mNumChannels; ++channelIndex)
			{
				const auto* channel = animation->mChannels[channelIndex];
				const std::string nodeName = channel->mNodeName.C_Str();

				if (auto foundNode = p_skeleton.FindNodeIndex(nodeName))
				{
					OvRendering::Animation::NodeAnimationTrack track;
					track.nodeIndex = foundNode.value();

					track.positionKeys.reserve(channel->mNumPositionKeys);
					for (uint32_t i = 0; i < channel->mNumPositionKeys; ++i)
					{
						const auto& key = channel->mPositionKeys[i];
						track.positionKeys.push_back({
							.time = static_cast<float>(key.mTime),
							.value = { key.mValue.x, key.mValue.y, key.mValue.z }
						});
					}

					track.rotationKeys.reserve(channel->mNumRotationKeys);
					for (uint32_t i = 0; i < channel->mNumRotationKeys; ++i)
					{
						const auto& key = channel->mRotationKeys[i];
						track.rotationKeys.push_back({
							.time = static_cast<float>(key.mTime),
							.value = { key.mValue.x, key.mValue.y, key.mValue.z, key.mValue.w }
						});
					}

					track.scaleKeys.reserve(channel->mNumScalingKeys);
					for (uint32_t i = 0; i < channel->mNumScalingKeys; ++i)
					{
						const auto& key = channel->mScalingKeys[i];
						track.scaleKeys.push_back({
							.time = static_cast<float>(key.mTime),
							.value = { key.mValue.x, key.mValue.y, key.mValue.z }
						});
					}

					const auto trackIndex = static_cast<uint32_t>(outAnimation.tracks.size());
					outAnimation.trackByNodeIndex.emplace(track.nodeIndex, trackIndex);
					outAnimation.tracks.push_back(std::move(track));
				}
			}

			p_animations.push_back(std::move(outAnimation));
		}
	}

	OvRendering::Resources::Mesh* ProcessMesh(
		const aiMatrix4x4& p_transform,
		aiMesh* p_mesh,
		OvRendering::Animation::Skeleton* p_skeleton
	)
	{
		std::vector<uint32_t> indices;
		indices.reserve(p_mesh->mNumFaces * 3);

		for (uint32_t faceID = 0; faceID < p_mesh->mNumFaces; ++faceID)
		{
			const auto& face = p_mesh->mFaces[faceID];

			if (face.mNumIndices < 3)
			{
				continue;
			}

			const auto a = face.mIndices[0];
			const auto b = face.mIndices[1];
			const auto c = face.mIndices[2];
			if (a >= p_mesh->mNumVertices || b >= p_mesh->mNumVertices || c >= p_mesh->mNumVertices)
			{
				continue;
			}

			indices.push_back(a);
			indices.push_back(b);
			indices.push_back(c);
		}

		if (p_mesh->mNumVertices == 0 || indices.empty())
		{
			return nullptr;
		}

		auto fillGeometry = [&](OvRendering::Geometry::Vertex& v, uint32_t i)
		{
			const aiVector3D position = p_transform * p_mesh->mVertices[i];
			const aiVector3D texCoords = p_mesh->mTextureCoords[0] ? p_mesh->mTextureCoords[0][i] : aiVector3D(0.0f, 0.0f, 0.0f);
			const aiVector3D normal = p_transform * (p_mesh->mNormals ? p_mesh->mNormals[i] : aiVector3D(0.0f, 0.0f, 0.0f));
			const aiVector3D tangent = p_transform * (p_mesh->mTangents ? p_mesh->mTangents[i] : aiVector3D(0.0f, 0.0f, 0.0f));
			const aiVector3D bitangent = p_transform * (p_mesh->mBitangents ? p_mesh->mBitangents[i] : aiVector3D(0.0f, 0.0f, 0.0f));

			v.position[0] = position.x;
			v.position[1] = position.y;
			v.position[2] = position.z;
			v.texCoords[0] = texCoords.x;
			v.texCoords[1] = texCoords.y;
			v.normals[0] = normal.x;
			v.normals[1] = normal.y;
			v.normals[2] = normal.z;
			v.tangent[0] = tangent.x;
			v.tangent[1] = tangent.y;
			v.tangent[2] = tangent.z;
			// Assimp calculates the tangent space vectors in a right-handed system.
			// But our shader code expects a left-handed system.
			// Multiplying the bitangent by -1 will convert it to a left-handed system.
			// Learn OpenGL also uses a left-handed tangent space for normal mapping and parallax mapping.
			v.bitangent[0] = -bitangent.x;
			v.bitangent[1] = -bitangent.y;
			v.bitangent[2] = -bitangent.z;
		};

		if (p_skeleton && p_mesh->HasBones())
		{
			std::vector<OvRendering::Geometry::SkinnedVertex> vertices(p_mesh->mNumVertices);
			for (uint32_t i = 0; i < p_mesh->mNumVertices; ++i)
			{
				fillGeometry(vertices[i], i);
			}

			for (uint32_t boneID = 0; boneID < p_mesh->mNumBones; ++boneID)
			{
				const aiBone* bone = p_mesh->mBones[boneID];
				const std::string boneName = bone->mName.C_Str();
				const auto offsetMatrix = ToMatrix4(bone->mOffsetMatrix);

				auto nodeIndex = p_skeleton->FindNodeIndex(boneName);
				if (!nodeIndex)
				{
					OVLOG_WARNING("AssimpParser: Bone '" + boneName + "' has no matching node in hierarchy and will be ignored.");
					continue;
				}

				uint32_t boneIndex = 0;

				if (auto existing = p_skeleton->FindBoneIndex(boneName))
				{
					boneIndex = existing.value();
				}
				else
				{
					boneIndex = static_cast<uint32_t>(p_skeleton->bones.size());
					p_skeleton->boneByName.emplace(boneName, boneIndex);
					p_skeleton->bones.push_back({
						.name = boneName,
						.nodeIndex = nodeIndex.value(),
						.offsetMatrix = offsetMatrix
					});
				}

				p_skeleton->nodes[nodeIndex.value()].boneIndex = static_cast<int32_t>(boneIndex);

				for (uint32_t weightID = 0; weightID < bone->mNumWeights; ++weightID)
				{
					const auto& weight = bone->mWeights[weightID];
					if (weight.mVertexId < vertices.size())
					{
						AddBoneData(vertices[weight.mVertexId], boneIndex, weight.mWeight);
					}
				}
			}

			return new OvRendering::Resources::Mesh(std::span(vertices), std::span(indices), p_mesh->mMaterialIndex);
		}

		std::vector<OvRendering::Geometry::Vertex> vertices(p_mesh->mNumVertices);
		for (uint32_t i = 0; i < p_mesh->mNumVertices; ++i)
		{
			fillGeometry(vertices[i], i);
		}

		return new OvRendering::Resources::Mesh(std::span(vertices), std::span(indices), p_mesh->mMaterialIndex);
	}

	void ProcessNode(
		const aiMatrix4x4& p_transform,
		aiNode* p_node,
		const aiScene* p_scene,
		std::vector<OvRendering::Resources::Mesh*>& p_meshes,
		OvRendering::Animation::Skeleton* p_skeleton
	)
	{
		const aiMatrix4x4 nodeTransform = p_transform * p_node->mTransformation;

		for (uint32_t i = 0; i < p_node->mNumMeshes; ++i)
		{
			if (OvRendering::Resources::Mesh* result = ProcessMesh(nodeTransform, p_scene->mMeshes[p_node->mMeshes[i]], p_skeleton))
			{
				p_meshes.push_back(result); // The model will handle mesh destruction
			}
		}

		for (uint32_t i = 0; i < p_node->mNumChildren; ++i)
		{
			ProcessNode(nodeTransform, p_node->mChildren[i], p_scene, p_meshes, p_skeleton);
		}
	}
}

bool OvRendering::Resources::Parsers::AssimpParser::LoadModel(
	const std::string& p_fileName,
	std::vector<Mesh*>& p_meshes,
	std::vector<std::string>& p_materials,
	std::optional<Animation::Skeleton>& p_skeleton,
	std::vector<Animation::SkeletalAnimation>& p_animations,
	std::vector<Resources::EmbeddedMaterialData>& p_embeddedMaterials,
	std::vector<Resources::EmbeddedTextureData>& p_embeddedTextures,
	EModelParserFlags p_parserFlags,
	bool p_generateEmbeddedAssets
)
{
	Assimp::Importer import;

	// Fix the flags to avoid conflicts/invalid scenarios.
	// This is a workaround, ideally the editor UI should not allow this to happen.
	p_parserFlags = FixFlags(p_parserFlags);

	const aiScene* scene = import.ReadFile(p_fileName, static_cast<unsigned int>(p_parserFlags));

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		return false;
	}

	p_materials.clear();
	p_embeddedMaterials.clear();
	p_embeddedTextures.clear();

	const auto modelDirectory = std::filesystem::path{ p_fileName }.parent_path();
	ProcessMaterials(
		scene,
		modelDirectory,
		p_materials,
		p_embeddedMaterials,
		p_embeddedTextures,
		p_generateEmbeddedAssets
	);

	bool hasBones = false;
	for (uint32_t i = 0; i < scene->mNumMeshes && !hasBones; ++i)
	{
		hasBones = scene->mMeshes[i] && scene->mMeshes[i]->HasBones();
	}

	p_skeleton.reset();
	p_animations.clear();

	if (hasBones)
	{
		p_skeleton.emplace();
		AddSkeletonNodeRecursive(p_skeleton.value(), scene->mRootNode, -1);
		ProcessAnimations(scene, p_skeleton.value(), p_animations);
	}

	ProcessNode(aiMatrix4x4{}, scene->mRootNode, scene, p_meshes, p_skeleton ? &p_skeleton.value() : nullptr);

	if (p_skeleton && p_skeleton->bones.empty())
	{
		p_skeleton.reset();
		p_animations.clear();
	}

	return true;
}
