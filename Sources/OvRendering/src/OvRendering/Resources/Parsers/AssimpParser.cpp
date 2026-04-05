/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include <cmath>
#include <string>
#include <utility>
#include <vector>

#include <assimp/Importer.hpp>
#include <assimp/matrix4x4.h>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include <OvDebug/Logger.h>
#include <OvRendering/Resources/Parsers/AssimpParser.h>

namespace
{
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

	void ProcessMaterials(const aiScene* p_scene, std::vector<std::string>& p_materials)
	{
		for (uint32_t i = 0; i < p_scene->mNumMaterials; ++i)
		{
			aiMaterial* material = p_scene->mMaterials[i];
			if (material)
			{
				aiString name;
				aiGetMaterialString(material, AI_MATKEY_NAME, &name);
				p_materials.push_back(name.C_Str());
			}
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
		else
		{
			std::vector<OvRendering::Geometry::Vertex> vertices(p_mesh->mNumVertices);
			for (uint32_t i = 0; i < p_mesh->mNumVertices; ++i)
			{
				fillGeometry(vertices[i], i);
			}

			return new OvRendering::Resources::Mesh(std::span(vertices), std::span(indices), p_mesh->mMaterialIndex);
		}
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
	EModelParserFlags p_parserFlags
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

	ProcessMaterials(scene, p_materials);

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

