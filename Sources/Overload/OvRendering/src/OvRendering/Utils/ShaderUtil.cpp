/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include "OvRendering/Utils/ShaderUtil.h"

namespace OvRendering::Utils
{
	std::string GetShaderTypeName(Settings::EShaderType p_type)
	{
		switch (p_type)
		{
		case OvRendering::Settings::EShaderType::VERTEX: return "Vertex";
		case OvRendering::Settings::EShaderType::FRAGMENT: return "Fragment";
		}

		return "None";
	}
}
