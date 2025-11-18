/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include <OvRendering/HAL/None/NoneShaderStage.h>

template<>
OvRendering::HAL::NoneShaderStage::TShaderStage(Settings::EShaderType p_type)
{
}

template<>
OvRendering::HAL::NoneShaderStage::~TShaderStage()
{
}

template<>
void OvRendering::HAL::NoneShaderStage::Upload(const std::string& p_source) const
{
}

template<>
OvRendering::Settings::ShaderCompilationResult OvRendering::HAL::NoneShaderStage::Compile() const
{
	return {
		.success = true
	};
}

template<>
uint32_t OvRendering::HAL::NoneShaderStage::GetID() const
{
	return 0;
}
