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
#include "OvRendering/Resources/Mesh.h"

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
		std::optional<Animation::Skeleton> m_skeleton;
		std::vector<Animation::SkeletalAnimation> m_animations;

		Geometry::BoundingSphere m_boundingSphere;
	};
}
