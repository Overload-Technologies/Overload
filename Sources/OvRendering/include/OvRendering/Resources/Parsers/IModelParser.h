/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include <optional>
#include <string>

#include <OvRendering/Animation/SkeletalData.h>
#include <OvRendering/Resources/Mesh.h>
#include <OvRendering/Resources/Parsers/EModelParserFlags.h>

namespace OvRendering::Resources::Parsers
{
	/**
	* Interface for any model parser
	*/
	class IModelParser
	{
	public:
		/**
		* Load meshes data from a file
		* Return true on success
		* @param p_filename
		* @param p_meshes
		* @param p_materials
		* @param p_skeleton
		* @param p_animation
		* @param p_parserFlags
		*/
		virtual bool LoadModel
		(
			const std::string& p_fileName,
			std::vector<Mesh*>& p_meshes,
			std::vector<std::string>& p_materials,
			std::optional<Animation::Skeleton>& p_skeleton,
			std::vector<Animation::SkeletalAnimation>& p_animations,
			EModelParserFlags p_parserFlags
		) = 0;
	};
}
