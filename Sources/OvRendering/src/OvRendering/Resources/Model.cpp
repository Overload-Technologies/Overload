/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include <algorithm>
#include <cmath>
#include <limits>

#include "OvRendering/Resources/Model.h"

const OvRendering::Geometry::BoundingSphere& OvRendering::Resources::Model::GetBoundingSphere() const
{
	return m_boundingSphere;
}

bool OvRendering::Resources::Model::IsSkinned() const
{
	return m_skeleton.has_value() && m_skeleton->IsValid();
}

const std::optional<OvRendering::Animation::Skeleton>& OvRendering::Resources::Model::GetSkeleton() const
{
	return m_skeleton;
}

const std::vector<OvRendering::Resources::EmbeddedMaterialData>& OvRendering::Resources::Model::GetEmbeddedMaterials() const
{
	return m_embeddedMaterials;
}

const std::vector<OvRendering::Resources::EmbeddedTextureData>& OvRendering::Resources::Model::GetEmbeddedTextures() const
{
	return m_embeddedTextures;
}

OvTools::Utils::OptRef<const OvRendering::Resources::EmbeddedMaterialData> OvRendering::Resources::Model::GetEmbeddedMaterial(uint32_t p_index) const
{
	if (p_index < m_embeddedMaterials.size())
	{
		return m_embeddedMaterials[p_index];
	}

	return std::nullopt;
}

OvTools::Utils::OptRef<const OvRendering::Resources::EmbeddedTextureData> OvRendering::Resources::Model::GetEmbeddedTexture(uint32_t p_index) const
{
	if (p_index < m_embeddedTextures.size())
	{
		return m_embeddedTextures[p_index];
	}

	return std::nullopt;
}

const std::vector<OvRendering::Animation::SkeletalAnimation>& OvRendering::Resources::Model::GetAnimations() const
{
	return m_animations;
}

const OvRendering::Animation::SkeletalAnimation* OvRendering::Resources::Model::FindAnimation(std::string_view p_name) const
{
	const auto found = std::find_if(m_animations.begin(), m_animations.end(), [p_name](const auto& p_animation)
	{
		return p_animation.name == p_name;
	});

	return found != m_animations.end() ? &(*found) : nullptr;
}

OvRendering::Resources::Model::Model(const std::string & p_path) : path(p_path)
{
}

OvRendering::Resources::Model::~Model()
{
	for (auto mesh : m_meshes)
		delete mesh;
}

void OvRendering::Resources::Model::ComputeBoundingSphere()
{
	if (m_meshes.size() == 1)
	{
		m_boundingSphere = m_meshes[0]->GetBoundingSphere();
	}
	else
	{
		m_boundingSphere.position = OvMaths::FVector3::Zero;
		m_boundingSphere.radius = 0.0f;

		if (!m_meshes.empty())
		{
			float minX = std::numeric_limits<float>::max();
			float minY = std::numeric_limits<float>::max();
			float minZ = std::numeric_limits<float>::max();

			float maxX = std::numeric_limits<float>::lowest();
			float maxY = std::numeric_limits<float>::lowest();
			float maxZ = std::numeric_limits<float>::lowest();

			for (const auto& mesh : m_meshes)
			{
				const auto& boundingSphere = mesh->GetBoundingSphere();
				minX = std::min(minX, boundingSphere.position.x - boundingSphere.radius);
				minY = std::min(minY, boundingSphere.position.y - boundingSphere.radius);
				minZ = std::min(minZ, boundingSphere.position.z - boundingSphere.radius);

				maxX = std::max(maxX, boundingSphere.position.x + boundingSphere.radius);
				maxY = std::max(maxY, boundingSphere.position.y + boundingSphere.radius);
				maxZ = std::max(maxZ, boundingSphere.position.z + boundingSphere.radius);
			}

			m_boundingSphere.position = OvMaths::FVector3{ minX + maxX, minY + maxY, minZ + maxZ } / 2.0f;
			m_boundingSphere.radius = OvMaths::FVector3::Distance(m_boundingSphere.position, { minX, minY, minZ });
		}
	}
}

const std::vector<OvRendering::Resources::Mesh*>& OvRendering::Resources::Model::GetMeshes() const
{
	return m_meshes;
}

const std::vector<std::string>& OvRendering::Resources::Model::GetMaterialNames() const
{
	return m_materialNames;
}
