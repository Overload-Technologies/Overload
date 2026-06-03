/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include <OvRendering/Utils/ShaderUtil.h>

namespace OvRendering::Utils
{
	std::string GetShaderTypeName(baregl::types::EShaderType p_type)
	{
		switch (p_type)
		{
			case baregl::types::EShaderType::VERTEX: return "Vertex";
			case baregl::types::EShaderType::FRAGMENT: return "Fragment";
			default: return "None";
		}
	}
}
