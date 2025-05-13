/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/matrix4x4.h>
#include <assimp/postprocess.h>

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
}

bool OvRendering::Resources::Parsers::AssimpParser::LoadModel(const std::string & p_fileName, std::vector<Mesh*>& p_meshes, std::vector<std::string>& p_materials, EModelParserFlags p_parserFlags)
{
	Assimp::Importer import;

	// Fix the flags to avoid conflicts/invalid scenarios.
	// This is a workaround, ideally the editor UI should not allow this to happen.
	p_parserFlags = FixFlags(p_parserFlags); 

	const aiScene* scene = import.ReadFile(p_fileName, static_cast<unsigned int>(p_parserFlags));

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
		return false;

	ProcessMaterials(scene, p_materials);

	aiMatrix4x4 identity;

	ProcessNode(&identity, scene->mRootNode, scene, p_meshes);

	return true;
}

void OvRendering::Resources::Parsers::AssimpParser::ProcessMaterials(const aiScene * p_scene, std::vector<std::string>& p_materials)
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

void OvRendering::Resources::Parsers::AssimpParser::ProcessNode(void* p_transform, aiNode * p_node, const aiScene * p_scene, std::vector<Mesh*>& p_meshes)
{
	aiMatrix4x4 nodeTransformation = *reinterpret_cast<aiMatrix4x4*>(p_transform) * p_node->mTransformation;

	// Process all the node's meshes (if any)
	for (uint32_t i = 0; i < p_node->mNumMeshes; ++i)
	{
		std::vector<Geometry::Vertex> vertices;
		std::vector<uint32_t> indices;
		aiMesh* mesh = p_scene->mMeshes[p_node->mMeshes[i]];
		ProcessMesh(&nodeTransformation, mesh, p_scene, vertices, indices);
		p_meshes.push_back(new Mesh(vertices, indices, mesh->mMaterialIndex)); // The model will handle mesh destruction
	}

	// Then do the same for each of its children
	for (uint32_t i = 0; i < p_node->mNumChildren; ++i)
	{
		ProcessNode(&nodeTransformation, p_node->mChildren[i], p_scene, p_meshes);
	}
}

void OvRendering::Resources::Parsers::AssimpParser::ProcessMesh(void* p_transform, aiMesh* p_mesh, const aiScene* p_scene, std::vector<Geometry::Vertex>& p_outVertices, std::vector<uint32_t>& p_outIndices)
{
	aiMatrix4x4 meshTransformation = *reinterpret_cast<aiMatrix4x4*>(p_transform);

	for (uint32_t i = 0; i < p_mesh->mNumVertices; ++i)
	{
		const aiVector3D position = meshTransformation * p_mesh->mVertices[i];
		const aiVector3D texCoords = p_mesh->mTextureCoords[0] ? p_mesh->mTextureCoords[0][i] : aiVector3D(0.0f, 0.0f, 0.0f);
		const aiVector3D normal = meshTransformation * (p_mesh->mNormals ? p_mesh->mNormals[i] : aiVector3D(0.0f, 0.0f, 0.0f));
		const aiVector3D tangent = meshTransformation * (p_mesh->mTangents ? p_mesh->mTangents[i] : aiVector3D(0.0f, 0.0f, 0.0f));
		const aiVector3D bitangent = meshTransformation * (p_mesh->mBitangents ? p_mesh->mBitangents[i] : aiVector3D(0.0f, 0.0f, 0.0f));

		p_outVertices.push_back({
			position.x,
			position.y,
			position.z,
			texCoords.x,
			texCoords.y,
			normal.x,
			normal.y,
			normal.z,
			tangent.x,
			tangent.y,
			tangent.z,
			// Assimp calculates the tangent space vectors in a right-handed system.
			// But our shader code expects a left-handed system.
			// Multiplying the bitangent by -1 will convert it to a left-handed system.
			// Learn OpenGL also uses a left-handed tangent space for normal mapping and parallax mapping.
			-bitangent.x,
			-bitangent.y,
			-bitangent.z
		});
	}

	for (uint32_t faceID = 0; faceID < p_mesh->mNumFaces; ++faceID)
	{
		auto& face = p_mesh->mFaces[faceID];

		for (size_t indexID = 0; indexID < 3; ++indexID)
			p_outIndices.push_back(face.mIndices[indexID]);
	}
}
