/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include <cstdint>

#include <OvRendering/Animation/SkeletalData.h>

namespace OvRendering::Geometry
{
	/**
	* Data structure that defines the geometry of a vertex
	*/
	struct Vertex
	{
		float position[3];
		float texCoords[2];
		float normals[3];
		float tangent[3];
		float bitangent[3];
	};

	/**
	* Extends Vertex with skeletal animation data
	*/
	struct SkinnedVertex : Vertex
	{
		uint32_t boneIDs[Animation::kMaxBonesPerVertex] = {};
		float boneWeights[Animation::kMaxBonesPerVertex] = {};
	};
}
