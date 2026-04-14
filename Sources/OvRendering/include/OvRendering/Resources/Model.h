/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include <vector>
#include <unordered_map>
#include <string>
#include <optional>

#include "OvRendering/Animation/SkeletalData.h"
#include "OvRendering/Resources/EmbeddedAssets.h"
#include "OvRendering/Resources/Mesh.h"
#include "OvTools/Utils/OptRef.h"

namespace OvRendering::Resources
{
	namespace Loaders { class ModelLoader; }

	/**
	* A model is a combinaison of meshes
	*/
	class Model
	{
		friend class Loaders::ModelLoader;

	public:
		/**
		* Returns the meshes
		*/
		const std::vector<Mesh*>& GetMeshes() const;

		/**
		* Returns the material names
		*/
		const std::vector<std::string>& GetMaterialNames() const;

		/**
		* Returns the bounding sphere of the model
		*/
		const OvRendering::Geometry::BoundingSphere& GetBoundingSphere() const;

		/**
		* Returns true if this model has skinning data
		*/
		bool IsSkinned() const;

		/**
		* Returns the skeleton data (if any)
		*/
		const std::optional<Animation::Skeleton>& GetSkeleton() const;

		/**
		* Returns embedded material data extracted from the model
		*/
		const std::vector<EmbeddedMaterialData>& GetEmbeddedMaterials() const;

		/**
		* Returns embedded texture data extracted from the model
		*/
		const std::vector<EmbeddedTextureData>& GetEmbeddedTextures() const;

		/**
		* Returns an embedded material data by index
		*/
		OvTools::Utils::OptRef<const EmbeddedMaterialData> GetEmbeddedMaterial(uint32_t p_index) const;

		/**
		* Returns an embedded texture data by index
		*/
		OvTools::Utils::OptRef<const EmbeddedTextureData> GetEmbeddedTexture(uint32_t p_index) const;

		/**
		* Returns all animations available on this model
		*/
		const std::vector<Animation::SkeletalAnimation>& GetAnimations() const;

		/**
		* Finds an animation by name
		* @param p_name
		*/
		const Animation::SkeletalAnimation* FindAnimation(std::string_view p_name) const;

	private:
		Model(const std::string& p_path);
		~Model();

		void ComputeBoundingSphere();

	public:
		const std::string path;

	private:
		std::vector<Mesh*> m_meshes;
		std::vector<std::string> m_materialNames;
		std::vector<EmbeddedMaterialData> m_embeddedMaterials;
		std::vector<EmbeddedTextureData> m_embeddedTextures;
		std::optional<Animation::Skeleton> m_skeleton;
		std::vector<Animation::SkeletalAnimation> m_animations;

		Geometry::BoundingSphere m_boundingSphere;
	};
}
