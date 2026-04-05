/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include <vector>

#include <OvRendering/Resources/Mesh.h>
#include <OvRendering/Resources/Parsers/IModelParser.h>

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
	};
}
