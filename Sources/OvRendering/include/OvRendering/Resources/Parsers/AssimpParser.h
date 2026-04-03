/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include <vector>

#include "OvRendering/Geometry/Vertex.h"
#include "OvRendering/Resources/Mesh.h"
#include "OvRendering/Resources/Parsers/IModelParser.h"

namespace OvRendering::Resources::Parsers
{
	/**
	* A simple class to load assimp model data (Vertices only)
	*/
	class AssimpParser : public IModelParser
	{
	public:
		/**
		* Simply load meshes from a file using assimp
		* Return true on success
		* @param p_filename
		* @param p_meshes
		* @param p_parserFlags
		*/
		bool LoadModel
		(
			const std::string& p_fileName,
			std::vector<Mesh*>& p_meshes,
			std::vector<std::string>& p_materials,
			std::optional<Animation::Skeleton>& p_skeleton,
			std::vector<Animation::SkeletalAnimation>& p_animations,
			EModelParserFlags p_parserFlags
		) override;

	private:
		void BuildSkeleton(const struct aiScene* p_scene, Animation::Skeleton& p_skeleton);
		void ProcessAnimations(const struct aiScene* p_scene, const Animation::Skeleton& p_skeleton, std::vector<Animation::SkeletalAnimation>& p_animations);
		void ProcessMaterials(const struct aiScene* p_scene, std::vector<std::string>& p_materials);
		void ProcessNode(void* p_transform, struct aiNode* p_node, const struct aiScene* p_scene, std::vector<Mesh*>& p_meshes, Animation::Skeleton* p_skeleton);
		void ProcessMesh(void* p_transform, struct aiMesh* p_mesh, const struct aiScene* p_scene, std::vector<Geometry::Vertex>& p_outVertices, std::vector<uint32_t>& p_outIndices, Animation::Skeleton* p_skeleton);
	};
}
