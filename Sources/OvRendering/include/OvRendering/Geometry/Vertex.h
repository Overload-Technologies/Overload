/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

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
		float boneIDs[Animation::kMaxBonesPerVertex];
		float boneWeights[Animation::kMaxBonesPerVertex];
	};
}
