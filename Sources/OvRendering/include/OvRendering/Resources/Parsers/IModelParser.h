/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include <string>

#include "OvRendering/Resources/Mesh.h"
#include "OvRendering/Resources/Parsers/EModelParserFlags.h"

namespace OvRendering::Resources::Parsers
{
	/**
	* Interface for any model parser
	*/
	class IModelParser
	{
	public:
		/**
		* Load meshes from a file
		* Return true on success
		* @param p_filename
		* @param p_meshes
		* @param p_parserFlags
		*/
		virtual bool LoadModel
		(
			const std::string& p_fileName,
			std::vector<Mesh*>& p_meshes,
			std::vector<std::string>& p_materials,
			EModelParserFlags p_parserFlags
		) = 0;
	};
}