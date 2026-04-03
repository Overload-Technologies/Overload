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

		p_flags |= TRIANGULATE;
		p_flags |= LIMIT_BONE_WEIGHTS;
		p_flags &= ~PRE_TRANSFORM_VERTICES;
		p_flags &= ~DEBONE;

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

	void AddBoneData(OvRendering::Geometry::Vertex& p_vertex, uint32_t p_boneIndex, float p_weight)
	{
		if (!std::isfinite(p_weight) || p_weight <= 0.0f)
		{
			return;
		}

		for (uint8_t i = 0; i < OvRendering::Animation::kMaxBonesPerVertex; ++i)
		{
			if (p_vertex.boneWeights[i] <= 0.0f)
			{
				p_vertex.boneIDs[i] = static_cast<float>(p_boneIndex);
				p_vertex.boneWeights[i] = p_weight;
				return;
			}
		}

		auto minSlot = 0u;
		for (uint8_t i = 1; i < OvRendering::Animation::kMaxBonesPerVertex; ++i)
		{
			if (p_vertex.boneWeights[i] < p_vertex.boneWeights[minSlot])
			{
				minSlot = i;
			}
		}

		if (p_weight > p_vertex.boneWeights[minSlot])
		{
			p_vertex.boneIDs[minSlot] = static_cast<float>(p_boneIndex);
			p_vertex.boneWeights[minSlot] = p_weight;
		}
	}

	float GetBoneWeightSum(const OvRendering::Geometry::Vertex& p_vertex)
	{
		float sum = 0.0f;

		for (uint8_t i = 0; i < OvRendering::Animation::kMaxBonesPerVertex; ++i)
		{
			sum += p_vertex.boneWeights[i];
		}

		return sum;
	}

	void NormalizeBoneData(OvRendering::Geometry::Vertex& p_vertex)
	{
		const float sum = GetBoneWeightSum(p_vertex);

		if (sum > 0.0f)
		{
			for (uint8_t i = 0; i < OvRendering::Animation::kMaxBonesPerVertex; ++i)
			{
				p_vertex.boneWeights[i] /= sum;
			}
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

	const bool hasBones = [&scene]()
	{
		for (uint32_t i = 0; i < scene->mNumMeshes; ++i)
		{
			if (const auto mesh = scene->mMeshes[i]; mesh && mesh->HasBones())
			{
				return true;
			}
		}

		return false;
	}();

	p_skeleton.reset();
	p_animations.clear();

	if (hasBones)
	{
		p_skeleton.emplace();
		BuildSkeleton(scene, p_skeleton.value());
		ProcessAnimations(scene, p_skeleton.value(), p_animations);
	}

	aiMatrix4x4 identity;
	ProcessNode(&identity, scene->mRootNode, scene, p_meshes, p_skeleton ? &p_skeleton.value() : nullptr);

	if (p_skeleton && p_skeleton->bones.empty())
	{
		p_skeleton.reset();
		p_animations.clear();
	}

	return true;
}

void OvRendering::Resources::Parsers::AssimpParser::BuildSkeleton(const aiScene* p_scene, Animation::Skeleton& p_skeleton)
{
	const auto addNodeRecursive = [&p_skeleton](
		const auto& p_self,
		aiNode* p_node,
		int32_t p_parentIndex,
		const OvMaths::FMatrix4& p_parentGlobal
	) -> void
	{
		const auto localBind = ToMatrix4(p_node->mTransformation);
		const auto globalBind = p_parentGlobal * localBind;
		const auto currentIndex = static_cast<uint32_t>(p_skeleton.nodes.size());

		p_skeleton.nodes.push_back({
			.name = p_node->mName.C_Str(),
			.parentIndex = p_parentIndex,
			.boneIndex = -1,
			.localBindTransform = localBind,
			.globalBindTransform = globalBind
		});

		p_skeleton.nodeByName.emplace(p_node->mName.C_Str(), currentIndex);

		for (uint32_t i = 0; i < p_node->mNumChildren; ++i)
		{
			p_self(
				p_self,
				p_node->mChildren[i],
				static_cast<int32_t>(currentIndex),
				globalBind
			);
		}
	};

	addNodeRecursive(addNodeRecursive, p_scene->mRootNode, -1, OvMaths::FMatrix4::Identity);

	auto globalInverse = p_scene->mRootNode->mTransformation;
	globalInverse.Inverse();
	p_skeleton.globalInverseTransform = ToMatrix4(globalInverse);
}

void OvRendering::Resources::Parsers::AssimpParser::ProcessAnimations(
	const aiScene* p_scene,
	const Animation::Skeleton& p_skeleton,
	std::vector<Animation::SkeletalAnimation>& p_animations
)
{
	p_animations.reserve(p_scene->mNumAnimations);

	for (uint32_t animIndex = 0; animIndex < p_scene->mNumAnimations; ++animIndex)
	{
		const auto* animation = p_scene->mAnimations[animIndex];

		Animation::SkeletalAnimation outAnimation{
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
				Animation::NodeAnimationTrack track;
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

void OvRendering::Resources::Parsers::AssimpParser::ProcessMaterials(const aiScene* p_scene, std::vector<std::string>& p_materials)
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

void OvRendering::Resources::Parsers::AssimpParser::ProcessNode(
	void* p_transform,
	aiNode* p_node,
	const aiScene* p_scene,
	std::vector<Mesh*>& p_meshes,
	Animation::Skeleton* p_skeleton
)
{
	aiMatrix4x4 nodeTransformation = *reinterpret_cast<aiMatrix4x4*>(p_transform) * p_node->mTransformation;

	// Process all the node's meshes (if any)
	for (uint32_t i = 0; i < p_node->mNumMeshes; ++i)
	{
		std::vector<Geometry::Vertex> vertices;
		std::vector<uint32_t> indices;
		aiMesh* mesh = p_scene->mMeshes[p_node->mMeshes[i]];

		ProcessMesh(&nodeTransformation, mesh, p_scene, vertices, indices, p_skeleton);

		if (vertices.empty() || indices.empty())
		{
			continue;
		}

		p_meshes.push_back(new Mesh(vertices, indices, mesh->mMaterialIndex)); // The model will handle mesh destruction
	}

	// Then do the same for each of its children
	for (uint32_t i = 0; i < p_node->mNumChildren; ++i)
	{
		ProcessNode(&nodeTransformation, p_node->mChildren[i], p_scene, p_meshes, p_skeleton);
	}
}

void OvRendering::Resources::Parsers::AssimpParser::ProcessMesh(
	void* p_transform,
	aiMesh* p_mesh,
	const aiScene* p_scene,
	std::vector<Geometry::Vertex>& p_outVertices,
	std::vector<uint32_t>& p_outIndices,
	Animation::Skeleton* p_skeleton
)
{
	(void)p_scene;

	aiMatrix4x4 meshTransformation = *reinterpret_cast<aiMatrix4x4*>(p_transform);
	p_outVertices.reserve(p_mesh->mNumVertices);

	for (uint32_t i = 0; i < p_mesh->mNumVertices; ++i)
	{
		const aiVector3D position = meshTransformation * p_mesh->mVertices[i];
		const aiVector3D texCoords = p_mesh->mTextureCoords[0] ? p_mesh->mTextureCoords[0][i] : aiVector3D(0.0f, 0.0f, 0.0f);
		const aiVector3D normal = meshTransformation * (p_mesh->mNormals ? p_mesh->mNormals[i] : aiVector3D(0.0f, 0.0f, 0.0f));
		const aiVector3D tangent = meshTransformation * (p_mesh->mTangents ? p_mesh->mTangents[i] : aiVector3D(0.0f, 0.0f, 0.0f));
		const aiVector3D bitangent = meshTransformation * (p_mesh->mBitangents ? p_mesh->mBitangents[i] : aiVector3D(0.0f, 0.0f, 0.0f));

		Geometry::Vertex vertex{};
		vertex.position[0] = position.x;
		vertex.position[1] = position.y;
		vertex.position[2] = position.z;
		vertex.texCoords[0] = texCoords.x;
		vertex.texCoords[1] = texCoords.y;
		vertex.normals[0] = normal.x;
		vertex.normals[1] = normal.y;
		vertex.normals[2] = normal.z;
		vertex.tangent[0] = tangent.x;
		vertex.tangent[1] = tangent.y;
		vertex.tangent[2] = tangent.z;
		// Assimp calculates the tangent space vectors in a right-handed system.
		// But our shader code expects a left-handed system.
		// Multiplying the bitangent by -1 will convert it to a left-handed system.
		// Learn OpenGL also uses a left-handed tangent space for normal mapping and parallax mapping.
		vertex.bitangent[0] = -bitangent.x;
		vertex.bitangent[1] = -bitangent.y;
		vertex.bitangent[2] = -bitangent.z;

		p_outVertices.push_back(vertex);
	}

	for (uint32_t faceID = 0; faceID < p_mesh->mNumFaces; ++faceID)
	{
		auto& face = p_mesh->mFaces[faceID];

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

		p_outIndices.push_back(a);
		p_outIndices.push_back(b);
		p_outIndices.push_back(c);
	}

	if (!p_skeleton || !p_mesh->HasBones())
	{
		return;
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
			if (weight.mVertexId < p_outVertices.size())
			{
				AddBoneData(p_outVertices[weight.mVertexId], boneIndex, weight.mWeight);
			}
		}
	}

	for (auto& vertex : p_outVertices)
	{
		NormalizeBoneData(vertex);
	}
}
